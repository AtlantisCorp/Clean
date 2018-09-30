/** \file GlDriver/Cocoa/OSXGlRenderWindow.mm
**/

#include "OSXGlRenderWindow.h"
#include <Clean/NotificationCenter.h>
using namespace Clean;

OSXGlRenderWindow::OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context)
    : nativeWindow(nil), nativeWindowDelegate(nil), nativeContext(nil)
{
    assert(context && "Null OSXGlContext passed to constructor.");
    static constexpr const NSWindowStyleMask defaultMask = NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
                                                           NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;
    
    NSWindow* nsWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 10, 10)
                                                     styleMask:defaultMask
                                                       backing:NSBackingStoreBuffered
                                                         defer:YES];
    
    if (!nsWindow) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSWindow couldn't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this];
    [nsWindow setDelegate:delegate];
    
    NSOpenGLContext* nsContext = (NSOpenGLContext*) context->toNSOpenGLContext();
    [nsContext setView:[nsWindow contentView]];
    
    nativeWindow = nsWindow;
    nativeWindowDelegate = delegate;
    nativeContext = context;
}

OSXGlRenderWindow::OSXGlRenderWindow(OSXGlRenderWindow const& rhs)
    : nativeWindow(nil), nativeWindowDelegate(nil), nativeContext(nil)
{
    static constexpr const NSWindowStyleMask defaultMask = NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
                                                           NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;
    
    NSWindow* nsWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 10, 10)
                                                     styleMask:defaultMask
                                                       backing:NSBackingStoreBuffered
                                                         defer:YES];
    
    if (!nsWindow) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSWindow couldn't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this];
    [nsWindow setDelegate:delegate];
    
    NSOpenGLContext* nsRhsContext = (NSOpenGLContext*) rhs.getOSXGlContext()->toNSOpenGLContext();
    NSOpenGLContext* nsContext = [[NSOpenGLContext alloc] initWithFormat:[nsRhsContext pixelFormat] shareContext:nsRhsContext];
    
    if (!nsContext) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSOpenGLContext could't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    [nsContext setView:[nsWindow contentView]];
    
    nativeWindow = nsWindow;
    nativeWindowDelegate = delegate;
    nativeContext = AllocateShared < OSXGlContext >(nsContext);
}

OSXGlRenderWindow::~OSXGlRenderWindow()
{
    if (nativeWindow)
        [nativeWindow close];
    
    nativeWindow = nil;
    nativeWindowDelegate = nil;
}

bool OSXGlRenderWindow::isValid() const 
{
    return nativeWindow && nativeWindowDelegate && nativeContext;
}