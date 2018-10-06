/** \file Core/RenderSurface.h
**/

#ifndef CLEAN_RENDERSURFACE_H
#define CLEAN_RENDERSURFACE_H

#include <atomic>

namespace Clean 
{
    /*! \typedef NativeSurface
     *  \brief Defines a standard type for a handle to a native surface.
    **/
    typedef void* NativeSurface;
    
    /** @brief Experimental attempt for a generic surface that accepts a native parent. 
     *
     * This base class is only a representation of a basic surface. It has only a handle and a parent. It can 
     * be used to create custom surfaces that are not windows, as using a custom NSView as a parent. However,
     * the Driver has no control over it and you are responsible for creation, management and destruction of the
     * custom surface. 
     *
    **/
    class RenderSurface 
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
        
        /*! @brief Creates a RenderSurface. 
         *
         * \param[in] handle Handle to store the surface. It can be anything as it represents a void*. Some
         *      system may use different handle representation however it should always fit into a pointer sized
         *      type. 
         *
         * \param[in] parent Handle to the parent of this surface. May be null if the surface has no parent. 
         *
        **/
        RenderSurface(NativeSurface handle, NativeSurface parent);
        
        /*! @brief Returns native handle. */
        NativeSurface getNativeHandle() const;
        
        /*! @brief Returns native parent. */
        NativeSurface getNativeParent() const;
        
    protected:
        
        /*! @brief Changes handle and parent handle. */
        void resetHandles(NativeSurface handle, NativeSurface parent);
    };
}

#endif // CLEAN_RENDERSURFACE_H
