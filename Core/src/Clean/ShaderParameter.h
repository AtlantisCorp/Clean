/** \file Core/ShaderParameter.h
**/

#ifndef CLEAN_SHADERPARAMETER_H
#define CLEAN_SHADERPARAMETER_H

#include "ShaderValue.h"

#include <string>
#include <cstddef>

namespace Clean 
{
    /*! @defgroup ShaderParameterGroup Shader's possible parameter types
     *  @brief Defines all supported types for a Shader's parameter (or uniform). 
     *  @{
    **/
    
    static constexpr const std::uint8_t kShaderParamNull = 0;
    
    static constexpr const std::uint8_t kShaderParamU32 = 1;
    static constexpr const std::uint8_t kShaderParamI32 = 2;
    static constexpr const std::uint8_t kShaderParamFloat = 3;
    
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
        ShaderValue value;
        
        /*! @brief Default constructor. */
        ShaderParameter() = default;
        
        /*! @brief Copy constructor. */
        ShaderParameter(ShaderParameter const& rhs);
        
        /*! @brief Creates a ShaderParameter with type and name. */
        ShaderParameter(std::uint8_t t, std::string const& n);
        
        /*! @brief Creates a ShaderParameter with all params. */
        ShaderParameter(std::uint8_t t, std::string const& n, std::int16_t i, ShaderValue const& v);
    };
}

#endif // CLEAN_SHADERPARAMETER_H
