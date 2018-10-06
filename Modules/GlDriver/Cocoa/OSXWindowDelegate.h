/** \file GlDriver/Cocoa/OSXWindowDelegate.h
**/

#ifndef GLDRIVER_COCOA_OSXWINDOWDELEGATE_H
#define GLDRIVER_COCOA_OSXWINDOWDELEGATE_H

#include "OSXPrerequisites.h"

class OSXGlRenderWindow;

/** @brief Delegate for NSWindow created by OSXGlRenderWindow. 
 *
 * This delegate's purpose is to handle Window's events and transmit them 
 * to our OSXGlRenderWindow through OSXGlRenderWindow::notify() method. Notes 
 * that this method is accessible only through OBJ-C objects. 
 * Our delegate translates mouse, keys, window move and resize events, and window
 * will close events. 
 *
**/
@interface OSXWindowDelegate : NSObject < NSWindowDelegate >
{
    //! @brief Context we also manage. 
    NSOpenGLContext* _context;
    
    //! @brief RenderWindow we will send events to. 
    OSXGlRenderWindow* _caller;
}

/*! @brief Initializes our delegate. */
- (id)initWithCaller:(OSXGlRenderWindow*)caller glContext:(NSOpenGLContext*)context;

@end

#endif // GLDRIVER_COCOA_OSXWINDOWDELEGATE_H