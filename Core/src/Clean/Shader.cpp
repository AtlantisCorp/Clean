/** \file Core/Shader.cpp
**/

#include "Shader.h"

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
}