/** \file Core/WindowManager.h
**/

#ifndef CLEAN_WINDOWMANAGER_H
#define CLEAN_WINDOWMANAGER_H

#include "Window.h"
#include "Manager.h"

namespace Clean 
{
    /** @brief Handles every Window objects in the engine. 
    **/
    class WindowManager final : public Manager < Window >
    {
    public:
        
        /** @brief Default constructor. */
        WindowManager() = default;
        
        /** @brief Default destructor. */
        ~WindowManager() = default;
        
        /** @brief Returns true if all windows are closed. */
        bool allWindowClosed() const;
        
        /** @brief Calls Window::update() on every windows. */
        void updateAllWindows();
        
        /** @brief Returns the Window designated by hdl, or nullptr. */
        std::shared_ptr < Window > findByHandle(std::uint16_t const& hdl);
    };
}

#endif // CLEAN_WINDOWMANAGER_H