/** \file GlDriver/All/GlContext.h
**/

#ifndef GLDRIVER_GLCONTEXT_H
#define GLDRIVER_GLCONTEXT_H

/** @brief Interface for an OpenGL Context wrapper. 
 *
 * Implemented upon the selected window service, as WINGlContext, OSXGlContext, X11GlContext
 * or WlGlContext (some other implementations may be used).
 *
**/
class GlContext 
{
public:
    
    /*! @brief Destructs the context. */
    virtual ~GlContext() = default;
    
    /*! @brief Returns true if context is valid. */
    virtual bool isValid() const = 0;
};

#endif // GLDRIVER_GLCONTEXT_H
