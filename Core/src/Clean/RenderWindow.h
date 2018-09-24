/** \file Core/RenderWindow.h
**/

#ifndef CLEAN_RENDERWINDOW_H
#define CLEAN_RENDERWINDOW_H

#include "Window.h"
#include "RenderTarget.h"

namespace Clean 
{
    /** @brief Rendering Window interface. 
     *
     * A RenderWindow is a derived of Window which allows a user to swap its buffers. This
     * means a RenderWindow has multiple framebuffers to display its content. A standard 
     * render window has two buffers (front and back), but nowadays a swap chains may include
     * more buffers, or even a circular buffer. 
     *
     * \note
     * A RenderWindow is always created by a Driver. This means the Driver is able to destroy it
     * at any time and so, a destroy() function is available for this behaviour. However, the RenderWindow
     * creates itself in its constructor. 
     *
    **/
    class RenderWindow : public Window, public RenderTarget
    {   
    public:
        
        /*! @brief Destructs the RenderWindow. */ 
        virtual ~RenderWindow() = default;
        
        /*! @brief Returns the current number of buffers used to display this window. */
        virtual std::size_t getBuffersCount() const = 0;
        
        /*! @brief Swap between back and front buffers, or advance to the next buffer if more than
         * two buffers are available. If one buffer is available, do nothing. */
        virtual void swapBuffers() = 0;
    };
}

#endif // CLEAN_RENDERWINDOW_H
