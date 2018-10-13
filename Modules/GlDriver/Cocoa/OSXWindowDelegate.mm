/** \file GlDriver/Cocoa/OSXWindowDelegate.mm
**/

#include "OSXWindowDelegate.h"
#include "OSXGlRenderWindow.h"

@implementation OSXWindowDelegate 

- (id)initWithCaller:(OSXGlRenderWindow*)caller glContext:(NSOpenGLContext*)context
{
    assert(caller && "Illegal null OSXGlRenderWindow.");
    assert(context && "Illegal null NSOpenGLContext.");
    self = [super init];
    
    if (self != nil) {
        _caller = caller;
        _context = context;
    }
    
    return self;
}

- (void)windowDidUpdate:(NSNotification*)notification
{
    [_context update];
}

/*! @brief Called when the Window did update. 
 * We must call NSOpenGLContext::update to ensure our GL context is correctly updated. 
**/
- (void)windowWillClose:(NSNotification*)notification
{
    _caller->notifyClose((id)self);
}

/*! @brief Calls OSXGlRenderWindow::notifyFullscreen(true). */
- (void)windowDidEnterFullscreen:(NSNotification*)notification
{
    _caller->notifyFullscreen(self, true);
}

/*! @brief Calls OSXGlRenderWindow::notifyFullscreen(false). */
- (void)windowDidExitFullscreen:(NSNotification*)notification
{
    _caller->notifyFullscreen(self, false);
}

- (void)windowDidMove:(NSNotification *)notification
{
    
}

- (void)windowDidBecomeMain:(NSNotification *)notification
{
    
}

@end
