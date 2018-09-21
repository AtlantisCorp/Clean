/** \file Core/VertexDescriptor.cpp
**/

#include "VertexDescriptor.h"

namespace Clean 
{
    VertexComponentInfos VertexDescriptor::findInfosFor(VertexComponent const& component)
    {
        VertexComponentPartialInfos partial = components[component];

        VertexComponentInfos complete;
        complete.stride = partial.stride;
        complete.buffer = localSubmesh.buffer;
        complete.offset = (localSubmesh.offset*partial.stride)+partial.offset;
        complete.elements = localSubmesh.elements;

        return complete;
    }

    bool VertexDescriptor::has(VertexComponent const& component)
    {
        return std::find(components.begin(), components.end(), component) != components.end();
    }
    
    void VertexDescriptor::addComponent(std::uint8_t component, std::ptrdiff_t offset, std::ptrdiff_t stride)
    {
        VertexComponentPartialInfos infos = { offset, stride };
        components.insert(std::make_pair(component, infos));
    }
}