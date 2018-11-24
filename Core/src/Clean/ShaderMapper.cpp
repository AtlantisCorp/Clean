/** =======================================================
 *  \file Core/ShaderMapper.cpp
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#include "ShaderMapper.h"
#include "EffectParameter.h"
#include "RenderPipeline.h"

namespace Clean 
{
    ShaderParameter ShaderMapper::map(EffectParameter const& param, RenderPipeline const& pipeline) const 
    {
        return ShaderParameter(param.type, param.name, -1, param.value);
    }
    
    bool ShaderMapper::hasPredefinedShaders() const
    {
        return false;
    }
    
    std::vector < ShaderMapper::PredefinedShader > ShaderMapper::getPredefinedShaders() const
    {
        return {};
    }
}
