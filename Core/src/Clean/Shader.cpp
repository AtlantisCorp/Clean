/** \file Core/Shader.cpp
**/

#include "Shader.h"
#include "VertexDescriptor.h"

namespace Clean 
{
    Shader::Shader(std::uint8_t stype) : type(stype) 
    {
        
    }
    
    void Shader::setMapper(std::shared_ptr < ShaderMapper > const& shaderMapper)
    {
        std::atomic_store(&mapper, shaderMapper);
    }
    
    std::shared_ptr < ShaderMapper > Shader::getMapper() const 
    {
        return std::atomic_load(&mapper);
    }
    
    std::uint8_t Shader::getType() const 
    {
        return type.load();
    }
    
    ShaderAttributesMap Shader::map(VertexDescriptor const& descriptor) const
    {
        auto lmapper = std::atomic_load(&mapper);
        if (lmapper) return lmapper->map(descriptor, *this);
        return {};
    }
    
    std::vector < ShaderAttributesMap > Shader::map(std::vector < VertexDescriptor > const& descs) const
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
