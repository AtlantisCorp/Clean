/** \file Core/ShaderParameter.cpp
**/

#include "ShaderParameter.h"

namespace Clean 
{
    std::string ShaderParameterToString(std::uint8_t type)
    {
        switch(type)
        {
            case kShaderParamU32: return "u32";
            case kShaderParamI32: return "i32";
            case kShaderParamFloat: return "fl";
            
            case kShaderParamVec2: return "vec2";
            case kShaderParamVec3: return "vec3";
            case kShaderParamVec4: return "vec4";
            
            case kShaderParamUVec2: return "uvec2";
            case kShaderParamUVec3: return "uvec3";
            case kShaderParamUVec4: return "uvec4";
            
            case kShaderParamIVec2: return "ivec2";
            case kShaderParamIVec3: return "ivec3";
            case kShaderParamIVec4: return "ivec4";
            
            case kShaderParamMat2: return "mat2";
            case kShaderParamMat3: return "mat3";
            case kShaderParamMat4: return "mat4";
            
            case kShaderParamMat2x3: return "mat2x3";
            case kShaderParamMat3x2: return "mat3x2";
            case kShaderParamMat2x4: return "mat2x4";
            case kShaderParamMat4x2: return "mat4x2";
            case kShaderParamMat3x4: return "mat3x4";
            case kShaderParamMat4x3: return "mat4x3";
            
            default:
            return std::string();
        }
    }
    
    std::uint8_t ShaderParameterFromString(std::string const& str)
    {
        if (str == "u32") return kShaderParamU32;
        if (str == "i32") return kShaderParamI32;
        if (str == "fl") return kShaderParamFloat;
        
        if (str == "vec2") return kShaderParamVec2;
        if (str == "vec3") return kShaderParamVec3;
        if (str == "vec4") return kShaderParamVec4;
        
        if (str == "uvec2") return kShaderParamUVec2;
        if (str == "uvec3") return kShaderParamUVec3;
        if (str == "uvec4") return kShaderParamUVec4;
        
        if (str == "ivec2") return kShaderParamIVec2;
        if (str == "ivec3") return kShaderParamIVec3;
        if (str == "ivec4") return kShaderParamIVec4;
        
        if (str == "mat2") return kShaderParamMat2;
        if (str == "mat3") return kShaderParamMat3;
        if (str == "mat4") return kShaderParamMat4;
        
        if (str == "mat2x3") return kShaderParamMat2x3;
        if (str == "mat3x2") return kShaderParamMat3x2;
        if (str == "mat2x4") return kShaderParamMat2x4;
        if (str == "mat4x2") return kShaderParamMat4x2;
        if (str == "mat3x4") return kShaderParamMat3x4;
        if (str == "mat4x3") return kShaderParamMat4x3;
        
        return kShaderParamNull;
    }
    
    ShaderParameter::ShaderParameter(ShaderParameter const& rhs)
    {
        type = rhs.type;
        idx = rhs.idx;
        name = rhs.name;
        memcpy(&value, &rhs.value, sizeof(value));
    }
    
    ShaderParameter::ShaderParameter(std::uint8_t t, std::string const& n) : type(t), idx(-1), name(n)
    {
        
    }
    
    ShaderParameter::ShaderParameter(std::uint8_t t, std::string const& n, std::int16_t i, ShaderValue const& v)
        : type(t), idx(i), name(n)
    {
        memcpy(&value, &v, sizeof(ShaderValue));
    }
}
