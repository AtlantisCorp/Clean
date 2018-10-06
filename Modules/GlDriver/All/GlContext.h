/** \file GlDriver/All/GlContext.h
**/

#ifndef GLDRIVER_GLCONTEXT_H
#define GLDRIVER_GLCONTEXT_H

#include <Clean/Handled.h>
#include <Clean/PixelFormat.h>

/** @brief Interface for an OpenGL Context wrapper. 
 *
 * Implemented upon the selected window service, as WINGlContext, OSXGlContext, X11GlContext
 * or WlGlContext (some other implementations may be used).
 *
**/
class GlContext : public Clean::Handled < GlContext >
{
public:
    
    /*! @brief Destructs the context. */
    virtual ~GlContext() = default;
    
    /*! @brief Returns true if context is valid. */
    virtual bool isValid() const = 0;
    
    /*! @brief 'Locks' the context. In fact, it uses makeCurrent() and, if available, lock the context 
     *  for multithreading purpose (OSX uses NSOpenGLContext::lock).
    **/
    virtual void lock() const = 0;
    
    /*! @brief 'Unlocks' the context. Actually does nothing, except on OSX where it calls NSOpenGLContext::unlock 
     *  only for multithreading purpose. 
    **/
    virtual void unlock() const = 0;
    
    /*! @brief Returns the current pixel format for this context. */
    virtual Clean::PixelFormat getPixelFormat() const = 0;
};

#endif // GLDRIVER_GLCONTEXT_H
