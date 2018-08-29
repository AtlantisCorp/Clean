/** \file Core/DynlibManager.h
**/

#ifndef CLEAN_DYNLIBMANAGER_H
#define CLEAN_DYNLIBMANAGER_H

#include "Dynlib.h"
#include "Manager.h"

namespace Clean 
{
    /*! @brief Manages all Dynlib objects for Core class. */
    class DynlibManager final : public Manager < Dynlib > 
    {
    public:
        
        /*! @brief Default constructor. */
        DynlibManager() = default;
        
        /*! @brief Default destructor. */
        ~DynlibManager() = default;
        
        /*! @brief Finds the first Dynlib object where filepath is given path. */
        std::shared_ptr < Dynlib > findFromFile(std::string const& path);
    };
}

#endif // CLEAN_DYNLIBMANAGER_H