/** \file Core/DriverResource.cpp
**/

#include "DriverResource.h"
#include "Driver.h"

namespace Clean 
{
    DriverResource::DriverResource(Driver* driver) 
        : counter(0), creator(driver), released(false)
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
            if (creator) {
                if (creator->shouldReleaseResource(*this)) {
                    releaseResource();
                }
            }
            
            else {
                releaseResource();
            }
        }
    }
}