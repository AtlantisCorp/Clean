/** \file GlDriver/GlRenderWindow.h
**/

#ifndef GLDRIVER_GLRENDERWINDOW_H
#define GLDRIVER_GLRENDERWINDOW_H

#include <Clean/RenderWindow.h>

class GlRenderWindow : public Clean::RenderWindow 
{
public:
    
    /*! @brief Constructs the GlRenderWindow. */
    GlRenderWindow() = default;
    
    /*! @brief Destructs the GlRenderWindow. */
    virtual ~GlRenderWindow() = default;
    
    /*! @brief Returns true if valid (initialized). */
    virtual bool isValid() const = 0;
};

#endif // GLDRIVER_GLRENDERWINDOW_H
