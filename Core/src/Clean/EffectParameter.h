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
    
    static constexpr const char* kEffectProjectionMat4 = "CleanProjectionMat4";
    static constexpr const std::uint64_t kEffectProjectionMat4Hash = Hash64Const(kEffectProjectionMat4);
    
    static constexpr const char* kEffectViewMat4 = "CleanViewMat4";
    static constexpr const std::uint64_t kEffectViewMat4Hash = Hash64Const(kEffectViewMat4);
    
    static constexpr const char* kEffectModelMat4 = "CleanModelMat4";
    static constexpr const std::uint64_t kEffectModelMat4Hash = Hash64Const(kEffectModelMat4);
    
    static constexpr const char* kEffectMaterialAmbientVec4 = "CleanMaterialAmbientVec4";
    static constexpr const std::uint64_t kEffectMaterialAmbientVec4Hash = Hash64Const(kEffectMaterialAmbientVec4);
    
    static constexpr const char* kEffectMaterialDiffuseVec4 = "CleanMaterialDiffuseVec4";
    static constexpr const std::uint64_t kEffectMaterialDiffuseVec4Hash = Hash64Const(kEffectMaterialDiffuseVec4);
    
    static constexpr const char* kEffectMaterialSpecularVec4 = "CleanMaterialSpecularVec4";
    static constexpr const std::uint64_t kEffectMaterialSpecularVec4Hash = Hash64Const(kEffectMaterialSpecularVec4);
    
    static constexpr const char* kEffectMaterialEmissiveVec4 = "CleanMaterialEmissiveVec4";
    static constexpr const std::uint64_t kEffectMaterialEmissiveVec4Hash = Hash64Const(kEffectMaterialEmissiveVec4);
}

#endif // CLEAN_EFFECTPARAMETER_H
