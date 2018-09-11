/** \file Core/Driver.cpp
**/

#include "Driver.h"

namespace Clean 
{
    Driver::Driver() : state(kDriverStateNotInited)
    {
        
    }
    
    std::uint8_t Driver::getState() const 
    {
        return state.load();
    }
    
    std::shared_ptr < RenderWindow > Driver::createRenderWindow(std::size_t width, std::size_t height, std::string const& title, 
        std::uint16_t style, bool fullscreen)
    {
        std::shared_ptr < RenderWindow > result = _createRenderWindow(width, height, title, style, fullscreen);
        if (!result) return result;
        
        renderWindows.addOnce(result);

        auto windowManager = Core::Get().getWindowManager();
        assert(windowManager && "Invalid WindowManager present.");
        windowManager->addOnce(result);
        
        return result;
    }
    
    std::shared_ptr < RenderSurface > Driver::createRenderSurface(std::size_t width, std::size_t height, NativeSurface parent)
    {
        std::shared_ptr < RenderSurface > result = _createRenderSurface(width, height, parent);
        if (!result) return result;
        
        renderWindows.addOnce(result);

        auto windowManager = Core::Get().getWindowManager();
        assert(windowManager && "Invalid WindowManager present.");
        windowManager->addOnce(result);
        
        return result;
    }
    
    PixelFormat Driver::getPixelFormat() const 
    {
        std::lock_guard < std::mutex > lck(pixelFormatMutex);
        return pixelFormat;
    }
    
    void Driver::update() 
    {
        commitAllQueues();
        
        renderWindows.forEach([](std::shared_ptr < RenderWindow >& wnd){
            assert(wnd && "Null window stored.");
            wnd->swapBuffers();
        });
        
        renderWindows.updateAllWindows();
    }
    
    void Driver::commitAllQueues() 
    {
        renderQueues.forEachCpy([this](std::shared_ptr < RenderQueue > const& queue){
            assert(queue && "Null RenderQueue stored.");
            commit(queue);
        });
    }
    
    std::shared_ptr < RenderQueue > Driver::createRenderQueue(std::uint8_t priority, std::uint8_t type)
    {
        auto result = _createRenderQueue(type);
        if (!result) return result;
        
        renderQueues.add(priority, result);
        return result;
    }
}