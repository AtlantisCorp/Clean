/** \file Core/DynlibManager.cpp
**/

#include "DynlibManager.h"

namespace Clean 
{
    std::shared_ptr < Dynlib > DynlibManager::findFromFile(std::string const& path)
    {
        std::lock_guard < std::mutex > lck(managedListMutex);
        auto check = std::find_if(managedList.begin(), managedList.end(), [path](std::shared_ptr < Dynlib > const& managed) {
            return managed->getFilepath() == path;
        });
        
        return (check == managedList.end()) ? *check : nullptr;
    }
}
