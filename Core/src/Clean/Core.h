/** \file Core/Core.h
**/

#ifndef CLEAN_CORE_H
#define CLEAN_CORE_H

#include "NotificationCenter.h"
#include "NotificationListener.h"
#include "ModuleManager.h"
#include "DynlibManager.h"
#include "WindowManager.h"

#include <memory>
#include <atomic>
#include <list>
#include <mutex>

namespace Clean 
{
    //! @brief Loads all modules and reloads those that were already loaded.
    static constexpr const std::uint8_t kModulesLoadReload = 0;
    
    //! @brief Loads all modules but not those who were already loaded. 
    static constexpr const std::uint8_t kModulesLoadNoReload = 1;
    
    /** @brief Base class for the Clean engine. 
     *
     * Created by a user, Core is the base class of the Clean engine. It lets all other
     * class access all subsystems of the engine, like NotificationCenter, other Modules, 
     * registered Drivers, etc. 
     *
     * By default, Modules are loaded under the 'Modules' directory. However, you can submit
     * other directories with addModulesDirectories(). You must use loadAllModules(kModulesLoadNoReload)
     * to load new modules and not reloading already loaded modules. 
     * 
     * Modules are loaded in two parts: first the file is loaded through Clean::Dynlib object, registered
     * into the DynlibManager. Then, 'GetFirstModuleInfos()' is called from the dynamic library. All modules
     * for this file are loaded from this structure into the ModuleManager. 
     *
     * Core's manipulation function should be called from main thread. However, all Core's getters can be 
     * called from any thread. Managers are threadsafe and can be used from any threads.
     *
     * Modules are destroyed with the destruction of the ModuleManager. However, DynlibManager may unload 
     * its dynamic libraries before ModuleManager. To prevent this, Dynlib stores all Modules registered in
     * ModuleManager for this particular Dynlib and calls 'Module::unload()' if the Modules are not already 
     * destroyed by the ModuleManager. (Modules are stored as std::weak_ptr inside Dynlib.)
     *
     * Modules unloading and lifetime of Core::Driver 
     *
     * When a module is unloaded, if it has registered a Core::Driver to the DriverManager, it must unregister
     * it at this moment. Two methods can be used: by directly unregistering it from the module's stop function,
     * or by registering a listener to the module's ModuleWillStop event and unregister the driver at this moment. 
     * In both ways, the driver will be destroyed only when all shared_ptr holding the driver will be destroyed. 
     * It means that unregistering the driver will not necessarily destroy it. However, module's unloading means 
     * Dynlib unloading and thus, the piece of code that manages the driver will be unlinked. This is why Driver
     * should always implement Driver::destroy() instead of releasing its resources in the destructor. 
     *
    **/
    class Core final
    {
        //! @brief NotificationCenter used by the Core object. 
        std::shared_ptr < NotificationCenter > notificationCenter;
        
        //! @brief Modules manager. 
        std::shared_ptr < ModuleManager > moduleManager;
        
        //! @brief Dynamic library manager. 
        std::shared_ptr < DynlibManager > dynlibManager;
        
        //! @brief Driver manager.
        std::shared_ptr < DriverManager > driverManager;
        
        //! @brief Window Manager instance. 
        std::shared_ptr < WindowManager > windowManager;
        
        //! @brief Directories used to load modules. 
        std::list < std::string > modulesDirectories;
        
        //! @brief Protects modulesDirectories. 
        std::mutex modulesDirMutex;
        
        //! @brief Core instance. Initialized once by Create().
        static std::unique_ptr < Core > instance;
        
        //! @brief Once flag to ensure instance is created only once. 
        static std::atomic_flag once;
        
    public:
        
        /*! @brief Creates the Core object. 
         *
         * \param listener Adds a NotificationListener to the newly created NotificationCenter. 
         *      Acts as a shortcut to Core::Get().getNotificationCenter()->addListener().
         *
         * \return A reference to the Core object. Throws an exception if instance cannot be 
         *      created, or if still null. 
        **/
        static Core& Create(std::shared_ptr < NotificationListener > const& listener = nullptr);
        
        /*! @brief Returns a reference to the Core object. 
         *
         * Throws an exception if instance is null. It may be the case if the user did not call 
         * Create() yet, thus this function must be called after the first Create(). 
        **/
        static Core& Get();
        
    public:
        
        /*! @brief Constructs the Core object. */
        Core();
        
        /*! @brief Destruct the Core object. */
        ~Core();
        
        /*! @brief Returns the NotificationCenter. */
        std::shared_ptr < NotificationCenter > getNotificationCenter();
        
        /*! @brief Loads all modules in modulesDirectories. 
         * 
         * \param loadMode Can be one of kModulesLoadReload or kModulesLoadNoReload. If first,
         *      all modules are unloaded and modules are all loaded as if never loaded before. 
         *      If second, already loaded modules are not reloaded. 
         * 
         * \note When adding a new module directory, use kModulesLoadNoReload to load only new
         * modules from that directory. 
         * 
         * \return Number of modules loaded. 
        **/
        std::size_t loadAllModules(std::uint8_t const& loadMode = kModulesLoadNoReload);
        
        /*! @brief Returns number of modules in moduleManager. */
        std::size_t getModuleCount() const;
        
        /*! @brief Finds a driver with given name. */
        std::shared_ptr < Driver > findDriver(std::string const& name);
    };
}

#endif // CLEAN_CORE_H