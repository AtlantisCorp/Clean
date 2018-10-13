/** =======================================================
 *  \file Core/EffectSession.cpp
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#include "EffectSession.h"
#include "RenderPipeline.h"
#include "Allocate.h"

namespace Clean 
{
    std::weak_ptr < EffectParameter > EffectSession::add(std::string const& name, ShaderValue const& value, std::uint8_t const& type) 
    {
        std::shared_ptr < EffectParameter > param = AllocateShared < EffectParameter >(name, value, type);
        assert(param && "Null allocation.");
        
        auto& params = globals.lock();
        params.push_back(param);
        globals.unlock();
        
        return param;
    }
    
    void EffectSession::remove(std::string const& name)
    {
        auto& params = globals.lock();
        auto it = std::find_if(params.begin(), params.end(), [name](auto p) { return p->name == name; });
        params.erase(it);
        globals.unlock();
    }
    
    void EffectSession::clear()
    {
        auto& params = globals.lock();
        params.clear();
        globals.unlock();
    }
    
    void EffectSession::bind(RenderPipeline const& pipeline) const
    {
        auto params = globals.load();
        auto mapper = pipeline.getMapper();
        assert(mapper && "Null ShaderMapper for RenderPipeline.");
        
        std::vector < ShaderParameter > sparameters;
        sparameters.reserve(params.size());
        
        for (auto p : params)
        {
            ShaderParameter parameter = mapper->map(*p, pipeline);
            sparameters.push_back(parameter);
        }
        
        pipeline.bindParameters(sparameters);
    }
}
