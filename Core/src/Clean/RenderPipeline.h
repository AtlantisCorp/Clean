/** \file Core/RenderPipeline.h
**/

#ifndef CLEAN_RENDERPIPELINE_H
#define CLEAN_RENDERPIPELINE_H

#include "Handled.h"
#include "Shader.h"
#include "ShaderParameter.h"
#include "DriverResource.h"
#include "EffectParameter.h"
#include "Texture.h"

#include <cstdint>
#include <map>
#include <mutex>
#include <memory>

namespace Clean 
{
    class Driver;
    
    //! @defgroup RenderPipelineDrawingMethodGroup RenderPipeline's drawing methods
    /** @brief Defines some drawing methods. We calls drawing methods the fact to draw vertices as points, 
     *  lines or filled geometry (and that's all). Default is kDrawingMethodFilled.
     *  @{
    **/
    
    static constexpr const std::uint8_t kDrawingMethodPoints = 1;
    static constexpr const std::uint8_t kDrawingMethodLines = 2;
    static constexpr const std::uint8_t kDrawingMethodFilled = 3;
    
    //! @}
    
    /** @brief Generic representation of a pipeline state. 
     *
     * It is used by a driver to draw multiple RenderCommands with the same pipeline states. From the view 
     * of the implementor, it corresponds to the current states used to render drawing commands. When 
     * implementing a RenderPipeline, care should be given to how the shaders are handled. 
     *
     * \note Attributes bound to the shader are not stored in the pipeline states, but in the RenderCommand. 
     * This way multiple commands with different buffers can use the same pipeline states. 
     *
     * A RenderPipeline represents how to render multiple render commands. In term of shaders, it also stores
     * the shader for each stage. Thus, attributes and uniforms are handled at this point. The ShaderMapper is 
     * the 'translator' that helps RenderPipeline converts VertexDescriptor to ShaderAttributes. 
     * 
     * ShaderAttributesMap caches and Mesh management
     * Mesh stores in its cache all ShaderAttributesMap generated for each RenderPipeline. When a Mesh populates
     * a RenderCommand, it uses the ShaderAttributesMap cached for this RenderPipeline. 
     *
    **/
    class RenderPipeline : public DriverResource, public Handled < RenderPipeline >
    {
        //! @brief Shaders associated to each shader stage. 
        std::map < std::uint8_t, std::shared_ptr < Shader > > shaders;
        
        //! @brief Protects shaders. 
        mutable std::mutex shadersMutex;
        
        //! @brief ShaderMapper associated to this shader. 
        std::shared_ptr < ShaderMapper > mapper;
        
    public:
        
        /*! @brief Default constructor. */
        RenderPipeline(Driver* driver);
        
        /*! @brief Default destructor. */
        virtual ~RenderPipeline() = default;
        
        /*! @brief Registers a shader into the given stage. 
         *
         * Derived implementation should consider adding the shader stage into its pipeline
         * implementation object, like OpenGL program object. 
         *
        **/
        virtual void shader(std::uint8_t stage, std::shared_ptr < Shader > const& shad);
        
        /*! @brief Registers multiple shaders for their given stage. */
        virtual void batchShaders(std::vector < std::shared_ptr < Shader > > const& shaders);
        
        /*! @brief Binds this pipeline onto the given Driver. */
        virtual void bind(Driver const& driver) const = 0;
        
        /*! @brief Binds multiple EffectParameters onto this pipeline. */
        virtual void bindEffectParameters(std::vector < std::shared_ptr < EffectParameter > > const& parameters) const;
        
        /*! @brief Binds one EffectParameter onto this pipeline. */
        virtual void bindEffectParameter(std::shared_ptr < EffectParameter > const& parameter) const;
        
        /*! @brief Binds multiple parameters onto this pipeline. */
        virtual void bindParameters(std::vector < ShaderParameter > const& parameters) const;
        
        /*! @brief Bind one parameter onto this pipeline. */
        virtual void bindParameter(ShaderParameter const& parameter) const = 0;
        
        /*! @brief Binds multiple ShaderAttribute onto this pipeline. */
        virtual void bindShaderAttributes(ShaderAttributesMap const& attributes) const = 0;
        
        /*! @brief Sets the current drawing method. */
        virtual void setDrawingMethod(std::uint8_t drawingMethod) const = 0;
        
        /*! @brief Changes the shader mapper.
         *
         * If the given mapper has no predefined shaders, then this mapper property is only
         * stored to this object's mapper pointer. However, if this mapper holds predefined
         * shaders:
         *   - If this RenderPipeline has currently no shaders, it will load mapper's predefined
         *     shaders after storing it.
         *   - If this RenderPipeline currently has some shaders, it will check if those are the same
         *     as the mapper's one. If one shader is missing in the RenderPipeline, it will load it
         *     automatically.
         *
         * \note When loading a mapper with predefined shaders, the RenderPipeline is not linked
         * automatically. After this loading, a user might insert other Shaders compatible with mapper's
         * one, like a Tesselation program, or a Geometry program. This can lead to multiple RenderPipeline
         * sharing the same ShaderMapper. 
        **/
        void setMapper(std::shared_ptr < ShaderMapper > const& mapper);
        
        /*! @brief Returns mapper. */
        std::shared_ptr < ShaderMapper > getMapper() const;
        
        /*! @brief Maps the given VertexDescriptor by using this RenderPipeline's ShaderMapper.
         *
         * If no ShaderMapper is present for this pipeline, it returns an empty ShaderAttributesMap. This can
         * be checked with ShaderAttributesMap::isValid() which returns false.
         *
         **/
        ShaderAttributesMap map(VertexDescriptor const& descriptor) const;
        
        /*! @brief Maps multiple VertexDescriptor to multiple ShaderAttributesMap. */
        std::vector < ShaderAttributesMap > map(std::vector < VertexDescriptor > const& descs) const;
        
        /*! @brief Returns true if the given attribute is present in this pipeline. */
        virtual bool hasAttribute(std::string const& attrib) const = 0;
        
        /*! @brief Returns an index representing the location of a given attribute in the pipeline. */
        virtual std::uint8_t findAttributeIndex(std::string const& attrib) const = 0;
        
        /*! @brief Builds a ShaderMapper from given file.
         *
         * Basically it tries to get a FileLoader < ShaderMapper > for the given file. Next, it uses
         * FileLoader::load to load the file and get a ShaderMapper. Generally the loader will use
         * BuildableShaderMapper to create the mapper but this is not disallowed to get another derived
         * class.
         *
         * \return True on success, false otherwise.
         *
         **/
        bool buildMapper(std::string const& filepath);
        
        /*! @brief Binds multiple TexturedParameters onto this Pipeline. */
        virtual void bindTexturedParameters(std::vector < std::shared_ptr < TexturedParameter > > const& params) const;
       
        /*! @brief Binds a TexturedParameter onto this Pipeline. */
        virtual void bindTexturedParameter(std::shared_ptr < TexturedParameter > const& param) const;
       
        /*! @brief Binds the given Texture to the given ShaderParameter. */
        virtual void bindTexture(ShaderParameter const& parameter, Texture const& texture) const = 0;
        
        /*! @brief Returns true if this RenderPipeline can be modified, false otherwise. */
        virtual bool isModifiable() const = 0;
    };
}

#endif // CLEAN_RENDERPIPELINE_H
