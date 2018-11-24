/** \file Core/ShaderMapper.h
**/

#ifndef CLEAN_SHADERMAPPER_H
#define CLEAN_SHADERMAPPER_H

#include "ShaderAttribute.h"
#include "ShaderParameter.h"
#include "FileLoader.h"

namespace Clean
{
    class RenderPipeline;
    struct VertexDescriptor;
    struct EffectParameter;

    /** @brief Makes the link between a VertexDescriptor and a Shader.
     *
     * A VertexDescriptor is defined by any mesh that has one or more vertex buffers. This descriptor holds all
     * the informations to describe a Vertex used by this particular mesh. A Shader has one or more attributes,
     * that expects informations that only the shader's creator know what they are for. For example, a shader with
     * attribute 'attrib0' might correspond to a Vertex's position, and in another shader correspond to a vertex's
     * color.
     *
     * ShaderMapper binds the VertexDescriptor to a ShaderAttributesMap compatible with the shader's attributes. This
     * class may be given by the Shader's writer, but anyone can derive this class to create its own mapper for its
     * shaders. The key is to know what you are using as shaders, and vertex data.
     *
     * \note Default shaders returned by Driver::findDefaultShaderForStage() are returned with an instance of
     * DefaultShaderMapper. This class can be used by anyone who uses the same layout as Clean's default shaders.
     *
    **/
    class ShaderMapper
    {
    public:
        
        /*! @brief A simple structure that store the file and the type of a shader predefined in
         *  a Mapper file.
         **/
        struct PredefinedShader
        {
            std::string filepath;
            std::uint8_t type;
        };
        
    public:

        /*! @brief Default destructor. */
        virtual ~ShaderMapper() = default;

        /*! @brief Maps a given vertex's descriptor to an AttributesMap.
         *
         * Implementation of this function should be done for one RenderPipeline. It basically maps all pipeline's
         * attributes to each component of VertexDescriptor (i.e. kVertexComponentColor, kVertexComponentNormal, ...).
         * A VertexComponent is bound to a ShaderAttribute structure which will be used by the driver to draw the render
         * command.
         *
        **/
        virtual ShaderAttributesMap map(VertexDescriptor const& descriptor, RenderPipeline const& pipeline) const = 0;

        /* Mapper is used like this example:

        {
            ShaderAttributesMap result(descriptor.indexInfos);
            result.setElements(descriptor.localSubmesh.elements);

            if (descriptor.has(kVertexComponentColor) && pipeline.hasAttribute("color"))
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentColor);

                ShaderAttribute attrib = ShaderAttribute::Enabled(
                    pipeline.findAttributeIndex("color"),
                    infos.offset, infos.stride, infos.buffer
                );

                result.add(std::move(attrib));
            }

            if (descriptor.has(kVertexComponentPosition) && pipeline.hasAttribute("position"))
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentPosition);

                ShaderAttribute attrib = ShaderAttribute::Enabled(
                    pipeline.findAttributeIndex("position"),
                    infos.offset, infos.stride, infos.buffer
                );

                result.add(std::move(attrib));
            }

            if (descriptor.has(kVertexComponentTexture) && pipeline.hasAttribute("texture"))
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentTexture);

                ShaderAttribute attrib = ShaderAttribute::Enabled(
                    pipeline.findAttributeIndex("texture"),
                    infos.offset, infos.stride, infos.buffer
                );

                result.add(std::move(attrib));
            }
        
            return result;
        }

        */
        
        /*! @brief Maps an Effect name to the correct shader's parameter name or index. */
        virtual ShaderParameter map(EffectParameter const& param, RenderPipeline const& pipeline) const;
        
        /*! @brief Returns true if this mapper has some predefined shaders. */
        virtual bool hasPredefinedShaders() const;
        
        /*! @brief Returns the list of PredefinedShaders. */
        virtual std::vector < PredefinedShader > getPredefinedShaders() const;
    };
    
    template <>
    class FileLoader < ShaderMapper > : public FileLoaderInterface 
    {
    public:
        virtual ~FileLoader() = default;
        virtual std::shared_ptr < ShaderMapper > load(std::string const& file) const = 0;
    };
}

#endif
