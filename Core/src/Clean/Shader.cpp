/** \file Core/Shader.cpp
**/

#include "Shader.h"
#include "VertexDescriptor.h"

namespace Clean 
{
    std::uint8_t ShaderTypeFromString(std::string const& rhs)
    {
        if (rhs == "Vertex") return kShaderTypeVertex;
        else if (rhs == "Hull") return kShaderTypeHull;
        else if (rhs == "Domain") return kShaderTypeDomain;
        else if (rhs == "Geometry") return kShaderTypeGeometry;
        else if (rhs == "Fragment") return kShaderTypeFragment;
        else if (rhs == "TessControl") return kShaderTypeTessControl;
        else if (rhs == "TessEval") return kShaderTypeTessEval;
        else if (rhs == "Pixel") return kShaderTypePixel;
        return kShaderTypeNull;
    }
    
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
