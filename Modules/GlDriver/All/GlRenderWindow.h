/** \file GlDriver/GlRenderWindow.h
**/

#ifndef GLDRIVER_GLRENDERWINDOW_H
#define GLDRIVER_GLRENDERWINDOW_H

#include "GlInclude.h"

#include <Clean/RenderWindow.h>

class GlRenderWindow : public Clean::RenderWindow 
{
    //! @brief Our VAO.
    GLuint vao;
    
public:
    
    /*! @brief Constructs the GlRenderWindow. */
    GlRenderWindow();
    
    /*! @brief Destructs the GlRenderWindow. */
    virtual ~GlRenderWindow();
    
    /*! @brief Returns true if valid (initialized). */
    virtual bool isValid() const = 0;
    
    /*! @brief Binds the current VAO. */
    virtual void bind(Clean::Driver& driver) const;
};

#endif // GLDRIVER_GLRENDERWINDOW_H
