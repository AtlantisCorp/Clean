/** =======================================================
 *  \file Core/MaterialManager.h
 *  \date 10/16/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_MATERIALMANAGER_H
#define CLEAN_MATERIALMANAGER_H

#include "Manager.h"
#include "Material.h"

namespace Clean 
{
    class MaterialManager : public Manager < Material >
    {
        //! @brief Current pointer to the global MaterialManager. 
        static std::atomic < MaterialManager* > currentManager;
        
        //! @brief Makes our Core class a friend. 
        friend class Core;
        
    public:
        
        /*! @brief Returns the current manager or throw an exception if not found. */
        static MaterialManager& Current();
        
    public:
        
        /*! @brief Default constructor. */
        MaterialManager() = default;
        
        /*! @brief Loads one or multiple materials from a file and returns them. */
        std::vector < std::shared_ptr < Material > > load(std::string const& filepath);
        
        /*! @brief Finds the first Material corresponding to name. */
        std::shared_ptr < Material > findByName(std::string const& name) const;
    };
}

#endif // CLEAN_MATERIALMANAGER_H