/** =======================================================
 *  \file Core/Material.cpp
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#include "Material.h"
#include "ShaderParameter.h"
#include "Allocate.h"

namespace Clean 
{
    Material::Material()
    {
        diffuseColor = AllocateShared < EffectParameter >(kEffectMaterialDiffuseVec4, ShaderValue(), kShaderParamVec4);
        assert(diffuseColor);
        
        specularColor = AllocateShared < EffectParameter >(kEffectMaterialSpecularVec4, ShaderValue(), kShaderParamVec4);
        assert(specularColor);
        
        ambientColor = AllocateShared < EffectParameter >(kEffectMaterialAmbientVec4, ShaderValue(), kShaderParamVec4);
        assert(ambientColor);
        
        emissiveColor = AllocateShared < EffectParameter >(kEffectMaterialEmissiveVec4, ShaderValue(), kShaderParamVec4);
        assert(emissiveColor);
    }
    
    glm::vec4 Material::getDiffuseColor() const 
    {
        SharedParameter param = std::atomic_load(&diffuseColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        return param->value.vec4;
    }
    
    void Material::setDiffuseColor(glm::vec4 const& color)
    {
        SharedParameter param = std::atomic_load(&diffuseColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        param->value.vec4 = color;
    }
    
    glm::vec4 Material::getSpecularColor() const 
    {
        SharedParameter param = std::atomic_load(&specularColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        return param->value.vec4;
    }
    
    void Material::setSpecularColor(glm::vec4 const& color)
    {
        SharedParameter param = std::atomic_load(&specularColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        param->value.vec4 = color;
    }
    
    glm::vec4 Material::getAmbientColor() const 
    {
        SharedParameter param = std::atomic_load(&ambientColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        return param->value.vec4;
    }
    
    void Material::setAmbientColor(glm::vec4 const& color)
    {
        SharedParameter param = std::atomic_load(&ambientColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        param->value.vec4 = color;
    }
    
    glm::vec4 Material::getEmissiveColor() const
    {
        SharedParameter param = std::atomic_load(&emissiveColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        return param->value.vec4;
    }
    
    void Material::setEmissiveColor(glm::vec4 const& color)
    {
        SharedParameter param = std::atomic_load(&emissiveColor);
        std::lock_guard < std::mutex > lck(param->mutex);
        param->value.vec4 = color;
    }
    
    std::vector < std::shared_ptr < EffectParameter > > Material::findAllParameters() const 
    {
        std::vector < SharedParameter > result;
        result.push_back(std::atomic_load(&ambientColor));
        result.push_back(std::atomic_load(&diffuseColor));
        result.push_back(std::atomic_load(&specularColor));
        return result;
    }
    
    std::string Material::getName() const
    {
        return name.load();
    }
}
