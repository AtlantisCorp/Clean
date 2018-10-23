/** **********************************************
 *  \file Core/EffectParameter.cpp
 *  \date 10/23/2018
 *  \author Luk2010
**/

#include "EffectParameter.h"
#include "ShaderParameter.h"

namespace Clean 
{
    std::uint8_t EffectParameterGetTypeFromHash(std::uint64_t const& rhs)
    {
        switch (rhs)
        {
            case kEffectProjectionMat4Hash:
            case kEffectViewMat4Hash:
            case kEffectModelMat4Hash:
            return kShaderParamMat4;
            
            case kEffectMaterialAmbientVec4Hash:
            case kEffectMaterialDiffuseVec4Hash:
            case kEffectMaterialSpecularVec4Hash:
            case kEffectMaterialEmissiveVec4Hash:
            return kShaderParamVec4;
            
            default:
            return kShaderParamNull;
        }
    }
}