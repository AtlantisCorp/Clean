/** \file Core/RenderWindowManager.h
**/

#ifndef CLEAN_RENDERWINDOWMANAGER_H
#define CLEAN_RENDERWINDOWMANAGER_H

#include "RenderWindow.h"
#include "Manager.h"

namespace Clean 
{
    /** @brief Handles RenderWindow registered in a Driver. 
     *
     * When this object is destroyed, it calls RenderWindow::close() on all its managed
     * objects to prevent possible leak of window closing when the driver closes. In this
     * matter, some windows in the WindowManager may be closed but still registered, because
     * they have been closed by the RenderWindowManager. 
     *
    **/
    class RenderWindowManager final : public Manager < RenderWindow >
    {
    public:
        
        RenderWindowManager() = default;
        ~RenderWindowManager();
        
        bool allWindowClosed() const;
        
        /*! @brief Calls Window::update() on all registered windows. */
        void updateAllWindows();
    };
}

#endif // CLEAN_RENDERWINDOWMANAGER_H