/** \file Core/RenderPipeline.h
**/

#ifndef CLEAN_RENDERPIPELINE_H
#define CLEAN_RENDERPIPELINE_H

#include "Shader.h"
#include "ShaderParameter.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <memory>

namespace Clean 
{
    class Driver;
    
    /** @brief Generic representation of a pipeline state. 
     *
     * It is used by a driver to draw multiple RenderCommands with the same pipeline states. From the view 
     * of the implementor, it corresponds to the current states used to render drawing commands. When 
     * implementing a RenderPipeline, care should be given to how the shaders are handled. 
     *
     * \note Attributes bound to the shader are not stored in the pipeline states, but in the RenderCommand. 
     * This way multiple commands with different buffers can use the same pipeline states. 
     *
    **/
    class RenderPipeline 
    {
        //! @brief Shaders associated to each shader stage. 
        std::map < std::uint8_t, std::shared_ptr < Shader > > shaders;
        
        //! @brief Protects shaders. 
        std::mutex shadersMutex;
        
    public:
        
        /*! @brief Default constructor. */
        RenderPipeline() = default;
        
        /*! @brief Default destructor. */
        virtual ~RenderPipeline() = default;
        
        /*! @brief Registers a shader into the given stage. 
         *
         * Derived implementation should consider adding the shader stage into its pipeline
         * implementation object, like OpenGL program object. 
         *
        **/
        virtual void shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad);
        
        /*! @brief Binds this pipeline onto the given Driver. */
        virtual void bind(Driver& driver) const = 0;
        
        /*! @brief Binds multiple parameters onto this pipeline. */
        virtual void bindParameters(std::vector < ShaderParameter > const& parameters) const = 0;
        
        /*! @brief Binds multiple ShaderAttribute onto this pipeline. */
        virtual void bindShaderAttributes(ShaderAttributesMap const& attributes) const = 0;
        
        /*! @brief Sets the current drawing method. */
        virtual void setDrawingMethod(std::uint8_t drawingMethod) const = 0;
    };
}

#endif // CLEAN_RENDERPIPELINE_H