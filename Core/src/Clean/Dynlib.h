/** \file Core/Dynlib.h
**/

#ifndef CLEAN_DYNLIB_H
#define CLEAN_DYNLIB_H

#include "Module.h"
#include "Exception.h"

#include <list>
#include <mutex>
#include <memory>

#ifdef CLEAN_PLATFORM_WIN32
#include <windows.h>
#define DlHandle HMODULE
#define DlLoad(a) ::LoadLibrary(a)
#define DlUnload(a) ::FreeLibrary(a)
#define DlSym(a, b) ::GetProcAddress(a, b)
static constexpr const char* kDynlibFileExtension = "dll";

/*! @brief On Windows, DlError uses GetLastError() and FormatMessage() to acquire a valid
 * error message. Thus, a macro cannot be used and a function is used instead. */
std::string DlError();

#elif defined(CLEAN_PLATFORM_MACOS)
#include <dlfcn.h>
#define DlHandle void*
#define DlLoad(a) ::dlopen(a, RTLD_LAZY)
#define DlUnload(a) ::dlclose(a)
#define DlSym(a, b) ::dlsym(a, b)
#define DlError() ::dlerror()
static constexpr const char* kDynlibFileExtension = "dylib";

#else
#include <dlfcn.h>
#define DlHandle void*
#define DlLoad(a) ::dlopen(a, RTLD_LAZY)
#define DlUnload(a) ::dlclose(a)
#define DlSym(a, b) ::dlsym(a, b)
#define DlError() ::dlerror()
static constexpr const char* kDynlibFileExtension = "so";

#endif // CLEAN_PLATFORM_*

namespace Clean 
{
    //! @brief Callback for Dynlib::forEachModules.
    typedef void (*DynlibForEachModuleCbk) (Module&);
    
    /** @brief Exception emitted when an error occured while loading a Dynlib. */
    class DynlibLoadException : public Exception
    {
    public:
        using Exception::Exception;
    };
    
    /** @brief Handles in a low level way a dynamic library load at runtime. */
    class Dynlib final
    {
        //! @brief Handle to the dynamic library. 
        DlHandle handle;
        
        //! @brief Loaded Module associated to this dynlib.
        std::list < std::weak_ptr < Module > > modules;
        
        //! @brief Protects modules.
        std::mutex modulesMutex;
        
        //! @brief File from which this dynamic library was called from. 
        std::string file;
        
    public:
        
        /*! @brief Construct a dynamic library and loads it. May throw a DynlibLoadException 
         * on error. 
        **/
        Dynlib(std::string const& filepath);
        
        /*! @brief Destructs the dynamic library. If modules are registered in this library, calls
         * Module::stop() for each of them before closing the library. 
        **/
        ~Dynlib();
        
        /*! @brief Adds a module watched by this library. */
        void addModule(std::shared_ptr < Module > const& module);
        
        /*! @brief Make something for each module present in this library. */
        void forEachModules(DynlibForEachModuleCbk callback);
        
        /*! @brief Finds and return the symbol for given string, or NULL if not found. */
        void* symbol(const char* sym);
        
        /*! @brief Returns file. */
        std::string const& getFilepath() const;
    };
}

#endif // CLEAN_DYNLIB_H