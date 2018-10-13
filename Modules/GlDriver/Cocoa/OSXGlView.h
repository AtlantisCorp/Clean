/** \file GlDriver/Cocoa/OSXGlView.h
**/

#ifndef GLDRIVER_COCOA_OSXGLVIEW_H
#define GLDRIVER_COCOA_OSXGLVIEW_H

#include "OSXPrerequisites.h"

@interface OSXGlView : NSView 
{
@private
    //! @brief Our OpenGL Context we manage.
    NSOpenGLContext* context_;
}

/*! @brief Initializes the view with an OpenGL Context. Must be valid. */
- (id) initWithFrame:(NSRect)frame andContext:(NSOpenGLContext*)context;

/*! @brief Returns the OpenGL Context associated. */
- (NSOpenGLContext*) openGLContext;

@end

#endif // GLDRIVER_COCOA_OSXGLVIEW_H
