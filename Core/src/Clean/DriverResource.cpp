/** \file Core/DriverResource.cpp
**/

#include "DriverResource.h"
#include "Driver.h"

namespace Clean 
{
    DriverResource::DriverResource(Driver* creator)
        : counter(0), driver(creator), released(false)
    {
        
    }
    
    void DriverResource::retain() 
    {
        counter.fetch_add(1);
    }
    
    void DriverResource::release() 
    {
        auto currentCount = counter.fetch_sub(1) - 1;
        
        if (!currentCount) {
            if (driver) {
                if (driver->shouldReleaseResource(*this)) {
                    releaseResource();
                }
            }
            
            else {
                releaseResource();
            }
        }
    }
    
    Driver& DriverResource::getDriver()
    {
        assert(driver && "Null Driver. Did this Resource has been created with a valid Driver?");
        return *driver;
    }
}
