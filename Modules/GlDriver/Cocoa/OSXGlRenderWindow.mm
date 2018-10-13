/** \file GlDriver/Cocoa/OSXGlRenderWindow.mm
**/

#include "OSXGlRenderWindow.h"
#include "OSXWindowDelegate.h"
#include "OSXGlView.h"
#include "../All/GlCheckError.h"

#include <Clean/NotificationCenter.h>
#include <Clean/Allocate.h>
using namespace Clean;

/*! @brief Converts the given Clean Window Style to NSWindowStyleMask. */
static NSWindowStyleMask OSXGlWindowStyleMask(std::uint16_t style) 
{
    NSWindowStyleMask result = 0;
    
    if (style & kWindowStyleTitled) result |= NSWindowStyleMaskTitled;
    if (style & kWindowStyleResizable) result |= NSWindowStyleMaskResizable;
    if (style & kWindowStyleClosable) result |= NSWindowStyleMaskClosable;
    if (style & kWindowStyleMiniaturizable) result |= NSWindowStyleMaskMiniaturizable;
    
    return result;
}

OSXGlRenderWindow::OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context)
    : nativeWindow(nil), nativeWindowDelegate(nil), nativeContext(nil), fullscreenMode(false)
{
    assert(context && "Null OSXGlContext passed to constructor.");
    
    static constexpr const NSWindowStyleMask defaultMask = NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
                                                           NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;
    
    NSWindow* nsWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 10, 10)
                                                     styleMask:defaultMask
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];
    
    if (!nsWindow) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSWindow couldn't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    NSOpenGLContext* nsContext = (NSOpenGLContext*) context->toNSOpenGLContext();
    OSXGlView* windowView = [[OSXGlView alloc] initWithFrame:[nsWindow frame] andContext:nsContext];
    
    [nsWindow setContentView:windowView];
    [nsContext setView:windowView];
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this glContext:nsContext];
    [nsWindow setDelegate:delegate];
    
    [nsWindow center];
    [nsWindow makeFirstResponder:windowView];
    [nsWindow setAcceptsMouseMovedEvents:YES];
    [nsWindow setRestorable:NO];
    
    nativeWindow = nsWindow;
    nativeWindowDelegate = delegate;
    nativeContext = context;
    nativeView = windowView;
    
    RenderSurface::resetHandles(nsWindow, NULL);
}

OSXGlRenderWindow::OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context, std::size_t width, std::size_t height, std::uint16_t style, std::string const& title)
{
    assert(context && "Null OSXGlContext passed to constructor.");
    
    NSWindowStyleMask windowMask = OSXGlWindowStyleMask(style);
    NSRect windowRect = NSMakeRect(0, 0, (CGFloat)width, (CGFloat)height);
    
    NSWindow* window = [[NSWindow alloc] initWithContentRect:windowRect
                                                   styleMask:windowMask
                                                     backing:NSBackingStoreBuffered
                                                       defer:NO];
    
    if (!window) {
        Notification notif = BuildNotification(kNotificationLevelError, "NSWindow couldn't be created for OSXGlRenderWindow #%i.", getHandle());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    NSOpenGLContext* windowContext = (NSOpenGLContext*) context->toNSOpenGLContext();
    OSXGlView* windowView = [[OSXGlView alloc] initWithFrame:[window frame] andContext:windowContext];
    
    [window setContentView:windowView];
    [windowContext setView:windowView];
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this glContext:windowContext];
    [window setDelegate:delegate];
    
    [window center];
    [window makeFirstResponder:windowView];
    [window setAcceptsMouseMovedEvents:YES];
    [window setRestorable:NO];
    [window setAutodisplay:YES];
    
    nativeWindow = window;
    nativeWindowDelegate = delegate;
    nativeContext = context;
    nativeView = windowView;
    setTitle(title);
    
    RenderSurface::resetHandles(window, NULL);
}

OSXGlRenderWindow::OSXGlRenderWindow(OSXGlRenderWindow const& rhs)
    : nativeWindow(nil), nativeWindowDelegate(nil), nativeContext(nil), fullscreenMode(false)
{   
    static constexpr const NSWindowStyleMask defaultMask = NSWindowStyleMaskTitled|NSWindowStyleMaskClosable|
                                                           NSWindowStyleMaskMiniaturizable|NSWindowStyleMaskResizable;
    
    NSWindow* nsWindow = [[NSWindow alloc] initWithContentRect:NSMakeRect(0, 0, 10, 10)
                                                     styleMask:defaultMask
                                                       backing:NSBackingStoreBuffered
                                                         defer:NO];
    
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
    
    OSXGlView* windowView = [[OSXGlView alloc] initWithFrame:[nsWindow frame] andContext:nsContext];
    
    [nsWindow setContentView:windowView];
    [nsContext setView:windowView];
    
    OSXWindowDelegate* delegate = [[OSXWindowDelegate alloc] initWithCaller:this glContext:nsContext];
    [nsWindow setDelegate:delegate];
    
    [nsWindow center];
    [nsWindow makeFirstResponder:windowView];
    [nsWindow setAcceptsMouseMovedEvents:YES];
    [nsWindow setRestorable:NO];
    
    nativeWindow = nsWindow;
    nativeWindowDelegate = delegate;
    nativeContext = AllocateShared < OSXGlContext >(nsContext);
    nativeView = windowView;
    
    RenderSurface::resetHandles(nsWindow, NULL);
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
    return fullscreenMode.load();
}

void OSXGlRenderWindow::update() 
{
    if (nativeWindow) {
        
        NSEvent* event = nil;
        
        do
        {
            event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                              untilDate:[NSDate distantPast]
                                                 inMode:NSDefaultRunLoopMode
                                                dequeue:YES];
            
            if (event) {
                [NSApp sendEvent:event];
                [nativeWindow update];
            }
            
        } while(event);
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
        GlRenderWindow::bind(driver);
    }
}

std::shared_ptr < OSXGlContext > OSXGlRenderWindow::getOSXGlContext() const
{
    return nativeContext;
}

void OSXGlRenderWindow::show() const 
{
    if (nativeWindow) {
        [nativeWindow orderFront:nil];
        [nativeWindow makeKeyWindow];
        [nativeWindow makeMainWindow];
    }
}

void OSXGlRenderWindow::setFullscreen(bool value) 
{
    if (nativeWindow && (isFullscreen() != value)) {
        [nativeWindow toggleFullScreen:nil];
    }
}

void OSXGlRenderWindow::prepare(Driver& driver) const
{
    if (nativeWindow)
    {
        bind(driver);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        
        glDisable(GL_CULL_FACE);
        
        glClearColor(0.0f, 1.0f, 0.0f, 1.0f);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
        
        NSRect frame = [[nativeWindow contentView] frame];
        glViewport(0, 0, (GLsizei)frame.size.width, (GLsizei)frame.size.height);
    }
}

void OSXGlRenderWindow::notifyClose(id delegate) 
{
    assert((delegate == nativeWindowDelegate) && "Illegal call to OSXGlRenderWindow::notifyClose");
    nativeWindow = nil;
    nativeWindowDelegate = nil;
    nativeContext.reset();
}

void OSXGlRenderWindow::notifyFullscreen(id delegate, bool value)
{
    assert((delegate == nativeWindowDelegate) && "Illegal call to OSXGlRenderWindow::notifyFullscreen");
    fullscreenMode.store(value);
}

std::uint16_t OSXGlRenderWindow::getStyle() const
{
    return kWindowStyleDefault;
}
