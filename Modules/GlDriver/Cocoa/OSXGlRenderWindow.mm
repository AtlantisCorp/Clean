/** \file GlDriver/Cocoa/OSXGlRenderWindow.mm
**/

#include "OSXGlRenderWindow.h"
#include "OSXWindowDelegate.h"

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
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
    
    NSOpenGLContext* nsContext = (NSOpenGLContext*) context->toNSOpenGLContext();
    [nsContext setView:[nsWindow contentView]];
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this glContext:nsContext];
    [nsWindow setDelegate:delegate];
    
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
    
    NSOpenGLContext* nsRhsContext = (NSOpenGLContext*) rhs.getOSXGlContext()->toNSOpenGLContext();
    NSOpenGLContext* nsContext = [[NSOpenGLContext alloc] initWithFormat:[nsRhsContext pixelFormat] shareContext:nsRhsContext];
    
    if (!nsContext) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSOpenGLContext could't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    [nsContext setView:[nsWindow contentView]];
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this glContext:nsContext];
    [nsWindow setDelegate:delegate];
    
    nativeWindow = nsWindow;
    nativeWindowDelegate = delegate;
    nativeContext = AllocateShared < OSXGlContext >(nsContext);
}

OSXGlRenderWindow::~OSXGlRenderWindow()
{
    if (nativeWindow) {
        [nativeWindow close];
    }
    
    nativeWindow = nil;
    nativeWindowDelegate = nil;
}

bool OSXGlRenderWindow::isValid() const 
{
    return nativeWindow && nativeWindowDelegate && nativeContext;
}

std::string OSXGlRenderWindow::getTitle() const 
{
    if (nativeWindow) {
        NSString* title = [nativeWindow title];
        return std::string([title cStringUsingEncoding:NSUTF8StringEncoding], 
                           [title lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
    }
    
    return std::string();
}

bool OSXGlRenderWindow::isFullscreen() const 
{
    return false;
}

void OSXGlRenderWindow::update() 
{
    if (nativeWindow) {
        NSEvent* event = [nativeWindow nextEventMatchingMask:NSEventMaskAny
                                                   untilDate:[NSDate distantFuture]
                                                      inMode:NSDefaultRunLoopMode
                                                     dequeue:YES];
        
        if (event) {
            [nativeWindow sendEvent:event];
        }
        
        [nativeWindow update];
    }
}

void OSXGlRenderWindow::draw()
{
    // NOTES: Do nothing.
}

void OSXGlRenderWindow::destroy()
{
    close();
    nativeWindow = nil;
    nativeWindowDelegate = nil;
    nativeContext.reset();
}

void OSXGlRenderWindow::close()
{
    if (nativeWindow) {
        [nativeWindow performClose:nil];
    }
}

void OSXGlRenderWindow::hide()
{
    if (nativeWindow) {
        [nativeWindow miniaturize:nil];
    }
}

void OSXGlRenderWindow::unhide()
{
    if (nativeWindow) {
        [nativeWindow deminiaturize:nil];
    }
}

void OSXGlRenderWindow::setTitle(std::string const& title)
{
    if (nativeWindow) {
        NSString* newTitle = [NSString stringWithCString:title.data() encoding:NSUTF8StringEncoding];
        assert(newTitle && "Illegal null NSString generated.");
        [nativeWindow setTitle:newTitle];
    }
}

void OSXGlRenderWindow::move(std::size_t x, std::size_t y)
{
    if (nativeWindow) {
        NSRect frame = [nativeWindow frame];
        frame.origin.x = (CGFloat)x;
        frame.origin.y = (CGFloat)y;
        [nativeWindow setFrame:frame display:YES animate:YES];
    }
}

WindowPosition OSXGlRenderWindow::getPosition() const 
{
    if (nativeWindow) {
        NSRect frame = [nativeWindow frame];
        return { (std::size_t)frame.origin.x, (std::size_t)frame.origin.y };
    }
    
    return { 0, 0 };
}

WindowSize OSXGlRenderWindow::getSize() const 
{
    if (nativeWindow) {
        NSRect frame = [nativeWindow frame];
        return { (std::size_t)frame.size.width, (std::size_t)frame.size.height };
    }
    
    return { 0, 0 };
}

void OSXGlRenderWindow::resize(std::size_t width, std::size_t height)
{
    if (nativeWindow) {
        NSRect frame = [nativeWindow frame];
        frame.size.width = (CGFloat)width;
        frame.size.height = (CGFloat)height;
        [nativeWindow setFrame:frame display:YES animate:YES];
    }
}

bool OSXGlRenderWindow::isClosed() const 
{
    return !isValid();
}

std::size_t OSXGlRenderWindow::getBuffersCount() const 
{
    if (!nativeContext) return 0;
    NSOpenGLPixelFormat* pixelFormat = nativeContext->getNSOpenGLPixelFormat();
    assert(pixelFormat && "Illegal null NSOpenGLPixelFormat.");
    
    GLint isDoubleBuffer, isTripleBuffer;
    [pixelFormat getValues:&isDoubleBuffer forAttribute:NSOpenGLPFADoubleBuffer forVirtualScreen:0];
    [pixelFormat getValues:&isTripleBuffer forAttribute:NSOpenGLPFATripleBuffer forVirtualScreen:0];
    
    if (isDoubleBuffer) return 2;
    if (isTripleBuffer) return 3;
    return 1;
}

void OSXGlRenderWindow::swapBuffers()
{
    if (nativeContext) {
        nativeContext->swapBuffers();
    }
}

void OSXGlRenderWindow::lock()
{
    if (nativeContext) {
        nativeContext->lock();
    }
}

void OSXGlRenderWindow::unlock()
{
    if (nativeContext) {
        nativeContext->unlock();
    }
}

void OSXGlRenderWindow::bind(Driver& driver) const
{
    if (nativeContext) {
        nativeContext->makeCurrent();
    }
}

std::shared_ptr < OSXGlContext > OSXGlRenderWindow::getOSXGlContext() const
{
    return nativeContext;
}

void OSXGlRenderWindow::notifyClose(id delegate) 
{
    assert((delegate == nativeWindowDelegate) && "Illegal call to OSXGlRenderWindow::notifyClose");
    nativeWindow = nil;
    nativeWindowDelegate = nil;
    nativeContext.reset();
}

std::uint16_t OSXGlRenderWindow::getStyle() const
{
    return kWindowStyleDefault;
}
