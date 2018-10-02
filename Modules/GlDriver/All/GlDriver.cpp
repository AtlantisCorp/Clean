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

/** @brief Utility class to map our default vertex shader. */
class DefaultGlslMapper : public ShaderMapper 
{
public:
    /*! @brief Maps the vertex descriptor to our default shader. */
    ShaderAttributesMap map(VertexDescriptor const& descriptor, RenderPipeline const& shader) const
    {
        ShaderAttributesMap result(descriptor.indexInfos);
        if (descriptor.indexInfos.elements == 0) result.setElements(descriptor.localSubmesh.elements);
        
        if (descriptor.has(kVertexComponentPosition) && shader.hasAttribute("position"))
        {
            auto& infos = descriptor.findInfosFor(kVertexComponentPosition);

            ShaderAttribute attrib = ShaderAttribute::Enabled(
                shader.findAttributeIndex("position"),
                infos.offset, infos.stride, infos.buffer
            );

            result.add(std::move(attrib));
        }
        
        return result;
    }
};

RenderCommand GlDriver::makeRenderCommand() const 
{
    auto pipeline = AllocateShared < GlRenderPipeline >(defaultContext);
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

void GlDriver::loadDefaultShaders()
{
    std::vector < std::shared_ptr < Shader > > defaultShaders;
    
    {
        std::scoped_lock < GlContext > ctxtLock(defaultContext);
    
        const char* defaultGlslVertex = R"(
        
        #version 330
        
        layout(location = 0) in vec4 position;
    
        uniform mat4 projection;
        uniform mat4 view;
        uniform mat4 model;
    
        void main() 
        {
            gl_Position = projection * view * model * vert;
        }
        
        )";
    
        auto defaultGlslVertexShader = makeShader(defaultGlslVertex, kBufferTypeVertex);
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
        
        auto defaultGlslFragmentShader = makeShader(defaultGlslFragment, kBufferTypeFragment);
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

std::shared_ptr < Shader > GlDriver::makeShader(const char* src, std::uint8_t stage)
{
    assert(src && "Illegal empty source given.");
    assert((stage != kBufferTypeNull) && "Illegal null shader's type.");
    std::scoped_lock < GlContext const > lck(*defaultContext);
    
    auto shader = AllocateShared < GlShader >(src, stage);
    if (!shader || !shader->isValid()) return nullptr;
    
    shaderManager.add(shader);
    return std::static_pointer_cast < Shader >(shader);
}

std::shared_ptr < RenderWindow > GlDriver::_createRenderWindow(std::size_t width, std::size_t height, 
    std::string const& title, std::uint16_t style, bool fullscreen) const 
{
#   ifdef CLEAN_WINDOW_OSX
    std::scoped_lock < std::mutex > lck(defaultsMutex);
    if (defaultWindow.use_count() == 1) {
        defaultWindow->setSize(width, height);
        defaultWindow->setStyle(style);
        defaultWindow->setTitle(title);
        defaultWindow->setFullscreen(fullscreen);
        return defaultWindow;
    }
    
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
    
    auto result = AllocateShared < OSXGlRenderWindow >(sharedContext, width, height, style, title);
    if (!result || !result->isValid()) {
        Notification notif = BuildNotification(kNotificationLevelError, "OSXGlRenderWindow failed creation from context #%i.", 
            sharedContext->getHandle());
        NotificationCenter::GetDefault()->send(notif);
        sharedContext->unlock();
        return nullptr;
    }
    
    sharedContext->unlock();
    renderWindows.add(std::static_pointer_cast < RenderWindow >(result));
    return result;
    
#   endif
}

std::shared_ptr < RenderSurface > GlDriver::_createRenderSurface(std::size_t width, std::size_t height, NativeSurface parent) const
{
#   ifdef CLEAN_WINDOW_OSX
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
    
    auto result = AllocateShared < OSXGlRenderSurface >(sharedContext, width, height, parent);
    if (!result || !result->isValid()) {
        Notification notif = BuildNotification(kNotificationLevelError, "OSXGlRenderSurface failed creation from context #%i.", 
            sharedContext->getHandle());
        NotificationCenter::GetDefault()->send(notif);
        sharedContext->unlock();
        return nullptr;
    }
    
    sharedContext->unlock();
    renderWindows.add(std::static_pointer_cast < RenderWindow >(result));
    return result;
    
#   endif
}

std::shared_ptr < RenderQueue > GlDriver::_createRenderQueue(std::uint8_t type) const
{
    return AllocateShared < RenderQueue >(type);
}
