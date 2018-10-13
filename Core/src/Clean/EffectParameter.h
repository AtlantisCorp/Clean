/** =======================================================
 *  \file Core/EffectSession.h
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

namespace Clean 
{
    struct EffectParameter 
    {
        std::string name;
        std::uint64_t hash;
        std::uint8_t type;
        ShaderValue value;
        
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
}

#endif // CLEAN_EFFECTPARAMETER_H
