/** \file Core/RenderSurface.h
**/

#ifndef CLEAN_RENDERSURFACE_H
#define CLEAN_RENDERSURFACE_H

#include "RenderWindow.h"

namespace Clean 
{
    /*! \typedef NativeSurface
     * \brief Defines a standard type for a handle to a native surface. 
     *
     * Basically is void* on every platforms, excepted on Windows where only HANDLE is suitable. However, 
     * if you want to use another window provider, define CLEAN_CUSTOM_NATIVE_WND to whatever you want.
     *
    **/
    
#   ifndef CLEAN_CUSTOM_NATIVE_WND
#       ifdef CLEAN_PLATFORM_WIN
    typedef HANDLE NativeSurface;
    
#       else 
    typedef void* NativeSurface;
    
#       endif
#   else
    typedef CLEAN_CUSTOM_NATIVE_WND NativeSurface;
    
#   endif
    
    /** @brief Experimental attempt for a generic surface that accepts a native parent. */
    class RenderSurface : public RenderWindow 
    {
    protected:
        
        //! @brief Native handle for this surface. 
        std::atomic < NativeSurface > handle;
        
        //! @brief Native parent for this surface. 
        std::atomic < NativeSurface > parent;
        
#       ifdef __cpp_lib_atomic_is_always_lock_free
        static_assert(std::atomic < NativeSurface >::is_always_lock_free, "A lockfree NativeSurface type is required.");
#       endif
        
    public:
        
        /*! @brief No default constructor. */
        RenderSurface() = delete;
        
        /*! @brief Returns native handle. */
        NativeSurface getNativeHandle() const;
        /*! @brief Returns native parent. */
        NativeSurface getNativeParent() const;
    };
}

#endif // CLEAN_RENDERSURFACE_H
