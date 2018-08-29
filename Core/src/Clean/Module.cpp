/** \file Core/Module.cpp
**/

#include "Module.h"

namespace Clean
{
    Module::Module(ModuleInfos* iInfos) : started(false)
    {
        if (!iInfos) throw ModuleInfosException("Null ModuleInfos given.");
        infos = *iInfos;
    }
    
    Module::~Module()
    {
        stop();
    }
    
    ModuleInfos const& Module::getInfos() const
    {
        return infos;
    }
    
    std::string const& Module::name() const 
    {
        return infos.name;
    }
    
    std::string const& Module::description() const 
    {
        return infos.description;
    }
    
    std::string const& Module::author() const 
    {
        return infos.author;
    }
    
    Version const& Module::version() const 
    {
        return infos.version;
    }
    
    sole::uuid const& Module::uuid() const
    {
        return infos.uuid;
    }
    
    void Module::start()
    {
        if (!started && infos.startCallback)
        {
            std::lock_guard < std::mutex > lck(callMutex);
            infos.startCallback();
            started.store(true);
        }
    }
    
    void Module::stop()
    {
        if (started && infos.stopCallback)
        {
            std::lock_guard < std::mutex > lck(callMutex);
            infos.stopCallback();
            started.store(false);
        }
    }
    
    void Module::reload()
    {
        stop();
        start();
    }
    
    bool Module::isStarted() const 
    {
        return started.load();
    }
}