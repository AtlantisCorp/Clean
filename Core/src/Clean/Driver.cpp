/** \file Core/Driver.cpp
**/

#include "Driver.h"
#include "Core.h"

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
    
    PixelFormat Driver::getPixelFormat() const 
    {
        std::lock_guard < std::mutex > lck(pixelFormatMutex);
        return pixelFormat;
    }
    
    bool Driver::allWindowClosed() const
    {
        return renderWindows.allWindowClosed();
    }
    
    void Driver::update() 
    {
        renderWindows.forEach([this](std::shared_ptr < RenderWindow >& wnd){
            assert(wnd && "Null window stored.");
            wnd->prepare(*this);
        });
        
        commitAllQueues();
        
        renderWindows.forEach([](std::shared_ptr < RenderWindow >& wnd){
            assert(wnd && "Null window stored.");
            wnd->swapBuffers();
            wnd->update();
        });
    }
    
    void Driver::commitAllQueues() 
    {
        renderQueues.forEachCpy([this](std::shared_ptr < RenderQueue > const& queue){
            assert(queue && "Null RenderQueue stored.");
            commit(queue);
        });
    }
    
    std::shared_ptr < RenderQueue > Driver::makeRenderQueue(std::uint8_t priority, std::uint8_t type)
    {
        auto result = _createRenderQueue(type);
        if (!result) return result;
        
        renderQueues.add(priority, result);
        return result;
    }
    
    void Driver::commit(std::shared_ptr < RenderQueue > const& queue)
    {
        assert(queue && "Null RenderQueue for commitment given.");
        std::size_t commitedCommands = queue->getCommitedCommands();
        
        // From now on, all RenderCommands pushed after this point are ignored by this
        // function as commitedCommands has already been loaded. Other RenderCommands will
        // be rendered into next commit, i.e. next frame. 
        
        while(commitedCommands)
        {
            RenderCommand command = queue->nextCommand();
            commitedCommands--;
            renderCommand(command);
        }
    }
    
    void Driver::renderCommand(RenderCommand const& command)
    {
        assert(command.target && command.pipeline && "Null RenderTarget or RenderPipeline for given RenderCommand.");
        RenderPipeline const& pipeline = *(command.pipeline);
        
        command.bind(*this);
        
        // Notes: Now RenderTarget and RenderPipeline are bound. We must ensure all parameters for the render command
        // are set for the current pipeline. 
        
        effSession.bind(pipeline);
        pipeline.bindParameters(command.parameters);
        
        // Now just render each subcommands. 
        
        for (RenderSubCommand const& subCommand : command.subCommands)
        {
            pipeline.bindParameters(subCommand.parameters);
            pipeline.bindShaderAttributes(subCommand.attributes);
            pipeline.setDrawingMethod(subCommand.drawingMethod);
            drawShaderAttributes(subCommand.attributes);
        }
    }
    
    bool Driver::shouldReleaseResource(DriverResource const& resource) const 
    {
        // NOTES: Default implementation always return true. We do not support persistent data by default.
        return true;
    }
    
    EffectSession& Driver::getEffectSession()
    {
        return effSession;
    }
    
    /** Pseudo-code sample for Driver::commit(): 
    
    assert(queue && "Null RenderQueue for commitment given.");
    std::size_t commitedCommands = queue->getCommitedCommands();
    
    while(commitedCommands)
    {
        RenderCommand command = queue->nextCommand();
        commitedCommands--;
        renderCommand(command);
    }
    
    Pseudo-code sample for Driver::renderCommand():
    
    command.target.bind()
    command.bind()
    command.pipeline.bind()
    
    foreach subcommand in command.subcommands
    
        renderSubCommand(command.pipeline, subcommand)
    
    end foreach
    
    Pseudo-code sample for Driver::renderSubCommand():
    
    pipeline.bindShaderAttributesMap(subcommand.attributes)
    pipeline.setDrawingMethod(subcommand.drawingMethod)
    
    if (subcommand.attributes.indexInfos)
        drawIndexedVertexes(subcommand.drawingMode, subcommand.attributes.indexInfos);
    else 
        drawVertexes(subcommand.drawingMode, subcommand.attributes.elements);
    
    **/
}
