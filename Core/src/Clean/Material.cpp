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
        
        diffuseTexture = AllocateShared < TexturedParameter >(kEffectMaterialDiffuseTexture, ShaderValue(), kShaderParamI32);
        ambientTexture = AllocateShared < TexturedParameter >(kEffectMaterialAmbientTexture, ShaderValue(), kShaderParamI32);
        specularTexture = AllocateShared < TexturedParameter >(kEffectMaterialSpecularTexture, ShaderValue(), kShaderParamI32);
        assert(diffuseTexture && ambientTexture && specularTexture);
    }
    
    Material::Material(std::string const& n) : Material()
    {
        name.store(n);
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
        result.push_back(std::atomic_load(&emissiveColor));
        return result;
    }
    
    std::vector < std::shared_ptr < TexturedParameter > > Material::findAllTexturedParameters() const
    {
        std::vector < SharedTexParam > result;
        result.push_back(std::atomic_load(&diffuseTexture));
        result.push_back(std::atomic_load(&ambientTexture));
        result.push_back(std::atomic_load(&specularTexture));
        return result;
    }
    
    std::string Material::getName() const
    {
        return name.load();
    }
    
    std::shared_ptr < Texture > Material::getDiffuseTexture() const 
    {
        SharedTexParam tex = std::atomic_load(&diffuseTexture);
        return std::atomic_load(&(tex->texture));
    }
    
    void Material::setDiffuseTexture(std::shared_ptr < Texture > const& texture)
    {
        SharedTexParam tex = std::atomic_load(&diffuseTexture);
        std::atomic_store(&(tex->texture), texture);
    }
    
    std::shared_ptr < Texture > Material::getAmbientTexture() const 
    {
        SharedTexParam tex = std::atomic_load(&ambientTexture);
        return std::atomic_load(&(tex->texture));
    }
    
    void Material::setAmbientTexture(std::shared_ptr < Texture > const& texture)
    {
        SharedTexParam tex = std::atomic_load(&ambientTexture);
        std::atomic_store(&(tex->texture), texture);
    }
    
    std::shared_ptr < Texture > Material::getSpecularTexture() const 
    {
        SharedTexParam tex = std::atomic_load(&specularTexture);
        return std::atomic_load(&(tex->texture));
    }
    
    void Material::setSpecularTexture(std::shared_ptr < Texture > const& texture)
    {
        SharedTexParam tex = std::atomic_load(&specularTexture);
        std::atomic_store(&(tex->texture), texture);
    }
}
