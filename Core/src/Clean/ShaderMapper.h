/** \file Core/ShaderMapper.h
**/

#ifndef CLEAN_SHADERMAPPER_H
#define CLEAN_SHADERMAPPER_H

#include "ShaderAttribute.h"

namespace Clean 
{
    class Shader;
    class VertexDescriptor;
    
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
        
        /*! @brief Default destructor. */
        virtual ~ShaderMapper() = default;
        
        /*! @brief Maps a given vertex's descriptor to an AttributesMap. 
         *
         * Implementation of this function should be done for one shader. It basically maps all shader's 
         * attributes to each component of VertexDescriptor (i.e. kVertexComponentColor, kVertexComponentNormal, ...).
         * A VertexComponent is bound to a ShaderAttribute structure which will be used by the driver to draw the render 
         * command.
         *
        **/
        virtual ShaderAttributesMap map(VertexDescriptor const& descriptor, Shader const& shader) = 0; 
        
        /* Mapper is used like this example:
        
        {
            ShaderAttributesMap result;
            
            if (descriptor.has(kVertexComponentColor) && shader.hasAttribute("color")) 
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentColor);
                
                ShaderAttribute attrib = { 
                    shader.findAttributeIndex("color"), // Attrib index
                    infos.offset, // Buffer offset
                    infos.stride, // Buffer stride
                    infos.buffer
                };
                
                result.add(std::move(attrib));
            }
            
            if (descriptor.has(kVertexComponentPosition) && shader.hasAttribute("position"))
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentPosition);
                
                ShaderAttribute attrib = { 
                    shader.findAttributeIndex("position"), // Attrib index
                    infos.offset, // Buffer offset
                    infos.stride, // Buffer stride
                    infos.buffer
                };
                
                result.add(std::move(attrib));
            }
            
            if (descriptor.has(kVertexComponentTexture) && shader.hasAttribute("texture"))
            {
                auto& infos = descriptor.findInfosFor(kVertexComponentTexture);
                
                ShaderAttribute attrib = { 
                    shader.findAttributeIndex("texture"), // Attrib index
                    infos.offset, // Buffer offset
                    infos.stride, // Buffer stride
                    infos.buffer
                };
                
                result.add(std::move(attrib));
            }
        }
        
        */
    };
}

#endif