/** \file GlDriver/GlDriver.cpp
**/

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>

#include "GlDriver.h"

#ifdef CLEAN_WINDOW_COCOA
#   include "../Cocoa/OSXGlContext.h"
#   include "../Cocoa/OSXGlRenderWindow.h"

#endif

using namespace Clean;

bool GlDriver::initialize() 
{ 
#   ifdef CLEAN_WINDOW_COCOA 
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
        
    defaultWindow = Clean::AllocateShared < OSXGlRenderWindow >(defaultNativeContext);
        
    if (!defaultWindow || !defaultWindow->isValid()) {
        Clean::Notification errNotif = Clean:: BuildNotification(kNotificationLevelError,
            "Driver %s can't create OSXGlRenderWindow.",
            getName().data());
        Clean::NotificationCenter::GetDefault()->send(errNotif);
        defaultContext.reset();
        return false;
    }
    
    loadDefaultShaders();
        
    return true;
        
#   endif
}
    
void GlDriver::destroy()
{
    {
        std::scoped_lock < GlContext, std::mutex > ctxtLock(defaultContext, defaultShadersMapMutex);
        for (auto& pair : defaultShadersMap) {
            pair.second->release();
        }
        
        defaultShadersMap.clear();
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
    return pixFormat;
        
#   endif
}
    
void GlDriver::drawShaderAttributes(std::uint8_t drawingMode, ShaderAttributesMap const& attributes)
{
    switch(drawingMode)
    {
            /*
        case kDrawingModeIndexed:
        glDrawIndexed(...);
        return;
            
        case kDrawingModeNormal:
        case kDrawingModeVertexes:
        glDrawArrays(...);
        return;
             */
    }
}

RenderCommand GlDriver::makeRenderCommand() const 
{
    auto pipeline = AllocateShared < GlRenderPipeline >(defaultContext);
    return { .pipeline = pipeline }; 
}

std::string const GlDriver::getName() const 
{
    return "Clean.GlDriver";
}

std::shared_ptr < Buffer > GlDriver::makeBuffer(std::uint8_t type, std::shared_ptr < Buffer > const& buffer)
{
    std::scoped_lock < GlContext > ctxtLock(*defaultContext);
    
    GLvoid* data = buffer->lock(kIOBufferReadOnly);
    GLsizeiptr size = static_cast < GLsizeiptr >(buffer->getSize());
    GLenum usage = GlBufferUsage(buffer->getUsage());
    
    assert((bufType != GL_INVALID_ENUM) && "Illegal type passed.");
    assert((usage != GL_INVALID_ENUM) && "Illegal buffer usage passed.");
    auto bufHandle = AllocateShared < GlBuffer >(this, buffer->getType(), size, data, usage);
    buffer->unlock(kIOBufferReadOnly);
    
    if (!bufHandle) {
        Notification notif = BuildNotification(kNotificationLevelError, "Can't create GlBuffer from buffer #%i: "
            "\tsize = %i\n\tusage = %i\n\ttype = %i", buffer->getHandle(), size, usage, bufType);
        NotificationCenter::GetDefault()->send(notif);
    }
    
    else {
        Notification notif = BuildNotification(kNotificationLevelInfo, "Created new GlBuffer from buffer #%i: "
            "\tsize = %i\n\tusage = %i\n\ttype = %i", buffer->getHandle(), size, usage, bufType);
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

/** @brief Utility class to map our default vertex shader. */
class DefaultGlslVertexMapper : public ShaderMapper 
{
public:
    /*! @brief Maps the vertex descriptor to our default shader. */
    ShaderAttributesMap map(VertexDescriptor const& descriptor, Shader const& shader) const
    {
        ShaderAttributesMap result(descriptor.indexInfos);
        if (descriptor.indexInfos.elements == 0) result.setElements(descriptor.localSubmesh.elements);
        
        if (descriptor.has(kVertexComponentPosition) && shader.hasAttribute("vert"))
        {
            auto& infos = descriptor.findInfosFor(kVertexComponentPosition);

            ShaderAttribute attrib = ShaderAttribute::Enabled(
                shader.findAttributeIndex("vert"),
                infos.offset, infos.stride, infos.buffer
            );

            result.add(std::move(attrib));
        }
        
        return result;
    }
};

/** @brief Utility class to map our default fragment shader. */
class DefaultGlslFragmentMapper : public ShaderMapper 
{
public:
    /*! @brief Returns an empty ShaderAttributesMap, as fragment shader in GLSL cannot be mapped to any 
     *  attributes. 
    **/
    ShaderAttributesMap map(VertexDescriptor const& descriptor, Shader const& shader) const
    {
        ShaderAttributesMap result(descriptor.indexInfos);
        if (descriptor.indexInfos.elements == 0) result.setElements(descriptor.localSubmesh.elements);
        return result;
    }
};

void GlDriver::loadDefaultShaders()
{
    std::vector < std::shared_ptr < Shader > > defaultShaders;
    
    {
        std::scoped_lock < GlContext > ctxtLock(defaultContext);
    
        const char* defaultGlslVertex = R"(
        
        #version 330
        
        layout(location = 0) in vec4 vert;
    
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
    
        void main() 
        {
            gl_Position = projection * view * model * vert;
        }
        
        )";
    
        auto defaultGlslVertexMapper = AllocateShared < DefaultGlslVertexMapper >();
        auto defaultGlslVertexShader = makeShader(defaultGlslVertex, kBufferTypeVertex, defaultGlslVertexMapper);
        if (defaultGlslVertexShader) {
            defaultGlslVertexShader->retain();
            defaultShaders.push_back(defaultGlslVertexShader);
        }
    
        const char defaultGlslFragment = R"(
        
        #version 330
        
        out vec4 fragColor;
    
        void main() 
        {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        }
    
        )";
    
        auto defaultGlslFragmentMapper = AllocateShared < DefaultGlslFragmentMapper >();
        auto defaultGlslFragmentShader = makeShader(defaultGlslFragment, kBufferTypeFragment, defaultGlslFragmentMapper);
        if (defaultGlslFragmentShader) {
            defaultGlslFragmentShader->retain();
            defaultShaders.push_back(defaultGlslFragmentShader);
        }
    }
    
    std::scoped_lock lck(defaultShadersMapMutex);
    for (auto shader : defaultShaders) {
        defaultShadersMap.insert(std::make_pair(shader->getType(), ReinterpretShared < GlBuffer >(shader)));
    }
}

std::shared_ptr < Shader > GlDriver::makeShader(const char* src, std::uint8_t stage, std::shared_ptr < ShaderMapper > const& mapper)
{
    assert(src && "Illegal empty source given.");
    assert((stage != kBufferTypeNull) && "Illegal null shader's type.");
    
    auto shader = AllocateShared < GlShader >(src, stage);
    if (shader && mapper) shader->setMapper(mapper);
    if (!shader || !shader->isValid()) return nullptr;
    
    shaderManager.add(shader);
    return std::static_pointer_cast < Shader >(shader);
}
