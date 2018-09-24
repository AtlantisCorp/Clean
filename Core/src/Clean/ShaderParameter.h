/** \file Core/ShaderParameter.h
**/

#ifndef CLEAN_SHADERPARAMETER_H
#define CLEAN_SHADERPARAMETER_H

#include <string>
#include <cstddef>

namespace Clean 
{
    /*! @defgroup ShaderParameterGroup Shader's possible parameter types
     *  @brief Defines all supported types for a Shader's parameter (or uniform). 
     *  @{
    **/
    
    static constexpr const std::uint8_t kShaderParamNull = 0;
    
    static constexpr const std::uint8_t kShaderParamU8 = 1;
    static constexpr const std::uint8_t kShaderParamU16 = 2;
    static constexpr const std::uint8_t kShaderParamU32 = 3;
    static constexpr const std::uint8_t kShaderParamU64 = 4;
    
    static constexpr const std::uint8_t kShaderParamI8 = 5;
    static constexpr const std::uint8_t kShaderParamI16 = 6;
    static constexpr const std::uint8_t kShaderParamI32 = 7;
    static constexpr const std::uint8_t kShaderParamI64 = 8;
    
    static constexpr const std::uint8_t kShaderParamFloat = 9;
    static constexpr const std::uint8_t kShaderParamDouble = 10;
    
    static constexpr const std::uint8_t kShaderParamVec2 = 11;
    static constexpr const std::uint8_t kShaderParamVec3 = 12;
    static constexpr const std::uint8_t kShaderParamVec4 = 13;
    
    static constexpr const std::uint8_t kShaderParamUVec2 = 14;
    static constexpr const std::uint8_t kShaderParamUVec3 = 15;
    static constexpr const std::uint8_t kShaderParamUVec4 = 16;
    
    static constexpr const std::uint8_t kShaderParamIVec2 = 17;
    static constexpr const std::uint8_t kShaderParamIVec3 = 18;
    static constexpr const std::uint8_t kShaderParamIVec4 = 19;
    
    static constexpr const std::uint8_t kShaderParamMat2 = 20;
    static constexpr const std::uint8_t kShaderParamMat3 = 21;
    static constexpr const std::uint8_t kShaderParamMat4 = 22;
    
    static constexpr const std::uint8_t kShaderParamMat2x3 = 23;
    static constexpr const std::uint8_t kShaderParamMat3x2 = 24;
    static constexpr const std::uint8_t kShaderParamMat2x4 = 25;
    static constexpr const std::uint8_t kShaderParamMat4x2 = 26;
    static constexpr const std::uint8_t kShaderParamMat3x4 = 27;
    static constexpr const std::uint8_t kShaderParamMat4x3 = 28;
    
    /*! @brief Converts the given ShaderParameter type to string. */
    std::string ShaderParameterToString(std::uint8_t type);
    
    /*! @brief Converts the given string to its ShaderParameter. */
    std::uint8_t ShaderParameterFromString(std::string const& str);
    
    /*! @} */
    
    /*! @brief Generic structure to hold a ShaderParameter. */
    struct ShaderParameter 
    {
        //! @brief Parameter's type. 
        std::uint8_t type = kShaderParamNull;
        
        //! @brief Parameter's index or location in the shader used. If more than one shader is used, don't
        //! use this parameter as multiple shaders may have different location for the same parameter's name. 
        std::int16_t idx = -1;
        
        //! @brief Parameter's name. Prefer this location's hint instead of \ref idx when using multiple shaders,
        //! except if you specify static locations in all your shaders. 
        std::string name;
        
        //! @brief Holds the parameter's value. 
        union value {
            std::uint8_t u8;
            std::uint16_t u16;
            std::uint32_t u32;
            std::uint64_t u64;
            
            std::int8_t i8;
            std::int16_t i16;
            std::int32_t i32;
            std::int64_t i64;
            
            float fl;
            double db;
            
            float vec2[2];
            float vec3[3];
            float vec4[4];
            
            std::uint32_t uvec2[2];
            std::uint32_t uvec3[3];
            std::uint32_t uvec4[4];
            
            std::int32_t ivec2[2];
            std::int32_t ivec3[3];
            std::int32_t ivec4[4];
            
            float mat2[2][2];
            float mat3[3][3];
            float mat4[4][4];
            
            float mat2x3[2][3];
            float mat3x2[3][2];
            float mat2x4[2][4];
            float mat4x2[4][2];
            float mat3x4[3][4];
            float mat4x3[4][3];
        };
    };
}

#endif // CLEAN_SHADERPARAMETER_H
