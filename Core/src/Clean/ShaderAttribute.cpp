/** \file Core/ShaderAttribute.cpp
**/

#include "ShaderAttribute.h"
#include "Buffer.h"
#include <algorithm>

namespace Clean 
{
    ShaderAttribute ShaderAttribute::Enabled(std::uint8_t index, std::uint8_t type, std::uint8_t components,
                                             std::ptrdiff_t offset, std::ptrdiff_t stride,
                                             std::shared_ptr < Buffer > const& buffer)
    {
        ShaderAttribute result;
        result.index = index;
        result.type = type;
        result.components = components;
        result.offset = offset;
        result.stride = stride;
        result.buffer = buffer;
        result.enabled = true;
        return result;
    }
    
    ShaderAttributesMap::ShaderAttributesMap() : attribs{ {ShaderAttribute()} }
    {
        
    }
    
    ShaderAttributesMap::ShaderAttributesMap(std::size_t count) : attribs{ {ShaderAttribute()} }, elements(count) 
    {
        
    }
    
    ShaderAttributesMap::ShaderAttributesMap(IndexedInfos const& infos) : attribs{ {ShaderAttribute()} }, indexInfos(infos) 
    {
        
    }
    
    void ShaderAttributesMap::add(ShaderAttribute&& attrib)
    {
        attrib.index = std::clamp < std::uint8_t >(attrib.index, 0, kShaderAttributeMax - 1);
        attribs[attrib.index] = std::move(attrib);
    }
    
    ShaderAttribute ShaderAttributesMap::find(std::uint8_t index) const
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        return attribs[index];
    }
    
    void ShaderAttributesMap::disable(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        attribs[index].enabled = false;
    }
    
    void ShaderAttributesMap::enable(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        attribs[index].enabled = true;
    }
    
    bool ShaderAttributesMap::isEnabled(std::uint8_t index)
    {
        assert(index < kShaderAttributeMax && "Invalid ShaderAttribute index.");
        return attribs[index].enabled;
    }
    
    IndexedInfos ShaderAttributesMap::getIndexedInfos() const 
    {
        return indexInfos;
    }
    
    void ShaderAttributesMap::setElements(std::size_t count) 
    {
        elements = count;
    }
    
    std::size_t ShaderAttributesMap::getElements() const 
    {
        return elements;
    }
    
    bool ShaderAttributesMap::isValid() const
    {
        return attribs.size() > 0;
    }
    
    std::size_t ShaderAttributesMap::countAttributes() const 
    {
        return attribs.size();
    }
}
