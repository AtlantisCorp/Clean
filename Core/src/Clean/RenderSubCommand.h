/** \file Core/RenderSubCommand.h
**/

#ifndef CLEAN_RENDERSUBCOMMAND_H
#define CLEAN_RENDERSUBCOMMAND_H

#include "ShaderAttribute.h"
#include "ShaderParameter.h"

namespace Clean 
{
    /** @brief Sub command of a RenderCommand. 
     *
     * As a RenderCommand holds multiple sub commands for the same pipeline, a sub command holds
     * all attributes to render a part of the command. A RenderSubCommand is generally generated
     * from a Mesh, but anyone can fill its own RenderSubCommand, given the drawing method and
     * its ShaderAttributesMap (\see ShaderMapper). 
     *
     * \sa RenderCommand, Mesh
     *
    **/
    struct RenderSubCommand 
    {
        //! @brief Drawing method used to render this sub-command.
        std::uint8_t drawingMethod;
        
        //! @brief Drawing mode used to render this sub-command.
        std::uint8_t drawingMode;
        
        //! @brief Attributes for Vertexes and Indexes to render this sub command.
        ShaderAttributesMap attributes;
        
        //! @brief Lists ShaderParameters set for this sub command. 
        std::vector < ShaderParameter > parameters;
        
        RenderSubCommand() = default;
        RenderSubCommand(RenderSubCommand const&) = default;
        
        /*! @brief Constructs a RenderSubCommand with a method and a ShaderAttributesMap. */
        RenderSubCommand(std::uint8_t method, ShaderAttributesMap attribs) : drawingMethod(method), attributes(attribs) {}
    };
}

#endif // CLEAN_RENDERSUBCOMMAND_H
