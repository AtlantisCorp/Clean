/** \file GlDriver/GlCheckError.h
**/

#ifndef GLDRIVER_GLCHECKERROR_H
#define GLDRIVER_GLCHECKERROR_H

#include "GlInclude.h"
#include <string>

/** @brief Generic structure to hold an OpenGL error. */
struct GlError 
{
    //! @brief GLenum value of the error. 
    GLenum error = GL_NO_ERROR;
    
    //! @brief Sring description of the error.
    std::string string;
};

/*! @brief Checks for the current OpenGL error by calling glGetError(). */
GlError GlCheckError(void);

#endif // GLDRIVER_GLCHECKERROR_H
