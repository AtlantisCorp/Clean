/** \file Core/DriverResource.h
**/

#ifndef CLEAN_DRIVERRESOURCE_H
#define CLEAN_DRIVERRESOURCE_H

#include <atomic>
#include <cstddef>

namespace Clean 
{
    class Driver;
    
    /** @brief Generic representation of a resource created (or made) by a driver. 
     *
     * A DriverResource is created and managed by its creator, a specific Driver. Thus, as it is actually
     * stored as a shared_ptr object in DriverResource, releasing of the resource cannot be tied to destruction
     * of shared_ptr. Instead, we chose to handle our own reference counter. When someone stores this resource 
     * externally from the driver, it calls 'DriverResource::retain()'. When it destroys one instance, it calls
     * 'DriverResource::release()'. 
     * 
     * \note All derived of DriverResource might not be related to a Driver. As example, GenBuffer is derived
     * from Buffer which is derived from DriverResource. However, GenBuffer is not related to any Driver. 
     * 
    **/
    class DriverResource
    {
        friend class Driver; 
        
        //! @brief Our reference counter. 
        std::atomic < std::size_t > counter;
        
    protected:
        
        //! @brief A pointer to a nullable driver. When this resource is created by a driver, it must
        //! be non-null and \ref Driver::shouldReleaseResource is called as a delegate. 
        Driver* driver;
        
        //! @brief True if \ref releaseResource is called. 
        std::atomic_bool released;
        
    public:
        
        /*! @brief Constructs the resource. */
        DriverResource(Driver* creator = nullptr);
        
        /*! @brief Destructs the resource. */
        virtual ~DriverResource() = default;
        
        /*! @brief Retains the counter. */
        void retain();
        
        /*! @brief Releases the counter. 
         * 
         * \note If counter is zero and Driver::shouldReleaseResource returns true, then virtual releaseResource()
         * is called as an internal to release this resource. 
         *
        **/
        void release();
        
    protected:
        
        /*! @brief Implementation of the real resource releasing. 
         *
         * \note 
         * Releasing the resource should always be followed by storing value true to \ref released,
         * in order to communicate to other objects if this resource is already released or not. 
         *
        **/
        virtual void releaseResource() = 0;
        
        /*! @brief Returns a reference to the actual driver which created this resource.
         *
         * \note This operation asserts the driver is not null before returning it. If no driver
         * has been set for this resource, this function fails.
         *
        **/
        Driver& getDriver();
    };
}

#endif // CLEAN_DRIVERRESOURCE_H
