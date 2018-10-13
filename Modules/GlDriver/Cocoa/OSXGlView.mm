/** \file GlDriver/Cocoa/OSXGlView.h
**/

#include "OSXGlView.h"

@implementation OSXGlView

- (id) initWithFrame:(NSRect)frame andContext:(NSOpenGLContext*)context
{
    self = [super initWithFrame:frame];
    
    if (self) {
        context_ = context;
        [[NSNotificationCenter defaultCenter] addObserver:self
                                                 selector:@selector(_surfaceNeedsUpdate:)
                                                     name:NSViewGlobalFrameDidChangeNotification
                                                   object:self];
    }
    
    return self;
}

- (NSOpenGLContext*) openGLContext
{
    return context_;
}

- (BOOL) isOpaque
{
    return YES;
}

- (BOOL) canBecomeKeyView 
{
    return YES;
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) _surfaceNeedsUpdate:(NSNotification*)notification
{
    [self update];
}

- (void) lockFocus
{
    NSOpenGLContext* context = [self openGLContext];
    
    [super lockFocus];
    if ([context view] != self) {
        [context setView:self];
    }
    
    [context makeCurrentContext];
}

- (void) viewDidMoveToWindow
{
    NSOpenGLContext* context = [self openGLContext];
    
    [super viewDidMoveToWindow];
    if ([self window] == nil) {
        [context clearDrawable];
    }
}

- (void) update
{
    NSOpenGLContext* context = [self openGLContext];
    
    if (![self inLiveResize]) {
        [context update];
    }
}

@end
