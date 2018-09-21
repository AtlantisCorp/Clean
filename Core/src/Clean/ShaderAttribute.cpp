/** \file Core/ShaderAttribute.cpp
**/

#include "ShaderAttribute.h"
#include "Buffer.h"

namespace Clean 
{
    ShaderAttribute ShaderAttribute::Enabled(std::uint8_t index, std::ptrdiff_t offset, std::ptrdiff_t stride, std::size_t elements,
                                             std::shared_ptr < Buffer > const& buffer)
    {
        ShaderAttribute result;
        result.index = index_;
        result.ofset = offset;
        result.stride = stride;
        result.elements = elements;
        result.buffer = buffer;
        result.enabled = true;
        return std::move(result);
    }
    
    ShaderAttributesMap::ShaderAttributesMap() : attribs{ ShaderAttribute() }
    {
        
    }
    
    ShaderAttributesMap::ShaderAttributesMap(IndexedInfos const& infos) : attribs{ ShaderAttribute() }, indexInfos(infos) 
    {
        
    }
    
    void ShaderAttributesMap::add(ShaderAttribute&& attrib)
    {
        attrib.index = std::clamp(attrib.index, 0, kShaderAttributeMax - 1);
        attribs[attrib.index] = std::move(attrib);
    }
    
    ShaderAttribute ShaderAttributesMap::find(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        return attribs[index];
    }
    
    void ShaderAttributesMap::disable(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        attribsMutex[index].enabled = false;
    }
    
    void ShaderAttributesMap::enable(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        attribsMutex[index].enabled = true;
    }
    
    bool ShaderAttributesMap::isEnabled(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        return attribsMutex[index].enabled;
    }
    
    IndexedInfos ShaderAttributesMap::getIndexedInfos() const 
    {
        return indexInfos;
    }
}