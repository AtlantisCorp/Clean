/** \file Core/RenderCommand.cpp
**/

#include "RenderCommand.h"

namespace Clean 
{
    void RenderCommand::sub(std::uint8_t type, ShaderAttributesMap const& attribs)
    {
        RenderSubCommand subCommand(type, attribs);
        subCommands.push_back(subCommand);
    }
    
    void RenderCommand::batchSub(std::uint8_t type, std::vector < ShaderAttributesMap > const& attribsVec)
    {
        std::size_t const capacity = subCommands.capacity() - subCommands.size();
        if (capacity < attribsVec.size()) subCommands.reserve(capacity + attribsVec.size());
        
        for (auto const& attrib : attribsVec) 
        {
            RenderSubCommand subCommand(type, attrib);
            subCommands.push_back(subCommand);
        }
    }
}