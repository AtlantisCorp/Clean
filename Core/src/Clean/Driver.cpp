/** \file Core/Driver.cpp
**/

#include "Driver.h"
#include "Core.h"
#include "Platform.h"
#include "ImageManager.h"

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
        // are set for the current pipeline. Notes also that EffectSession binds its parameters here for the RenderCommand.
        
        effSession.bind(pipeline);
        command.parameters.bind(pipeline);
        
        // Now just render each subcommands. 
        
        for (RenderSubCommand const& subCommand : command.subCommands)
        {
            subCommand.parameters.bind(pipeline);
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
    
    std::vector < std::shared_ptr < Shader > > Driver::makeShaders(std::vector < std::pair < std::uint8_t, std::string > > const& loadMap)
    {
        std::vector < std::shared_ptr < Shader > > result;
        
        for (auto const& pair : loadMap) 
        {
            std::uint8_t shaderStage = pair.first;
            if (!shaderStage) {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Invalid shader stage.", 0));
                continue;
            }
            
            std::string shaderFile = pair.second;
            if (shaderFile.empty()) {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Shader file is empty.", 0));
                continue;
            }
            
            auto foundShader = findShaderPath(shaderFile);
            if (foundShader) { result.push_back(foundShader); continue; }
            
            std::fstream shaderStream = FileSystem::Current().open(shaderFile, std::ios::in);
            if (!shaderStream) {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Shader file %s not found.", shaderFile.data()));
                continue;
            }
            
            std::string shaderSource;
            Platform::StreamGetContent(shaderStream, shaderSource);
            shaderStream.close();
            
            if (shaderSource.empty()) {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Shader file %s has no source.", shaderFile.data()));
                continue;
            }
            
            foundShader = makeShader(shaderSource.data(), shaderStage);
            if (!foundShader) {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Error while loading file %s.", shaderFile.data()));
                continue;
            }
            
            foundShader->setOriginPath(shaderFile);
            result.push_back(foundShader);
        }
        
        return result;
    }
    
    std::shared_ptr < Texture > Driver::makeTexture(std::string const& filepath) 
    {
        auto image = ImageManager::Current().load(filepath);
        
        if (!image) {
            NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "Image file %s not found.", filepath.data()));
            return nullptr;
        }
        
        std::uint8_t bestPixelFormat = 0;
        if (shouldConvertPixelFormat(image->pixelFormat(), bestPixelFormat))
        {   
            auto converter = PixelSetConverterManager::Current().findConverter(image->pixelFormat(), bestPixelFormat);
            
            if (converter)
            {
                auto convertedImage = AllocateShared < Image >();
                convertedImage->setOrigin(image->getOrigin());
                convertedImage->setSize(image->getSize());
            
                auto pixels = converter->convert(image->getPixelSet());
                convertedImage->setPixelSet(pixels);
                
                image = convertedImage;
                
                // NOTES: ImageManager does not store this new image because it is used only to create the texture. It will
                // be destroyed immediatly after the texture creation. 
            }
            
            else 
            {
                NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelWarning, "PixelFormat %i used instead of %i because PixelSetConverter cannot convert it.", image->pixelFormat(), bestPixelFormat));
            }
        }
        
        return makeTexture(image);
    }
    
    bool Driver::shouldConvertPixelFormat(std::uint8_t src, std::uint8_t& best) const
    {
        return false;
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
