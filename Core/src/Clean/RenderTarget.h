/** \file Core/RenderTarget.h
**/

#ifndef CLEAN_RENDERTARGET_H
#define CLEAN_RENDERTARGET_H

namespace Clean 
{
    class Driver;
    
    /** @brief Generic RenderTarget.
     *
     * A RenderTarget is a target where the driver can render things. It can be a RenderWindow, a RenderSurface,
     * a RenderToTexture or event your own RenderTarget derived. 
     *
     * A RenderTarget has the capabilities to be locked and unlocked in the same manner of a simple mutex. However,
     * this has nothing to do with threading. A RenderTarget is locked when it can be used to render things on it (
     * it is part of the current pipeline states) and unlocked when it cannot be used to render things. 
     *
    **/
    class RenderTarget 
    {
    public:
        RenderTarget() = default;
        RenderTarget(RenderTarget&&) = default;
        RenderTarget(RenderTarget const&) = delete;
        virtual ~RenderTarget() = default;
        
        /*! @brief Locks the target for further use. */
        virtual void lock() = 0;
        
        /*! @brief Unlocks the target for other targets to be used. */
        virtual void unlock() = 0;
        
        /*! @brief Binds this target onto the given driver. */
        virtual void bind(Driver& driver) const = 0;
        
        /*! @brief Prepares the RenderTarget for drawing. It can be setting the viewport, clearing buffers,
         *  or anything that should be done before drawing to it. */
        virtual void prepare(Driver& driver) const = 0;
    };
}

#endif // CLEAN_RENDERTARGET_H
