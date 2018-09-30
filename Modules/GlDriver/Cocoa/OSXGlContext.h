/** \file GlDriver/Cocoa/OSXGlContext.h
**/

#ifndef GLDRIVER_COCOA_OSXGLCONTEXT_H
#define GLDRIVER_COCOA_OSXGLCONTEXT_H

#include "OSXPrerequisites.h"
#include "../All/GlContext.h"

#include <Clean/PixelFormat.h>

class OSXGlContext : public GlContext 
{
    //! @brief NSOpenGLContext object to store our GL Context. 
    id context;
    
public:
    
    /*! @brief Constructs the OSXGlContext from the given PixelFormat object. May stores
     *  an invalid context object if PixelFormat is invalid too.
     *
     * \param[in] pixelFormat Format we want our OpenGL Context to have. Typically the closest
     *      available pixel format is used. 
     *
    **/
    OSXGlContext(Clean::PixelFormat const& pixelFormat);
    
    /*! @brief Constructs the OSXGlContext from the given PixelFormat object and shares its
     *  resources with the given context if possible. 
     *
     * \param[in] pixelFormat Format we want our OpenGL Context to have. Typically the closest
     *      available pixel format is used. 
     * \param[in] sharedContext Context to share resources with. 
     *
    **/
    OSXGlContext(Clean::PixelFormat const& pixelFormat, OSXGlContext const& sharedContext);
    
#ifdef CLEAN_LANG_OBJC
    /*! @brief Constructs the OSXGlContext with the given NSOpenGLContext. */
    OSXGlContext(NSOpenGLContext* natContext);
        
#endif
    
    /*! @brief Destructs the OpenGL Context and all its resources associated. */
    ~OSXGlContext();
    
    /*! @brief Returns true if context holds a valid NSOpenGLContext object. */
    bool isValid() const;
    
#ifdef CLEAN_LANG_OBJC
    /*! @brief Returns the NSOpenGLContext associated. 
     * 
     * Function not available when compiling C++ files because of ARC retain system. Under OBJ-C
     * compilation, ARC will retain one count for returning a copy of our id, which will not be 
     * released from the C++ point of view. So disabled under C++.
    **/
    id toNSOpenGLContext() const;
    
#endif
};

#endif // GLDRIVER_COCOA_OSXGLCONTEXT_H