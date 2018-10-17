/** =======================================================
 *  \file Core/MaterialManager.cpp
 *  \date 10/16/2018
 *  \author luk2010
    ======================================================= **/

#include "MaterialManager.h"
#include "Platform.h"
#include "Core.h"
#include "NotificationCenter.h"

namespace Clean 
{
    std::atomic < MaterialManager* > MaterialManager::currentManager = nullptr;
    
    MaterialManager& MaterialManager::Current()
    {
        MaterialManager* manager = currentManager.load();
        assert(manager && "Null Current MaterialManager. Perhaps Core class is not created yet.");
        return *manager;
    }
    
    std::vector < std::shared_ptr < Material > > MaterialManager::load(std::string const& filepath)
    {
        std::string const extension = Platform::PathGetExtension(filepath);
        assert(!extension.empty() && "File must have an extension to be loaded.");
        
        auto loader = Core::Get().findFileLoader < Material >(extension);
        if (!loader) {
            Notification notif = BuildNotification(kNotificationLevelWarning, "No FileLoader found to load Material file '%s'.", filepath.data());
            NotificationCenter::GetDefault()->send(notif);
            return {};
        }
        
        auto result = loader->load(filepath);
        decltype(result) result2;
        
        if (result.empty()) {
            Notification notif = BuildNotification(kNotificationLevelError, "FileLoader %s cannot load file '%s'.", 
                                                   loader->getInfos().name.data(), filepath.data());
            NotificationCenter::GetDefault()->send(notif);
            return {};
        }
        
        // Check for each Material if they wern't already loaded. 
        
        for (auto& material : result)
        {
            auto checked = findByName(material->getName());
            if (!checked) result2.push_back(material);
        }
        
        batchAddOnce(result2);
        return result;
    }
    
    std::shared_ptr < Material > MaterialManager::findByName(std::string const& name) const
    {
        std::lock_guard < std::mutex > lck(managedListMutex);
        auto check = std::find_if(managedList.begin(), managedList.end(), [name](auto material) { return material->getName() == name; });
        return (check == managedList.end()) ? nullptr : *check;
    }
}
