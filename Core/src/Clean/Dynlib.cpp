/** \file Core/Dynlib.cpp
**/

#include "Dynlib.h"

#ifdef CLEAN_PLATFORM_WIN32
std::string DlError()
{
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError();
    
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, 
        dw, 
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
        (LPTSTR)&lpMsgBuf, 
        0, NULL);
    
    auto result = std::string((const char*)lpMsgBuf);
    LocalFree(lpMsgBuf);
    
    return result;
}

#endif // CLEAN_PLATFORM_*

namespace Clean 
{
    Dynlib::Dynlib(std::string const& filepath)
    {
        handle = DlLoad(filepath.data());
        file = filepath;
        
        if (!handle)
        {
            std::string error = DlError();
            throw DynlibLoadException(error);
        }
    }
    
    Dynlib::~Dynlib()
    {
        assert(handle && "Invalid DlHandle stored.");
        forEachModules([](Module& module) { module.stop(); });
        DlClose(handle);
    }
    
    void Dynlib::addModule(std::shared_ptr < Module > const& module)
    {
        std::lock_guard < std::mutex > lck(modulesMutex);
        modules.push_back(module);
    }
    
    void Dynlib::forEachModules(DynlibForEachModuleCbk callback)
    {
        std::lock_guard < std::mutex > lck(modulesMutex);
        
        for (auto& wmodule : modules)
        {
            auto smodule = wmodule.lock();
            if (smodule) callback(*smodule);
        }
    }
    
    void* Dynlib::symbol(const char* sym)
    {
        return DlSym(handle, sym);
    }
    
    std::string const& Dynlib::getFilepath() const 
    {
        return file;
    }
}