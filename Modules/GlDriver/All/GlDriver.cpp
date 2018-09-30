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
        
    return true;
        
#   endif
}
    
void GlDriver::destroy()
{
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
