/** \file Core/DriverManager.h
**/

#ifndef CLEAN_DRIVERMANAGER_H
#define CLEAN_DRIVERMANAGER_H

#include "Manager.h"
#include "Driver.h"

namespace Clean 
{
    /** @brief Specialization of Manager for Driver. */
    class DriverManager : public Manager < Driver >
    {
    public:
        
        /*! @brief Finds a Driver with given name. */
        std::shared_ptr < Driver > findDriverByName(std::string const& name);
    };
}

#endif // CLEAN_DRIVERMANAGER_H