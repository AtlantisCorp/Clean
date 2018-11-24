/** \file Core/RenderPipeline.cpp
**/

#include "RenderPipeline.h"
#include "VertexDescriptor.h"
#include "NotificationCenter.h"

#include "Platform.h"
#include "Core.h"

namespace Clean 
{
    RenderPipeline::RenderPipeline(Driver* driver) 
        : DriverResource(driver)
    {
        
    }
    
    void RenderPipeline::shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad) 
    {
        if (!shad) return;
        std::scoped_lock < std::mutex > lck(shadersMutex);
        shaders[stage] = shad;
    }
    
    void RenderPipeline::batchShaders(std::vector < std::shared_ptr < Shader > > const& shads)
    {
        std::scoped_lock < std::mutex > lck(shadersMutex);
        
        for (std::shared_ptr < Shader > shader : shads)
        {
            assert(shader && "Null Shader given.");
            std::uint8_t stage = shader->getType();
            assert(stage && "Illegal Shader's stage given.");
            shaders[stage] = shader;
            this->shader(stage, shader);
        }
    }
    
    void RenderPipeline::bindEffectParameters(std::vector < std::shared_ptr < EffectParameter > > const& parameters) const 
    {
        if (!parameters.size()) return;
        
        std::shared_ptr < ShaderMapper > loadedMapper = std::atomic_load(&mapper);
        
        if (!loadedMapper) {
            Notification notif = BuildNotification(kNotificationLevelError, "Null ShaderMapper or EffectParameters given to pipeline #%i.", getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
        
        for (auto parameter : parameters)
        {
            std::lock_guard < std::mutex > lck(parameter->mutex);
            ShaderParameter sparam = loadedMapper->map(*parameter, *this);
            bindParameter(sparam);
        }
    }
    
    void RenderPipeline::bindEffectParameter(std::shared_ptr < EffectParameter > const& parameter) const 
    {
        std::shared_ptr < ShaderMapper > loadedMapper = std::atomic_load(&mapper);
        
        if (!loadedMapper || !parameter) {
            Notification notif = BuildNotification(kNotificationLevelError, "Null ShaderMapper or EffectParameter given to pipeline #%i.", getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
        
        std::lock_guard < std::mutex > lck(parameter->mutex);
        ShaderParameter sparam = loadedMapper->map(*parameter, *this);
        bindParameter(sparam);
    }
    
    void RenderPipeline::bindParameters(std::vector < ShaderParameter > const& parameters) const 
    {
        for (ShaderParameter const& parameter : parameters) {
            bindParameter(parameter);
        }
    }
    
    void RenderPipeline::setMapper(std::shared_ptr < ShaderMapper > const& shaderMapper)
    {
        std::shared_ptr < ShaderMapper > nullPtr;
        std::atomic_store(&mapper, nullPtr);
        
        // If we already are linked, we can't modify the mapper or any of the shaders
        // present in this pipeline. A ShaderMapper must be present before linking the
        // pipeline.
        if (!isModifiable()) return;
        
        if (shaderMapper)
        {
            if (shaderMapper->hasPredefinedShaders())
            {
                std::scoped_lock < std::mutex > shadersLock(shadersMutex);
                auto predefinedShaders = shaderMapper->getPredefinedShaders();
                // For each predefined shader, we check if we need to load it or if we already have
                // this shader. If we already have a shader that is not the same for a predefined
                // shader's stage, this is considered as an error and we must exit promptly.
                for (auto predefinedShader : predefinedShaders)
                {
                    auto shaderFound = shaders.find(predefinedShader.type);
                    
                    if (shaderFound == shaders.end())
                    {
                        // Simple case: we load the predefined shader for this stage.
                        Driver& driver = getDriver();
                        auto loadedShader = driver.makeShaders({{ predefinedShader.type, predefinedShader.filepath }});
                        
                        if (loadedShader.empty())
                        {
                            Notification notif = BuildNotification(kNotificationLevelError, "Shader %s couldn't be loaded by Driver %s.", predefinedShader.filepath.data(), driver.getName().data());
                            NotificationCenter::GetDefault()->send(notif);
                            return;
                        }
                        
                        shader(predefinedShader.type, loadedShader[0]);
                        assert(loadedShader[0] && "Error loading shader.");
                    }
                    
                    else
                    {
                        // We check if the current shader is the same as the predefined's one. We can do this
                        // only by checking the shader's original file path.
                        auto loadedShader = shaderFound->second;
                        assert(loadedShader && "Error while looking for Shader.");
                        
                        if (loadedShader->getOriginPath() != predefinedShader.filepath)
                        {
                            Notification notif = BuildNotification(kNotificationLevelError, "Shader %s, present in ShaderMapper, is not the same as Shader %s, present in RenderPipeline.", predefinedShader.filepath.data(), loadedShader->getOriginPath().data());
                            NotificationCenter::GetDefault()->send(notif);
                            return;
                        }
                    }
                }
            }
            
            // If we go here, this means we are all alright about predefined shaders and all this shit.
            // That's good, we can store the new mapper. Wow.
            std::atomic_store(&mapper, shaderMapper);
        }
    }
    
    std::shared_ptr < ShaderMapper > RenderPipeline::getMapper() const 
    {
        return std::atomic_load(&mapper);
    }
    
    ShaderAttributesMap RenderPipeline::map(VertexDescriptor const& descriptor) const
    {
        auto lmapper = std::atomic_load(&mapper);
        if (lmapper) return lmapper->map(descriptor, *this);
        return {};
    }
    
    std::vector < ShaderAttributesMap > RenderPipeline::map(std::vector < VertexDescriptor > const& descs) const
    {
        auto lmapper = std::atomic_load(&mapper);
        if (!lmapper) return {};
        
        std::vector < ShaderAttributesMap > result;
        result.reserve(descs.size());
        
        for (auto const& desc : descs)
        {
            result.push_back(lmapper->map(desc, *this));
        }
        
        return result;
    }
    
    bool RenderPipeline::buildMapper(std::string const& filepath)
    {
        std::string const extension = Platform::PathGetExtension(filepath);
        assert(!extension.empty() && "Empty file extension.");
        
        auto loader = Core::Get().findFileLoader < ShaderMapper >(extension);
        if (!loader) {
            NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "No loader found for file %s.", filepath.data()));
            return false;
        }
        
        auto mapper = loader->load(filepath);
        if (mapper) setMapper(mapper);
        
        return (bool)mapper;
    }
    
    void RenderPipeline::bindTexturedParameters(std::vector < std::shared_ptr < TexturedParameter > > const& params) const
    {
        if (!params.size()) return;
        
        auto loadedMapper = std::atomic_load(&mapper);
        
        if (!loadedMapper) {
            Notification notif = BuildNotification(kNotificationLevelError, "Null ShaderMapper given to pipeline #%i.", getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
        
        for (auto const& param : params)
        {
            if (!param->texture) continue;
            
            std::lock_guard < std::mutex > lck(param->param.mutex);
            ShaderParameter sparam = loadedMapper->map(param->param, *this);
            bindTexture(sparam, *param->texture);
        }
    }
    
    void RenderPipeline::bindTexturedParameter(std::shared_ptr < TexturedParameter > const& param) const
    {
        auto loadedMapper = std::atomic_load(&mapper);
        
        if (!loadedMapper) {
            Notification notif = BuildNotification(kNotificationLevelError, "Null ShaderMapper given to pipeline #%i.", getHandle());
            NotificationCenter::GetDefault()->send(notif);
            return;
        }
        
        
        if (!param->texture) return;
            
        std::lock_guard < std::mutex > lck(param->param.mutex);
        ShaderParameter sparam = loadedMapper->map(param->param, *this);
        bindTexture(sparam, *param->texture);
    }
}
