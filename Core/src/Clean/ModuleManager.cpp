/** \file Core/ModuleManager.cpp
**/

#include "ModuleManager.h"

namespace Clean 
{
    std::shared_ptr < Module > ModuleManager::findByName(std::string const& name)
    {
        std::lock_guard < std::mutex > lck(managedListMutex);
        auto check = std::find_if(managedList.begin(), managedList.end(), [name](std::shared_ptr < Module > const& managed) {
            return managed->name() == name;
        });
        
        return (check == managedList.end()) ? *check : nullptr;
    }
    
    std::shared_ptr < Module > ModuleManager::findByUUID(sole::uuid const& uuid)
    {
        std::lock_guard < std::mutex > lck(managedListMutex);
        auto check = std::find_if(managedList.begin(), managedList.end(), [uuid](std::shared_ptr < Module > const& managed) {
            return managed->uuid() == uuid;
        });
        
        return (check == managedList.end()) ? *check : nullptr;
    }
}
