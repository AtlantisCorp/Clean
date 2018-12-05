/** \file GlDriver/GlDriver.cpp
**/

#include "GlDriver.h"
#include "GlRenderPipeline.h"
#include "GlRenderQueue.h"
#include "GlCheckError.h"
#include "GlTexture.h"

#ifdef CLEAN_WINDOW_COCOA
#   include "../Cocoa/OSXGlContext.h"
#   include "../Cocoa/OSXGlRenderWindow.h"
#   include "../Cocoa/OSXGlApplication.h"

#elif defined(CLEAN_WINDOW_WIN32)
#   include "../Win32/WinGlInclude.h"
#   include "../Win32/WinGlContext.h"
#   include "../Win32/WinGlRenderWindow.h"

#endif

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
#include <Clean/VertexDescriptor.h>
using namespace Clean;

bool GlDriver::initialize() 
{ 
#   ifdef CLEAN_WINDOW_COCOA 
    OSXGlCheckNSApplicationRunning();
    
    std::scoped_lock < std::mutex, std::mutex > lck(defaultsMutex, pixelFormatMutex); 
    auto defaultNativeContext = Clean::AllocateShared < OSXGlContext >(pixelFormat); 
    defaultContext = std::static_pointer_cast < GlContext >(defaultNativeContext);
        
    if (!defaultContext || !defaultContext->isValid()) {
        Clean::Notification errNotif = Clean:: BuildNotification(kNotificationLevelError,
            "Driver %s can't create OSXGlContext.",
            getName().data());
        Clean::NotificationCenter::GetDefault()->send(errNotif);
        return false;
    }
        
    defaultWindow = nullptr;
    
    OSXGlFillGlTable(glTable);
    loadDefaultShaders();
    loadDefaultGlStates();

    state = kDriverStateInited;
        
    return true;

#   elif defined(CLEAN_WINDOW_WIN32)

    // Asserts libraries are loaded and filled.

    WinGlLoadLibraries();
    
    // When we initialize OPENGL on Windows, we must create a blank context where we can extract 
    // our WGL functions. Those functions will makes us able to load a real context with the correct
    // desired OPENGL version.

    if (!WinGlRegisterWindowClass())
        return false;

    hiddenWindow = WinGlCreateHiddenWindow(&deviceNotifHandle);
    if (!hiddenWindow) return false;

    // At this point we have a valid Window with which we must load WGL function pointers. To achieve this,
    // we create a new context and get our functions pointer.

    WinGlMakeWGLTable(hiddenWindow, wglTable);

    // Now we can create our default context, with the correct PixelFormat submitted by the user. This context
    // will be tied to our hidden window and shared with all other contexts.

    hiddenContext = WinGlMakeContext(hiddenWindow, pixelFormat, NULL, wglTable);

    // Now we must get some informations about the current context. To do this, we load the GlPtrTable with the 
    // HINSTANCE found inside WGlPtrTable and this context.
    
    wglTable.makeCurrent(GetDC(hiddenWindow), hiddenContext);
    WinGlMakeGLTable(wglTable, glTable);

    // Create our default context to pass it to other objects.

    std::scoped_lock < std::mutex, std::mutex > lck(defaultsMutex, pixelFormatMutex); 
    defaultContext = AllocateShared < WinGlContext >(hiddenWindow, hiddenContext, wglTable, pixelFormat);

    if (!defaultContext->isValid())
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested context.", 0);
        NotificationCenter::GetDefault()->send(notif);
        return false;
    }

    // Now we can load our shaders. 

    defaultContext->makeCurrent();
    loadDefaultShaders();

    state.store(kDriverStateInited);
    return true;

#   endif
}
    
void GlDriver::destroy()
{
    {
        defaultContext->lock();
        std::scoped_lock < std::mutex > ctxtLock(defaultShadersMapMutex);
        for (auto& pair : defaultShadersMap) {
            pair.second->release();
        }
        
        defaultShadersMap.clear();    
        defaultContext->unlock();
    }
    
    std::scoped_lock < std::mutex > lck(defaultsMutex);
    defaultWindow.reset();
    defaultContext.reset();
}
    
