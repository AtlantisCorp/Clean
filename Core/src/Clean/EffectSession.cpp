/** =======================================================
 *  \file Core/EffectSession.cpp
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#include "EffectSession.h"
#include "RenderPipeline.h"
#include "Material.h"
#include "Allocate.h"

namespace Clean 
{
    EffectSession::EffectSession(EffectSession const& rhs)
    {
        globals.store(rhs.globals.load());
    }
    
    std::weak_ptr < EffectParameter > EffectSession::add(std::string const& name, ShaderValue const& value, std::uint8_t const& type) 
    {
        std::shared_ptr < EffectParameter > param = AllocateShared < EffectParameter >(name, value, type);
        assert(param && "Null allocation.");
        
        auto& params = globals.lock();
        params.push_back(param);
        globals.unlock();
        
        return param;
    }
    
    std::weak_ptr < EffectParameter > EffectSession::add(std::shared_ptr < EffectParameter > const& parameter)
    {
        if (!parameter)
            return {};
        
        auto& params = globals.lock();
        auto it = std::find_if(params.begin(), params.end(), [parameter](auto p) { return p == parameter; });
        
        if (params.end() == it) 
            params.push_back(parameter);
        
        globals.unlock();
        return parameter;
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
        pipeline.bindEffectParameters(globals.load());
        pipeline.bindTexturedParameters(texturedParams.load());
    }
    
    void EffectSession::addMaterial(Material const& material)
    {
        auto parameters = material.findAllParameters();
        batchAddOneHash(parameters);
        
        auto texturedParameters = material.findAllTexturedParameters();
        batchAddOneHash(texturedParameters);
    }
    
    void EffectSession::batchAddOneHash(std::vector < std::shared_ptr < EffectParameter > > const& p)
    {
        auto& parameters = globals.lock();
        
        for (auto& param : p)
        {
            auto it = std::find_if(parameters.begin(), parameters.end(), [param](auto& pointer) { return pointer->hash == param->hash; });
            
            if (it != parameters.end()) 
                (*it) = param;
            else 
                parameters.push_back(param);
        }
        
        globals.unlock();
    }
    
    void EffectSession::batchAddOneHash(std::vector < std::shared_ptr < TexturedParameter > > const& p)
    {
        auto& parameters = texturedParams.lock();
        
        for (auto& param : p)
        {
            auto it = std::find_if(parameters.begin(), parameters.end(), [param](auto& ptr) { 
                return ptr->param.hash == param->param.hash; });
            
            if (it != parameters.end()) 
                (*it) = param;
            else 
                parameters.push_back(param);
        }
        
        texturedParams.unlock();
    }
}
