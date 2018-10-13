/** =======================================================
 *  \file Core/ShaderValue.h
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_SHADERVALUE_H
#define CLEAN_SHADERVALUE_H

#include <cstddef>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/matrix.hpp>

namespace Clean 
{
    /** @brief Holds a value for a shader. */
    union ShaderValue
    {
        std::uint32_t u32;
        std::int32_t i32;
        float fl;
        
        glm::vec2 vec2;
        glm::vec3 vec3;
        glm::vec4 vec4;
        
        glm::uvec2 uvec2;
        glm::uvec3 uvec3;
        glm::uvec4 uvec4;
        
        glm::ivec2 ivec2;
        glm::ivec3 ivec3;
        glm::ivec4 ivec4;
        
        glm::mat2 mat2;
        glm::mat3 mat3;
        glm::mat4 mat4;
        
        glm::mat2x3 mat2x3;
        glm::mat3x2 mat3x2;
        glm::mat2x4 mat2x4;
        glm::mat4x2 mat4x2;
        glm::mat3x4 mat3x4;
        glm::mat4x3 mat4x3;
        
        // ShaderValue() { memset(this, 0, sizeof(ShaderValue)); }
        // ShaderValue(ShaderValue const& rhs) { memcpy(this, &rhs, sizeof(ShaderValue)); }
    };
}

#endif // CLEAN_SHADERVALUE_H