PixelFormat GlDriver::selectPixelFormat(PixelFormat const& pixFormat, PixelFormatPolicy policy)
{
#   ifdef CLEAN_WINDOW_COCOA
    // NOTE: NSOpenGLPixelFormat already implements this automatically. We don't need to call CGLChoosePixelFormat
    // or something similar to provide a correct pixel format. However, NSOpenGLPixelFormat is still null if the
    // given PixelFormat is not valid.
    pixelFormat = pixFormat;
    return pixFormat;

#   elif defined(CLEAN_WINDOW_WIN32)

    pixelFormat = pixFormat;
    return pixFormat;
        
#   endif
}
    
void GlDriver::drawShaderAttributes(ShaderAttributesMap const& attributes)
{
    IndexedInfos indexInfos = attributes.getIndexedInfos();
    
    if (indexInfos.elements && indexInfos.buffer) {
        
        GLvoid* pointer = NULL;
        
        if (indexInfos.buffer->isBindable()) {
            indexInfos.buffer->bind(*this);
        } else {
            pointer = (GLvoid*) indexInfos.buffer->lock(kBufferIOReadOnly);
        }
        
        glTable.drawElements(GL_TRIANGLES, indexInfos.elements, GL_UNSIGNED_INT, pointer);
        
        if (indexInfos.buffer->isBindable()) {
            indexInfos.buffer->unbind(*this);
        } else {
            indexInfos.buffer->unlock(kBufferIOReadOnly);
        }
    }
    
    else if (attributes.getElements()) {
        glTable.drawArrays(GL_TRIANGLES, 0, attributes.getElements());
    }
}

/** @brief Utility class to map our default vertex shader. */
class DefaultGlslMapper : public ShaderMapper 
{
public:
    /*! @brief Maps the vertex descriptor to our default shader. */
    ShaderAttributesMap map(VertexDescriptor const& descriptor, RenderPipeline const& shader) const
    {
        ShaderAttributesMap result(descriptor.indexInfos);
        result.setElements(descriptor.localSubmesh.elements);
        
        if (descriptor.has(kVertexComponentPosition) && shader.hasAttribute("position"))
        {
            VertexComponentInfos infos = descriptor.findInfosFor(kVertexComponentPosition);

            ShaderAttribute attrib = ShaderAttribute::Enabled(
                shader.findAttributeIndex("position"), kShaderAttribFloat, 4,
                infos.offset, infos.stride, infos.buffer
            );

            result.add(std::move(attrib));
        }
        
        else if (descriptor.has(kVertexComponentNormal) && shader.hasAttribute("normal"))
        {
            VertexComponentInfos infos = descriptor.findInfosFor(kVertexComponentNormal);
            
            ShaderAttribute attrib = ShaderAttribute::Enabled(
                shader.findAttributeIndex("normal"), kShaderAttribFloat, 4,
                infos.offset, infos.stride, infos.buffer
            );
            
            result.add(std::move(attrib));
        }
        
        return result;
    }
    
    /*! @brief Maps generic uniforms to our shader. */
    ShaderParameter map(EffectParameter const& param, RenderPipeline const& pipeline) const 
    {
        switch (param.hash)
        {
            case kEffectProjectionMat4Hash:
            return ShaderParameter(kShaderParamMat4, "projection", 3, param.value);
            
            case kEffectViewMat4Hash:
            return ShaderParameter(kShaderParamMat4, "view", 11, param.value);
            
            case kEffectModelMat4Hash:
            return ShaderParameter(kShaderParamMat4, "model", 7, param.value);
            
            case kEffectMaterialAmbientVec4Hash:
            return ShaderParameter(kShaderParamVec4, "material.ambient", 0, param.value);
            
            case kEffectMaterialDiffuseVec4Hash:
            return ShaderParameter(kShaderParamVec4, "material.diffuse", 1, param.value);
            
            case kEffectMaterialSpecularVec4Hash:
            return ShaderParameter(kShaderParamVec4, "material.specular", 2, param.value);
                
            case kEffectMaterialEmissiveVec4Hash:
            return ShaderParameter(kShaderParamVec4, "material.emissive", -1, param.value);
            
            default:
            return ShaderMapper::map(param, pipeline);
        }
    }
};

