/** \file Core/Core.cpp
**/

#include "Core.h"
#include "Allocate.h"
#include "Exception.h"

namespace Clean 
{
    std::unique_ptr < Core > Core::instance = nullptr;
    std::atomic_flag Core::once;
    
    Core& Core::Create(std::shared_ptr < NotificationListener > const& listener)
    {
        if (!once.test_and_set())
        {
            instance = std::make_unique < Core >(Allocate < Core >(1));
            assert(instance && "Invalid allocation of Clean::Core.");
            instance->getNotificationCenter()->addListener(listener);
        }
        
        if (!instance) throw NullPointerException("Null Core::instance pointer.");
        return *instance;
    }
    
    Core& Core::Get()
    {
        if (!instance) throw NullPointerException("Null Core::instance pointer.");
        return *instance;
    }
    
    Core::Core()
    {
        notificationCenter = AllocateShared < NotificationCenter >();
        assert(notificationCenter && "Can't allocate Clean::NotificationCenter.");
        
        moduleManager = AllocateShared < ModuleManager >();
        assert(moduleManager && "Can't allocate Clean::ModuleManager.");
        
        dynlibManager = AllocateShared < DynlibManager >();
        assert(dynlibManager && "Can't allocate Clean::DynlibManager.");
        
        modulesDirectories.push_back("Modules");
        loadAllModules();
    }
    
    Core::~Core()
    {
        
    }
    
    std::shared_ptr < NotificationCenter > Core::getNotificationCenter()
    {
        return notificationCenter;
    }
    
    std::size_t Core::loadAllModules(std::uint8_t const& loadMode)
    {
        std::lock_guard < std::mutex > lck(modulesDirMutex);
        assert(dynlibManager && moduleManager && "Invalid managers.");
        std::size_t result = 0;
        
        for (auto& dir : modulesDirectories)
        {
            std::string path = Platform::PathConcatenate(dir, std::string("*.") + kDynlibFileExtension);
            auto files = Platform::FindFiles(path, Platform::kFindFilesNotRecursive);
            
            for (auto& file : files)
            {
                auto dynlib = dynlibManager->findFromFile(file);
                
                if (dynlib && (loadMode == kModulesLoadReload))
                {
                    dynlib.forEachModules([](Module& module){ 
                        module.reload(); 
                    });
                }
                
                else if (!dynlib)
                {
                    try 
                    {
                        dynlib = AllocateShared < Dynlib >(file);
                        dynlibManager->add(dynlib);
                        
                        ModuleGetFirstModuleInfosCbk callback = (ModuleGetFirstModuleInfosCbk) dynlib->symbol(kModuleGetFirstModuleInfosCbk);
                        
                        if (callback)
                        {
                            ModuleInfos* infos = callback();
                            
                            do 
                            {
                                try
                                {
                                    auto module = AllocateShared < Module >(infos);
                                    module->start();
                                    dynlib->addModule(module);
                                    moduleManager->add(module);
                                    result++;
                                }
                                
                                catch(ModuleInfosException const& e)
                                {
                                    Notification notif = BuildNotification(kNotificationLevelError, 
                                                                           "Module %s cannot be loaded: %s", 
                                                                           infos->name, e.what());
                                    notificationCenter->send(notif);
                                }
                            }
                            while((infos = infos->next) != NULL)
                        }
                    }
                    
                    catch(DynlibLoadException const& e)
                    {
                        Notification notif = BuildNotification(kNotificationLevelError,
                                                               "Dynlib %s cannot be loaded: %s",
                                                               file.data(), e.what());
                        notificationCenter->send(notif);
                    }
                }
            }
        }
        
        return result;
    }
    
    std::size_t Core::getModuleCount() const 
    {
        return moduleManager->count();
    }
}