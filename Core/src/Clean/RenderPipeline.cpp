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
        std::atomic_store(&mapper, shaderMapper);
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
}
