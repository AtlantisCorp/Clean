/** \file Core/Core.cpp
**/

#include "Core.h"
#include "Allocate.h"
#include "Exception.h"
#include "Platform.h"

namespace Clean
{
    std::unique_ptr < Core > Core::instance = nullptr;
    std::atomic_flag Core::once;

    Core& Core::Create(std::shared_ptr < NotificationListener > const& listener)
    {
        if (!once.test_and_set())
        {
            instance.reset(Allocate < Core >(1));
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
        std::atomic_store(&NotificationCenter::defaultCenter, notificationCenter);

        moduleManager = AllocateShared < ModuleManager >();
        assert(moduleManager && "Can't allocate Clean::ModuleManager.");

        dynlibManager = AllocateShared < DynlibManager >();
        assert(dynlibManager && "Can't allocate Clean::DynlibManager.");

        windowManager = AllocateShared < WindowManager >();
        assert(windowManager && "Can't allocate Clean::WindowManager.");
        
        driverManager = AllocateShared < DriverManager >();
        assert(driverManager && "Can't allocate Clean::DriverManager.");
        
        MeshManager::currentManager.store(&meshManager);
        assert(MeshManager::currentManager.load() && "Can't store Clean::MeshManager.");
        
        MaterialManager::currentManager.store(&materialManager);
        assert(MaterialManager::currentManager.load() && "Can't store Clean::MeshManager.");

        modulesDirectories.push_back("Modules");
        fileSystem.addRealPath("Module", "Modules");
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
                    dynlib->forEachModules([](Module& module){
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
                                                                           infos->name.data(), e.what());
                                    notificationCenter->send(notif);
                                }
                            }
                            while((infos = infos->next) != NULL);
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

    std::shared_ptr < Driver > Core::findDriver(std::string const& name)
    {
        return driverManager->findDriverByName(name);
    }

    void Core::clearFileLoaders()
    {
        std::scoped_lock < std::mutex > lck(fileLoadersMutex);
        fileLoaders.clear();
    }
    
    std::shared_ptr < WindowManager > Core::getWindowManager()
    {
        return std::atomic_load(&windowManager);
    }
    
    void Core::addDriver(std::shared_ptr < Driver > const& driver)
    {
        auto loadedManager = std::atomic_load(&driverManager);
        assert(loadedManager && "Null DriverManager.");
        assert(driver && "Illegal null driver passed.");
        loadedManager->add(driver);
    }
    
    FileSystem& Core::getCurrentFileSystem()
    {
        return fileSystem;
    }
    
    void Core::destroy()
    {
        NotificationCenter::GetDefault()->exitLoopThread = true;
        
        {
            std::unique_lock < std::mutex > lock(NotificationCenter::GetDefault()->condLoopThreadMutex);
            NotificationCenter::GetDefault()->condLoopThread.notify_all();
        }
        
        NotificationCenter::GetDefault()->loopThread.join();
        
        clearFileLoaders();
        materialManager.reset();
        meshManager.reset();
        windowManager.reset();
        driverManager.reset();
        moduleManager.reset();
        dynlibManager.reset();
    }
    
    MeshManager& Core::getMeshManager() 
    {
        return meshManager;
    }
    
    MaterialManager& Core::getMaterialManager()
    {
        return materialManager;
    }
}
