/** \file Core/RenderCommand.cpp
**/

#include "RenderCommand.h"

namespace Clean 
{
    void RenderCommand::sub(std::uint8_t type, AttributesMap const& attribs, IndexedBufferInfos const& indexedInfos)
    {
        RenderSubCommand subCommand(type, attribs, indexedInfos);
        subCommands.push_back(subCommand);
    }
}