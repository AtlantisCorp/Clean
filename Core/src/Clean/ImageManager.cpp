/** =======================================================
 *  \file Core/ImageManager.cpp
 *  \date 10/27/2018
 *  \author luk2010
    ======================================================= **/

#include "ImageManager.h"
#include "Core.h"
#include "Platform.h"
#include "NotificationCenter.h"

namespace Clean 
{
    std::shared_ptr < Image > ImageManager::load(std::string const& filepath)
    {
        {
            auto result = findFile(filepath);
            if (result) return result;
        }
        
        std::string extension = Platform::PathGetExtension(filepath);
        auto loader = Core::Get().findFileLoader < Image >(extension);
        
        if (!loader)
        {
            NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "No loader found for extension %s.", extension.data()));
            return nullptr;
        }
        
        auto result = loader->load(filepath);
        if (result) add(result);
        
        return result;
    }
    
    std::shared_ptr < Image > ImageManager::findFile(std::string const& filepath)
    {
        std::scoped_lock < std::mutex > lck(managedListMutex);
        auto it = std::find_if(managedList.begin(), managedList.end(), [filepath](auto p) { return p->getFile() == filepath; });
        if (it == managedList.end()) return nullptr;
        return *it;
    }
}