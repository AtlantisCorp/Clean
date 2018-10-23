/** \file Core/VertexDescriptor.cpp
**/

#include "VertexDescriptor.h"

namespace Clean 
{
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
