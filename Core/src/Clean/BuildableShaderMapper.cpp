/** ********************************************
 *  \file Core/BuildableShaderMapper.cpp
 *  \date 10/21/2018
 *  \author luk2010
**/

#include "BuildableShaderMapper.h"
#include "ShaderAttribute.h"
#include "VertexDescriptor.h"
#include "RenderPipeline.h"

namespace Clean 
{
    ShaderAttributesMap BuildableShaderMapper::map(VertexDescriptor const& descriptor, RenderPipeline const& pipeline) const 
    {
        ShaderAttributesMap result(descriptor.indexInfos);
        result.setElements(descriptor.localSubmesh.elements);
        
        // We must fill all attributes with the elements described in our attributes array and in our
        // pipeline. Notes we should find a way to cache data to avoid using attributes location finding.
        
        auto components = descriptor.findAllComponents();
        
        for (auto component : components)
        {
            Attribute attribute = findAttribute(component.type);
            if (!attribute.type) continue;
            
            std::int8_t index = (attribute.index >= 0) ? attribute.index : 
                                                         pipeline.findAttributeIndex(attribute.name);
            
            ShaderAttribute attrib = ShaderAttribute::Enabled(
                index, attribute.type, attribute.components,
                component.offset, component.stride, component.buffer
            );
                
            result.add(std::move(attrib));
        }
        
        return result;
    }
    
    ShaderParameter BuildableShaderMapper::map(EffectParameter const& param, RenderPipeline const& pipeline) const
    {
        Constant constant = findConstant(param.hash);
        if (!constant.type) return ShaderMapper::map(param, pipeline);
        
        return ShaderParameter(constant.type, constant.name, constant.index, param.value);
    }
    
    BuildableShaderMapper::Attribute BuildableShaderMapper::findAttribute(std::uint8_t component) const 
    {
        auto& attribs = attributes.lock();
        auto it = std::find_if(attribs.begin(), attribs.end(), [component](auto a){ return a.vertexComponent == component; });
        Attribute result = (it == attribs.end()) ? Attribute() : *it;
        attributes.unlock();
        return result;
    }
    
    BuildableShaderMapper::Constant BuildableShaderMapper::findConstant(std::uint64_t hash) const
    {
        auto& consts = constants.lock();
        auto it = std::find_if(consts.begin(), consts.end(), [hash](auto c){ return c.hash == hash; });
        Constant result = (it == consts.end()) ? Constant() : *it;
        constants.unlock();
        return result;
    }
    
    bool BuildableShaderMapper::addAttribute(BuildableShaderMapper::Attribute const& attrib) 
    {
        Attribute foundAttrib = findAttribute(attrib.vertexComponent);
        if (foundAttrib.type > 0) return false;
        
        auto& attribs = attributes.lock();
        attribs.push_back(attrib);
        attributes.unlock();
        return true;
    }
    
    bool BuildableShaderMapper::addConstant(BuildableShaderMapper::Constant const& constant)
    {
        Constant foundConstant = findConstant(constant.hash);
        if (foundConstant.type > 0) return false;
        
        auto& consts = constants.lock();
        consts.push_back(constant);
        constants.unlock();
        return true;
    }
    
    void BuildableShaderMapper::clear() 
    {
        attributes.store({});
        constants.store({});
        predefinedShaders.store({});
    }
    
    void BuildableShaderMapper::setName(std::string const& value)
    {
        name.store(value);
    }
    
    std::string BuildableShaderMapper::getName() const 
    {
        return name.load();
    }
    
    bool BuildableShaderMapper::addPredefinedShader(BuildableShaderMapper::PredefinedShader const& shader)
    {
        auto& shaders = predefinedShaders.lock();
        auto it = std::find_if(shaders.begin(), shaders.end(), [shader](auto const& s){ return shader.filepath == s.filepath && shader.type == s.type; });
        if (it != shaders.end()) return false;
        
        shaders.push_back(shader);
        predefinedShaders.unlock();
        return true;
    }
    
    bool BuildableShaderMapper::hasPredefinedShaders() const
    {
        return !predefinedShaders.load().empty();
    }
    
    std::vector < BuildableShaderMapper::PredefinedShader > BuildableShaderMapper::getPredefinedShaders() const
    {
        return predefinedShaders.load();
    }
}
