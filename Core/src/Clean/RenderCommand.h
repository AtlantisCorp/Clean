/** \file Core/RenderCommand.h
**/

#ifndef CLEAN_RENDERCOMMAND_H
#define CLEAN_RENDERCOMMAND_H

#include "RenderTarget.h"
#include "RenderPipeline.h"
#include "RenderSubCommand.h"
#include "ShaderAttribute.h"
#include "ShaderParameter.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace Clean 
{
    class Driver;
    
    /** @brief Basic command executed by a driver. 
     *
     * A RenderCommand holds a pipeline state (RenderPipeline), configured to render the RenderSubCommands described
     * in the RenderCommand itself. The RenderCommand is created by an object to be rendered by a driver, which only
     * draws RenderCommands. 
     *
     * Several RenderSubCommands can be submitted to a RenderCommand to provide several render with the same pipeline
     * states. This lets the driver optimize multiple drawing commands and save time from changing the pipeline states
     * from one command to another. 
     * 
     * \note Multithread safety
     * A RenderCommand is *not* thread-safe. It is created by one object and pushed to the desired RenderQueue. The 
     * RenderQueue is then the only owner and user of the command. Multiple objects cannot share a RenderCommand from 
     * different threads. 
     *
    **/
    struct RenderCommand 
    {
        RenderCommand() = default;
        RenderCommand(RenderCommand&&) = default;
        RenderCommand(RenderCommand const&) = default;
        
        //! @brief RenderTarget where the driver must draw the command. 
        std::shared_ptr < RenderTarget > target = nullptr;
        
        //! @brief The RenderPipeline used to store some pipeline states for this command.
        std::shared_ptr < RenderPipeline > pipeline = nullptr;
        
        //! @brief List of RenderSubCommands that have to be executed by the driver. 
        std::vector < RenderSubCommand > subCommands;
        
        //! @brief Lists ShaderParameters set for this command. 
        std::vector < ShaderParameter > parameters;
        
        /*! @brief Creates a new sub command with its type and its ShaderAttributesMap.
         *
         * \param[in] type SubCommand type, can be kRenderSubCommandVertex, kRenderSubCommandIndexed. Default 
         *      is not indexed.
         * \param[in] attribs Map of Attributes. \sa Clean::ShaderAttributesMap
         *
        **/
        void sub(std::uint8_t type, ShaderAttributesMap const& attribs);
        
        /*! @brief Creates multiple sub command with the same drawing type. 
         *
         * This function allow multiple sub command generation. Pre-allocation of space for the RenderCommands
         * allows faster generation. 
         *
         * \param[in] type SubCommand type, can be kRenderSubCommandVertex, kRenderSubCommandIndexed. Default 
         *      is not indexed.
         * \param[in] attribsVec Vector with Map of Attributes. \sa Clean::ShaderAttributesMap
         *
        **/
        void batchSub(std::uint8_t type, std::vector < ShaderAttributesMap > const& attribsVec);
        
        /*! @brief Binds target and pipeline to the given Driver. */
        void bind(Driver& driver) const;
    };
}

#endif // CLEAN_RENDERCOMMAND_H