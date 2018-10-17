/** =======================================================
 *  \file Core/MeshManager.cpp
 *  \date 10/16/2018
 *  \author luk2010
    ======================================================= **/

#include "MeshManager.h"
#include "Core.h"
#include "Platform.h"

namespace Clean 
{
    std::atomic < MeshManager* > MeshManager::currentManager = nullptr;
    
    MeshManager& MeshManager::Current()
    {
        MeshManager* manager = currentManager.load();
        assert(manager && "Null Current MeshManager. Perhaps Core class is not created yet.");
        return *manager;
    }
    
    std::shared_ptr < Mesh > MeshManager::load(std::string const& filepath, std::function < bool(FileLoader<Mesh>const&) > checker)
    {
        auto checked = findByFile(filepath);
        if (checked) return checked;
        
        std::string const extension = Platform::PathGetExtension(filepath);
        assert(!extension.empty() && "File must have a valid extension.");
        
        auto loader = Core::Get().findFileLoader < Mesh >(extension);
        if (!loader) {
            Notification notif = BuildNotification(kNotificationLevelWarning, "No FileLoader found to load Mesh file '%s'.", filepath.data());
            NotificationCenter::GetDefault()->send(notif);
            return nullptr;
        }
        
        if (checker) {
            if (!checker(*loader)) {
                Notification notif = BuildNotification(kNotificationLevelWarning, "Checker furnished to load file '%s' returned false.",
                                                       filepath.data());
                NotificationCenter::GetDefault()->send(notif);
                return nullptr;
            }
        }
        
        auto result = loader->load(filepath);
        
        if (!result) {
            Notification notif = BuildNotification(kNotificationLevelError, "FileLoader %s cannot load file '%s'.", 
                                                   loader->getInfos().name.data(), filepath.data());
            NotificationCenter::GetDefault()->send(notif);
            return nullptr;
        }
        
        add(result);
        return result;
    }
    
    std::shared_ptr < Mesh > MeshManager::findByFile(std::string const& filepath) const
    {
        if (filepath.empty())
            return nullptr;
        
        std::string const realPath = Core::Get().getCurrentFileSystem().findRealPath(filepath);
        if (realPath.empty()) return nullptr;
        
        std::lock_guard < std::mutex > lck(managedListMutex);
        auto it = std::find_if(managedList.begin(), managedList.end(), [realPath](auto mesh){ return mesh->getFilePath() == realPath; });
        
        return (it == managedList.end()) ? nullptr : *it;
    }
}