RenderCommand GlDriver::makeRenderCommand() 
{
    auto pipeline = AllocateShared < GlRenderPipeline >(this, glTable);
    auto defaultGlslMapper = AllocateShared < DefaultGlslMapper >();
    pipeline->setMapper(defaultGlslMapper);
    
    return { .pipeline = pipeline }; 
}

std::string const GlDriver::getName() const 
{
    return "Clean.GlDriver";
}

std::shared_ptr < Buffer > GlDriver::makeBuffer(std::uint8_t type, std::shared_ptr < Buffer > const& buffer)
{
    std::scoped_lock < GlContext const > ctxtLock(*defaultContext);
    
    GLvoid* data = buffer->lock(kBufferIOReadOnly);
    GLsizeiptr size = static_cast < GLsizeiptr >(buffer->getSize());
    GLenum usage = GlBufferUsage(buffer->getUsage());
    
    assert((usage != GL_INVALID_ENUM) && "Illegal buffer usage passed.");
    auto bufHandle = AllocateShared < GlBuffer >(this, buffer->getType(), size, data, usage);
    buffer->unlock(kBufferIOReadOnly);
    
    if (!bufHandle) {
        Notification notif = BuildNotification(kNotificationLevelError, "Can't create GlBuffer from buffer #%i: "
            "\tsize = %i\n\tusage = %i\n\ttype = %i", buffer->getHandle(), size, usage, buffer->getType());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    else {
        Notification notif = BuildNotification(kNotificationLevelInfo, "Created new GlBuffer from buffer #%i: "
            "\tsize = %i\n\tusage = %i\n\ttype = %i", buffer->getHandle(), size, usage, buffer->getType());
        NotificationCenter::GetDefault()->send(notif);
        bufferManager.add(bufHandle);
    }
    
    return std::static_pointer_cast < Buffer >(bufHandle);
}

std::shared_ptr < Shader > GlDriver::findDefaultShaderForStage(std::uint8_t stage) const 
{
    std::scoped_lock < std::mutex > lck(defaultShadersMapMutex);
    auto it = defaultShadersMap.find(stage);
    if (it != defaultShadersMap.end()) return std::static_pointer_cast < Shader >(it->second);
    return nullptr;
}

void GlDriver::loadDefaultShaders()
{
    std::vector < std::shared_ptr < Shader > > defaultShaders;
    
    {
        defaultContext->lock();
        
        const char* defaultGlslVertex = R"(
        
        #version 330 core
        
        layout(location = 0) in vec4 position;
        layout(location = 1) in vec4 aNormal;
        
        out vec3 Normal;
    
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
    
        void main() 
        {
            gl_Position = projection * view * model * position;
            Normal = aNormal.xyz;
        }
        
        )";
    
        auto defaultGlslVertexShader = makeShader(defaultGlslVertex, kShaderTypeVertex);
        if (defaultGlslVertexShader) {
            defaultGlslVertexShader->retain();
            defaultShaders.push_back(defaultGlslVertexShader);
        }
    
        const char* defaultGlslFragment = R"(
        
        #version 330 core
        
        in vec3 Normal;
        out vec4 FragColor;
        
        struct Material 
        {
            vec4 ambient;
            vec4 diffuse;
            vec4 specular;
        };
        
        uniform Material material;
    
        void main() 
        {
            FragColor = material.ambient + material.diffuse + material.specular;
        }
    
        )";
        
        auto defaultGlslFragmentShader = makeShader(defaultGlslFragment, kShaderTypeFragment);
        if (defaultGlslFragmentShader) {
            defaultGlslFragmentShader->retain();
            defaultShaders.push_back(defaultGlslFragmentShader);
        }
        
        defaultContext->unlock();
    }
    
    std::scoped_lock lck(defaultShadersMapMutex);
    for (auto shader : defaultShaders) {
        defaultShadersMap.insert(std::make_pair(shader->getType(), ReinterpretShared < GlShader >(shader)));
    }
}

std::shared_ptr < Shader > GlDriver::makeShader(const char* src, std::uint8_t stage)
{
    assert(src && "Illegal empty source given.");
    assert((stage != kShaderTypeNull) && "Illegal null shader's type.");
    std::scoped_lock < GlContext const > lck(*defaultContext);
    
    auto shader = AllocateShared < GlShader >(src, stage, glTable);
    if (!shader || !shader->isValid()) return nullptr;
    
    shaderManager.add(shader);
    return std::static_pointer_cast < Shader >(shader);
}

