/** \file Core/RenderPipeline.cpp
**/

#include "RenderPipeline.h"

namespace Clean 
{
    void RenderPipeline::shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad) 
    {
        if (!shad) return;
        std::scoped_lock < std::mutex > lck(shadersMutex);
        shaders[stage] = shad;
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
}