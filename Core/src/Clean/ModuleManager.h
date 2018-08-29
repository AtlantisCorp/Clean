/** \file Core/ModuleManager.h
**/

#ifndef CLEAN_MODULEMANAGER_H
#define CLEAN_MODULEMANAGER_H

#include "Manager.h"
#include "Module.h"

namespace Clean 
{
    /** @brief Manages modules loaded in the Engine. 
     *
     * At destruction, all modules are unloaded. However, if a dynamic library has already 
     * unloaded the module, it is only destroyed (freed from memory) and unload is not called
     * again, preventing multiple unloading. 
     *
    **/
    class ModuleManager final : public Manager < Module >
    {
    public:
        
        /*! @brief Default constructor. */
        ModuleManager() = default;
        
        /*! @brief Default destructor. */
        ~ModuleManager() = default;
        
        /*! @brief Finds a module by its name. */
        std::shared_ptr < Module > findByName(std::string const& name);
        
        /*! @brief Finds a module by its UUID. */
        std::shared_ptr < Module > findByUUID(sole::uuid const& uuid);
    };
}

#endif // CLEAN_MODULEMANAGER_H