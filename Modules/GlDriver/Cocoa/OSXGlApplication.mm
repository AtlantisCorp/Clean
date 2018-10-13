/** \file GlDriver/Cocoa/OSXGlApplication.mm
**/

#include "OSXGlApplication.h"
#include "OSXGlApplicationDelegate.h"

@implementation OSXGlApplication

// From http://cocoadev.com/index.pl?GameKeyboardHandlingAlmost
// This works around an AppKit bug, where key up events while holding
// down the command key don't get sent to the key window.
- (void)sendEvent:(NSEvent *)event
{
    if ([event type] == NSEventTypeKeyUp &&
        ([event modifierFlags] & NSEventModifierFlagCommand))
    {
        [[self keyWindow] sendEvent:event];
    }
    else
        [super sendEvent:event];
}


/*! @brief Thread-entry point. */
- (void)doNothing:(id)object
{
    
}

@end

void OSXGlCheckNSApplicationRunning()
{
    if (NSApp == nil)
    {
        // We must create an application, a delegate for this application and run this application. When the
        // delegate receives the NSApplicationDidStart (or something like that), it stops the application from
        // running and we handle ourselves the event loop.
        NSApplication* current = [OSXGlApplication sharedApplication];
        OSXGlApplicationDelegate* delegate = [[OSXGlApplicationDelegate alloc] init];
        
        // In case we are unbundled, make us a proper UI application
        [current setActivationPolicy:NSApplicationActivationPolicyRegular];
        
        // Sets our delegate and run NSApplication (for like 1usec).
        [current setDelegate:delegate];
        
        // Forces our NSApplication to be multithreaded.
        [NSThread detachNewThreadSelector:@selector(doNothing:)
                                 toTarget:current
                               withObject:nil];
        
        [current run];
    }
}
