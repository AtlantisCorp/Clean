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

- (void)windowWillClose:(NSNotification*)notification
{
    _caller->notifyClose((id)self);
}

@end
