/** \file Core/Module.h
**/

#ifndef CLEAN_MODULE_H
#define CLEAN_MODULE_H

#include "Version.h"
#include "Exception.h"
#include "sole.hpp"

#include <string>
#include <atomic>
#include <mutex>

namespace Clean 
{
    //! @brief Callback used to start a module. 
    typedef void (*ModuleStartCbk)();
    //! @brief Callback used to stop a module.
    typedef void (*ModuleStopCbk)();
    
    /** @brief Informations about a module. 
     *
     * This is what a user must return from GetFirstModuleInfos. Remember other defined modules in
     * the same Dynlib are linked by the next field of this structure, giving a basic forward list. 
     *
    **/
    struct ModuleInfos final
    {
        //! @brief Name of this module. May not be unique. [required]
        std::string name;
        
        //! @brief Description of this module. [optional]
        std::string description;
        
        //! @brief Author(s) for this module. [optional]
        std::string author;
        
        //! @brief Version of this module. [required]
        Version version;
        
        //! @brief UUID of this module. [required]
        sole::uuid uuid;
        
        //! @brief Start function. [optional]
        ModuleStartCbk startCallback = nullptr;
        
        //! @brief Stop function. [optional]
        ModuleStopCbk stopCallback = nullptr;
        
        //! @brief Next ModuleInfos structure. [optional]
        ModuleInfos* next = nullptr;
    };
    
    /** @brief Basic exception to notifiate something is wrong about ModuleInfos. */
    class ModuleInfosException : public Exception 
    {
    public:
        using Exception::Exception;
    };
    
    /** @brief Handles informations about a specific module. 
     *
     * A module is an external piece of code, that can be loaded and unloaded, though each of them 
     * is optional. A module is loaded when created by the Core object (\see Core::loadAllModules),
     * and is unloaded at destruction of ModuleManager or at destruction of the Dynlib that owns this
     * module. 
     *
    **/
    class Module final
    {
        //! @brief Informations of this module.
        ModuleInfos infos;
        
        //! @brief Module is started ?
        std::atomic_bool started;
        
        //! @brief Protects start/stop function call, in case it is called from different threads.
        std::mutex callMutex;
        
    public:
        
        /*! @brief Construct a Module from a ModuleInfos. 
         * If something is wrong, it throws ModuleInfosException. 
        **/
        Module(ModuleInfos* iInfos);
        
        /*! @brief Copy constructor. */
        Module(Module const&) = delete;
        
        /*! @brief Destruct the module and stops it if not already stopped. */
        ~Module();
        
        /*! @brief Returns all infos about this module. */
        ModuleInfos const& getInfos() const;
        
        /*! @brief Returns name of this module. */
        std::string const& name() const;
        
        /*! @brief Returns description of this module. */
        std::string const& description() const;
        
        /*! @brief Returns author(s) of this module. */
        std::string const& author() const;
        
        /*! @brief Returns version of this module. */
        Version const& version() const;
        
        /*! @brief Returns uuid of this module. */
        sole::uuid const& uuid() const;
        
        /*! @brief Starts this module. */
        void start();
        
        /*! @brief Stops this module. */
        void stop();
        
        /*! @brief Reload this module (stop then start). */
        void reload();
        
        /*! @brief Returns true if started. */
        bool isStarted() const;
    };
    
    //! @brief Callback to get first module infos. 
    typedef ModuleInfos* (*ModuleGetFirstModuleInfosCbk)();
    
    //! @brief Constant name previous callback must have inside the Dynlib. 
    static constexpr const char* kModuleGetFirstModuleInfosCbk = "GetFirstModuleInfos";
}

#endif // CLEAN_MODULE_H