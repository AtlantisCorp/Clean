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
    
    //! @}
    
    /** @brief Generic shader definition. */
    class Shader : public DriverResource, public Handled < Shader > 
    {
        //! @brief Type related to this shader.
        std::atomic < std::uint8_t > type;
        
    public:
        
        /*! @brief Default constructor. */
        Shader() = default;
        
        /*! @brief Constructs a shader with given type. */
        explicit Shader(std::uint8_t stype);
        
        /*! @brief Default destructor. */
        virtual ~Shader() = default;
        
        /*! @brief Returns shader's type. */
        std::uint8_t getType() const;
    };
}

#endif
