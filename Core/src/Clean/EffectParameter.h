/** =======================================================
 *  \file Core/EffectParameter.h
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_EFFECTPARAMETER_H
#define CLEAN_EFFECTPARAMETER_H

#include "ShaderValue.h"
#include "Hash.h"

#include <string>
#include <cstddef>
#include <utility>
#include <mutex>

namespace Clean 
{
    struct EffectParameter 
    {
        std::string name;
        std::uint64_t hash;
        std::uint8_t type;
        ShaderValue value;
        mutable std::mutex mutex;
        
        EffectParameter(std::string const& n, ShaderValue const& v, std::uint8_t const& t)
        : name(n), hash(Hash64(n.data())), type(t), value(v) {}
        
        EffectParameter(EffectParameter const& rhs)
            : name(rhs.name), hash(rhs.hash), type(rhs.type), value(rhs.value) {}
        
        EffectParameter() = default;
    };
    
    static constexpr const char* kEffectNullParameter = nullptr;
    static constexpr const std::uint64_t kEffectNullParameterHash = 0;
    
    static constexpr const char* kEffectProjectionMat4 = "kEffectProjectionMat4";
    static constexpr const std::uint64_t kEffectProjectionMat4Hash = Hash64Const(kEffectProjectionMat4);
    
    static constexpr const char* kEffectViewMat4 = "kEffectViewMat4";
    static constexpr const std::uint64_t kEffectViewMat4Hash = Hash64Const(kEffectViewMat4);
    
    static constexpr const char* kEffectModelMat4 = "kEffectModelMat4";
    static constexpr const std::uint64_t kEffectModelMat4Hash = Hash64Const(kEffectModelMat4);
    
    static constexpr const char* kEffectMaterialAmbientVec4 = "kEffectMaterialAmbientVec4";
    static constexpr const std::uint64_t kEffectMaterialAmbientVec4Hash = Hash64Const(kEffectMaterialAmbientVec4);
    
    static constexpr const char* kEffectMaterialDiffuseVec4 = "kEffectMaterialDiffuseVec4";
    static constexpr const std::uint64_t kEffectMaterialDiffuseVec4Hash = Hash64Const(kEffectMaterialDiffuseVec4);
    
    static constexpr const char* kEffectMaterialSpecularVec4 = "kEffectMaterialSpecularVec4";
    static constexpr const std::uint64_t kEffectMaterialSpecularVec4Hash = Hash64Const(kEffectMaterialSpecularVec4);
    
    static constexpr const char* kEffectMaterialEmissiveVec4 = "kEffectMaterialEmissiveVec4";
    static constexpr const std::uint64_t kEffectMaterialEmissiveVec4Hash = Hash64Const(kEffectMaterialEmissiveVec4);
    
    static constexpr const char* kEffectMaterialDiffuseTexture = "kEffectMaterialDiffuseTexture";
    static constexpr const std::uint64_t kEffectMaterialDiffuseTextureHash = Hash64Const(kEffectMaterialDiffuseTexture);
    
    static constexpr const char* kEffectMaterialAmbientTexture = "kEffectMaterialAmbientTexture";
    static constexpr const std::uint64_t kEffectMaterialAmbientTextureHash = Hash64Const(kEffectMaterialAmbientTexture);
    
    static constexpr const char* kEffectMaterialSpecularTexture = "kEffectMaterialSpecularTexture";
    static constexpr const std::uint64_t kEffectMaterialSpecularTextureHash = Hash64Const(kEffectMaterialSpecularTexture);
    
    /*! @brief Returns the ShaderParam type constant from given Hash. 
     *
     * Hash must be a valid value recognized as one of the preset Effect Parameters Constants. In
     * the case hash is not recognized, kShaderParamNull is always returned.
     *
    **/
    std::uint8_t EffectParameterGetTypeFromHash(std::uint64_t const& rhs);
    
    class Texture; 
    /*! @brief Groups a Texture and an EffectParameter. 
     *
     * It is used by EffectSession (and stored by Material) to inform RenderPipeline about Textures
     * to be bound into shaders. Textures are bound tighly with their associated EffectParameter, which
     * is why this structure exists.
     *
    **/
    struct TexturedParameter 
    {
        //! @brief EffectParameter associated to the Texture. 
        EffectParameter param;
        
        //! @brief Texture associated to the parameter. 
        std::shared_ptr < Texture > texture = nullptr;
        
        /*! @brief Constructs a basic TexturedParameter. */
        TexturedParameter(std::string const& n, ShaderValue const& v, std::uint8_t const& t)
            : param(n, v, t) {}
        
        /*! @brief Copies the TexturedParameter. */
        TexturedParameter(TexturedParameter const& rhs) = default;
        
        /*! @brief Default constructor. */
        TexturedParameter() = default;
    };
}

#endif // CLEAN_EFFECTPARAMETER_H