std::shared_ptr < Shader > GlDriver::findShaderPath(std::string const& origin) const 
{
    return shaderManager.findByPath(origin);
}

std::shared_ptr < Texture > GlDriver::makeTexture(std::shared_ptr < Image > const& image)
{
    GLenum target = GL_TEXTURE_2D;
    GLuint handle; glTable.genTextures(1, &handle);
    assert(target && handle && "glGenTextures failed.");
    
    auto texture = AllocateShared < GlTexture >(this, handle, target, glTable);
    assert(texture && "Clean::AllocateShared failed (maybe memory is not available?).");
    
    if (!texture->upload(image)) 
    {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Texture #%i was unable to upload data from Image #%i.", 
            texture->getHandle(), image->getHandle()));
        return nullptr;
    }
    
    texture->retain();
    textureManager.add(std::static_pointer_cast < Texture >(texture));
    
    return std::static_pointer_cast < Texture >(texture);
}

std::shared_ptr < RenderWindow > GlDriver::_createRenderWindow(std::size_t width, std::size_t height, 
    std::string const& title, std::uint16_t style, bool fullscreen) const 
{
#   ifdef CLEAN_WINDOW_COCOA
    std::scoped_lock < std::mutex > lck(defaultsMutex);
    auto currentContext = ReinterpretShared < OSXGlContext >(defaultContext);
    currentContext->lock();
    
    auto sharedContext = AllocateShared < OSXGlContext >(currentContext->getPixelFormat(), *currentContext);
    if (!sharedContext || !sharedContext->isValid()) {
        Notification notif = BuildNotification(kNotificationLevelError, "OSXGlContext creation failed copy from context #%i.",
            defaultContext->getHandle());
        NotificationCenter::GetDefault()->send(notif);
        currentContext->unlock();
        return nullptr;
    }
    
    currentContext->unlock();
    sharedContext->lock();
    
    auto result = AllocateShared < OSXGlRenderWindow >(sharedContext, width, height, style, title, glTable);
    if (!result || !result->isValid()) {
        Notification notif = BuildNotification(kNotificationLevelError, "OSXGlRenderWindow failed creation from context #%i.", 
            sharedContext->getHandle());
        NotificationCenter::GetDefault()->send(notif);
        sharedContext->unlock();
        return nullptr;
    }
    
    result->setFullscreen(fullscreen);
    sharedContext->unlock();
    return result;

#   elif defined(CLEAN_WINDOW_WIN32)
    assert(hiddenWindow && hiddenContext);

    std::scoped_lock < std::mutex > lck(defaultsMutex);
    defaultContext->makeCurrent();

    // Creates our window before creating a shared context.

    HWND windowHandle = WinGlMakeWindow(width, height, title, style, fullscreen);
    auto window = AllocateShared < WinGlRenderWindow >(windowHandle, GetDC(windowHandle));

    if (!window->isValid())
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested window %s.", title.data());
        NotificationCenter::GetDefault()->send(notif);
        return nullptr;
    }

    // Creates our shared context for our new window.

    HGLRC sharedContextHandle = WinGlMakeContext(windowHandle, pixelFormat, hiddenContext, wglTable);
    auto sharedContext = AllocateShared < WinGlContext >(windowHandle, sharedContextHandle, wglTable, pixelFormat);

    if (!sharedContext->isValid())
    {
        Notification notif = BuildNotification(kNotificationLevelError, "WGL: Failed to create requested window context %s.", title.data());
        NotificationCenter::GetDefault()->send(notif);
        return nullptr;
    }

    window->setContext(sharedContext);
    return window; 
    
#   else
#   error "GlDriver::_createRenderWindow not implemented on your platform."
    
#   endif
}

std::shared_ptr < RenderQueue > GlDriver::_createRenderQueue(std::uint8_t type) const
{
    return AllocateShared < GlRenderQueue >(type);
}

void GlDriver::loadDefaultGlStates()
{
    glTable.enable(GL_DEPTH_TEST);
}
