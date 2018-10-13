/** \file Core/RenderWindowManager.cpp
**/

#include "RenderWindowManager.h"

namespace Clean 
{
    RenderWindowManager::~RenderWindowManager()
    {
        forEach([](std::shared_ptr < RenderWindow > const& wnd){
            assert(wnd && "Null pointer stored.");
            wnd->close();
        });
    }
    
    bool RenderWindowManager::allWindowClosed() const 
    {
        bool result = true;
        
        forEach([&result](std::shared_ptr < RenderWindow > const& window){
            assert(window && "Null pointer stored.");
            if (!window->isClosed()) 
                result = false;
        });
        
        return result;
    }
    
    void RenderWindowManager::updateAllWindows() 
    {
        forEach([](std::shared_ptr < RenderWindow > const& window){
            assert(window && "Null pointer stored.");
            window->update();
        });
    }
}
