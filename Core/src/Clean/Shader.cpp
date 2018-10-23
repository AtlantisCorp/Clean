/** \file Core/Shader.cpp
**/

#include "Shader.h"
#include "VertexDescriptor.h"

namespace Clean 
{
    Shader::Shader(std::uint8_t stype) : type(stype) 
    {
        
    }
    
    std::uint8_t Shader::getType() const 
    {
        return type.load();
    }
    
    void Shader::setOriginPath(std::string const& origin)
    {
        originPath = origin;
    }
    
    std::string Shader::getOriginPath() const 
    {
        return originPath;
    }
}
