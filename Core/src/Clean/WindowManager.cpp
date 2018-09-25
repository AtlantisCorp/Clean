/** \file Core/WindowManager.cpp
**/

#include "WindowManager.h"

namespace Clean 
{
    bool WindowManager::allWindowClosed() const 
    {
        bool result;
        
        forEach([&result](std::shared_ptr < Window > const& window){
            assert(window && "Null pointer stored.");
            if (!window->isClosed()) 
                result = false;
        });
        
        return result;
    }
    
    void WindowManager::updateAllWindows()
    {
        forEach([](std::shared_ptr < Window > const& window){
            assert(window && "Null pointer stored.");
            window->update();
        });
    }
    
    std::shared_ptr < Window > WindowManager::findByHandle(std::uint16_t const& hdl) 
    {
        std::scoped_lock < std::mutex > lck(managedListMutex);
        auto it = std::find_if(managedList.begin(), managedList.end(), [&hdl](std::shared_ptr < Window > const& wnd){
            assert(wnd && "Null pointer stored.");
            return wnd->getHandle() == hdl;
        });
        
        if (it == managedList.end())
            return nullptr;
        else 
            return *it;
    }
}
