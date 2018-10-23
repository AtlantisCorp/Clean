/** \file Core/VertexDescriptor.cpp
**/

#include "VertexDescriptor.h"
#include "ShaderAttribute.h"

#include <cstring>
#include <string>

namespace Clean 
{
    std::uint8_t VertexComponentFromString(std::string const& rhs) 
    {
        if (!strcmp(rhs.data(), "kVertexComponentNull")) return kVertexComponentNull;
        else if (!strcmp(rhs.data(), "kVertexComponentPosition")) return kVertexComponentPosition;
        else if (!strcmp(rhs.data(), "kVertexComponentColor")) return kVertexComponentColor;
        else if (!strcmp(rhs.data(), "kVertexComponentTexture")) return kVertexComponentTexture;
        else if (!strcmp(rhs.data(), "kVertexComponentNormal")) return kVertexComponentNormal;
        else if (!strcmp(rhs.data(), "kVertexComponentTangent")) return kVertexComponentTangent;
        else if (!strcmp(rhs.data(), "kVertexComponentBiTangent")) return kVertexComponentBiTangent;
        else if (!strcmp(rhs.data(), "kVertexComponentOther1")) return kVertexComponentOther1;
        else if (!strcmp(rhs.data(), "kVertexComponentOther2")) return kVertexComponentOther2;
        else if (!strcmp(rhs.data(), "kVertexComponentOther3")) return kVertexComponentOther3;
        
        return kVertexComponentNull;
    }
    
    std::uint8_t VertexComponentGetShaderAttribType(std::uint8_t const& rhs)
    {
        switch (rhs)
        {
            case kVertexComponentPosition:
            case kVertexComponentColor:
            case kVertexComponentTexture:
            case kVertexComponentNormal:
            case kVertexComponentTangent:
            case kVertexComponentBiTangent:
            return kShaderAttribFloat;
            
            default:
            return kShaderAttribNull;
        }
    }
    
    std::uint8_t VertexComponentCount(std::uint8_t const& rhs)
    {
        switch (rhs)
        {
            case kVertexComponentPosition:
            case kVertexComponentNormal:
            case kVertexComponentColor:
            return 4;
            
            case kVertexComponentTexture:
            case kVertexComponentTangent:
            case kVertexComponentBiTangent:
            return 3;
            
            default:
            return 0;
        }
    }
    
    VertexComponentInfos VertexDescriptor::findInfosFor(VertexComponent const& component) const
    {
        VertexComponentPartialInfos partial = components.at(component);

        VertexComponentInfos complete;
        complete.type = component;
        complete.stride = partial.stride;
        complete.buffer = localSubmesh.buffer;
        complete.offset = (localSubmesh.offset*partial.stride)+partial.offset;
        complete.elements = localSubmesh.elements;

        return complete;
    }

    bool VertexDescriptor::has(VertexComponent const& component) const
    {
        return components.find(component) != components.end();
    }
    
    void VertexDescriptor::addComponent(std::uint8_t component, std::ptrdiff_t offset, std::ptrdiff_t stride)
    {
        VertexComponentPartialInfos infos = { offset, stride };
        components.insert(std::make_pair(component, infos));
    }
    
    std::vector < VertexComponentInfos > VertexDescriptor::findAllComponents() const
    {
        std::vector < VertexComponentInfos > result;
        
        for (auto const& pair : components)
        {
            VertexComponentInfos infos = findInfosFor(pair.first);
            result.push_back(infos);
        }
        
        return result;
    }
}
