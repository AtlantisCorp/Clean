/** \file Core/DriverManager.cpp
**/

#include "DriverManager.h"

namespace Clean 
{
    std::shared_ptr < Driver > DriverManager::findDriverByName(std::string const& name)
    {
        std::scoped_lock < std::mutex > lck(managedListMutex);
        auto found = std::find_if(managedList.begin(), managedList.end(), [name](auto const& driver){ 
            assert(driver); return driver->getName() == name; });
        if (found != managedList.end()) return *found;
        return nullptr;
    }
}
