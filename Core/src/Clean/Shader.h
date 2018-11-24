/** \file Core/Shader.h
**/

#ifndef CLEAN_SHADER_H
#define CLEAN_SHADER_H

#include "Handled.h"
#include "ShaderMapper.h"
#include "DriverResource.h"

#include <cstdint>
#include <atomic>
#include <memory>
#include <vector>
#include <string>

namespace Clean 
{
    //! @defgroup ShaderTypeGroup Shader's possible types. 
    //! @{
    
    static constexpr const std::uint8_t kShaderTypeNull         = 0;
    static constexpr const std::uint8_t kShaderTypeVertex       = 1;
    static constexpr const std::uint8_t kShaderTypeHull         = 3;
    static constexpr const std::uint8_t kShaderTypeDomain       = 4;
    static constexpr const std::uint8_t kShaderTypeGeometry     = 5;
    static constexpr const std::uint8_t kShaderTypeFragment     = 2;
    
    static constexpr const std::uint8_t kShaderTypeTessControl  = kShaderTypeHull;
    static constexpr const std::uint8_t kShaderTypeTessEval     = kShaderTypeDomain;
    static constexpr const std::uint8_t kShaderTypePixel        = kShaderTypeFragment;
    
    //! @brief Converts a String representation of Shader Type to its constant equivalent.
    //!
    //! | String      | Shader Type            |
    //! |-------------|------------------------|
    //! | Vertex      | kShaderTypeVertex      |
    //! | Hull        | kShaderTypeHull        |
    //! | Domain      | kShaderTypeDomain      |
    //! | Geometry    | kShaderTypeGeometry    |
    //! | Fragment    | kShaderTypeFragment    |
    //! | TessControl | kShaderTypeTessControl |
    //! | TessEval    | kShaderTypeTessEval    |
    //! | Pixel       | kShaderTypePixel       |
    //!
    //! \note All other strings are converted to \ref kShaderTypeNull.
    std::uint8_t ShaderTypeFromString(std::string const& rhs);
    
    //! @}
    
    /** @brief Generic shader definition. */
    class Shader : public DriverResource, public Handled < Shader > 
    {
        //! @brief Type related to this shader.
        std::atomic < std::uint8_t > type;
        
        //! @brief Original file path used to load this Shader. It is generally filled directly
        //! by the Driver class when loading a shader with Driver::makeShaders. 
        std::string originPath;
        
    public:
        
        /*! @brief Default constructor. */
        Shader() = default;
        
        /*! @brief Constructs a shader with given type. */
        explicit Shader(std::uint8_t stype);
        
        /*! @brief Default destructor. */
        virtual ~Shader() = default;
        
        /*! @brief Returns shader's type. */
        std::uint8_t getType() const;
        
        /*! @brief Sets the original file path. */
        void setOriginPath(std::string const& origin);
        
        /*! @brief Returns the original file path. */
        std::string getOriginPath() const;
    };
}

#endif
