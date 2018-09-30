/** \file GlDriver/GlDriver.h
**/

#ifndef GLDRIVER_GLDRIVER_H
#define GLDRIVER_GLDRIVER_H

#include <Clean/Driver.h>

#include "GlContext.h"
#include "GlRenderWindow.h"

class GlDriver : public Clean::Driver 
{
    //! @brief Our main OpenGL Context. This is where all begins. It may be associated to a 
    //! main window if necessary (like on Windows), but not always (like on MAC OS). 
    std::shared_ptr < GlContext > defaultContext;
    
    //! @brief Our main OpenGL Window, if created. OpenGL is always associated to one window
    //! which represents the default framebuffer. When 'makeRenderWindow' is called, the first
    //! render window will be this one and other will have another context shared with defaultContext.
    std::shared_ptr < GlRenderWindow > defaultWindow;
    
    //! @brief Protects defaultContext and defaultWindow.
    mutable std::mutex defaultsMutex;
    
public:
    
    bool initialize();
    
    void destroy();
    
    Clean::PixelFormat selectPixelFormat(Clean::PixelFormat const& pixFormat, Clean::PixelFormatPolicy policy = Clean::kPixelFormatClosest);
    
    void drawShaderAttributes(std::uint8_t drawingMode, Clean::ShaderAttributesMap const& attributes);
};

#endif // GLDRIVER_GLDRIVER_H
