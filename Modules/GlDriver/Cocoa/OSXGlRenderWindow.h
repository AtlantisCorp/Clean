/** \file GlDriver/Cocoa/OSXGlRenderWindow.h
**/

#ifndef GLDRIVER_COCOA_OSXGLRENDERWINDOW_H
#define GLDRIVER_COCOA_OSXGLRENDERWINDOW_H

#include "OSXPrerequisites.h"
#include "OSXGlContext.h"
#include "../All/GlRenderWindow.h"

class OSXGlRenderWindow : public GlRenderWindow 
{
    //! @brief An id representing a NSWindow.
    id nativeWindow;
    
    //! @brief The nativeWindow delegate. 
    id nativeWindowDelegate;
    
    //! @brief The NSOpenGLContext associated to this render window.
    std::shared_ptr < OSXGlContext > nativeContext;
    
public:
    
    /*! @brief Constructs an OSXGlRenderWindow from the given native context. */
    OSXGlRenderWindow(std::shared_ptr < OSXGlContext > const& context);
    
    /*! @brief Constructs an OSXGlRenderWindow from the given native RenderWindow. */
    OSXGlRenderWindow(OSXGlRenderWindow const& rhs);
    
    /*! @brief Destructs the window. */
    ~OSXGlRenderWindow();
    
    /*! @brief Returns true if all components could be created (native* variables). */
    bool isValid() const;
};

#endif // GLDRIVER_COCOA_OSXGLRENDERWINDOW_H