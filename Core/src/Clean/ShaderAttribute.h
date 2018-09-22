/** \file Core/ShaderAttribute.h
**/

#ifndef CLEAN_SHADERATTRIBUTE_H
#define CLEAN_SHADERATTRIBUTE_H

#include "Buffer.h"
#include "IndexedInfos.h"

#include <cstdint>
#include <memory>
#include <array>

namespace Clean 
{
    class Buffer;
    
    //! @brief Defines what is the maximum number of ShaderAttributes for one shader. This value is 
    //! based on what might be used by a shader. Generally we will have positions, colors, textures, 
    //! normals, tangents, bitangents, which makes only 6 attributes. 
    static constexpr const std::uint8_t kShaderAttributeMax = 20;
    
    /** @brief Generic attribute in a shader. */
    struct ShaderAttribute final
    {
        //! @brief Index used in the shader to identify the attribute.
        std::uint8_t index = 0;
        
        //! @brief Offset in the buffer to localize first data, in bytes. 
        std::ptrdiff_t offset = 0;
        
        //! @brief Stride between two data, in bytes.
        std::ptrdiff_t stride = 0;
        
        //! @brief Number of elements to bind.
        std::size_t elements = 0;
        
        //! @brief Buffer used to bind data. 
        std::shared_ptr < Buffer > buffer = nullptr;
        
        //! @brief Tells if the attribute is enabled.
        bool enabled = false;
        
        /*! @brief Constructs a default enabled ShaderAttribute. */
        static ShaderAttribute Enabled(std::uint8_t index, std::ptrdiff_t offset, std::ptrdiff_t stride, std::size_t elements,
                                       std::shared_ptr < Buffer > const& buffer);
        
        /*! @brief Constructs a default disabled ShaderAttribute. */
        ShaderAttribute() = default; 
        
        /*! @brief Moves ShaderAttribute. */
        ShaderAttribute(ShaderAttribute&&) = default;
        
        /*! @brief Copies ShaderAttribute. */
        ShaderAttribute(ShaderAttribute const&) = default;
    };
    
    /** @brief Stores ShaderAttributes into a map-like object. 
     *
     * ShaderAttributes are stored into a std::array of ShaderAttributes. All ShaderAttributes are disabled by default,
     * and user must add enabled attributes when creating this map. A ShaderAttributesMap is generally returned by 
     * ShaderMapper::map() to let a user register a sub command to a RenderCommand. 
     *
     * Thread safety and performances
     * It has been decided not to include a mutex protection, nor atomic operations to this class's members, because it
     * should be used only by one thread at a time. Indeed, when constructing a RenderCommand, an instance of this class 
     * is manipulated only by the class which creates it. Therefore, it is manipulated only by the Driver, on its render
     * thread. No one will manipulate the same instance of this class at the same time. Finally, no mutex locking is better
     * for performances, as this class members will be accessed each frame by the driver. 
     *
     * Indexed instancing of Shader Attributes
     * When you want to draw indexed vertexes, shader attributes will still reflect your vertex descriptor. However, indexes
     * must be declared for the whole ShaderAttributesMap. Fill VertexDescriptor::indexInfos to provide information for the
     * future ShaderAttributesMap. 
     *
    **/
    class ShaderAttributesMap final
    {
        //! @brief Stores ShaderAttributes by value. 
        std::array < ShaderAttribute, kShaderAttributeMax > attribs;
        
        //! @brief Stores indexed instancing informations.
        IndexedInfos indexInfos;
        
    public:
        
        /*! @brief Default constructor. */
        ShaderAttributesMap() = default;
        
        /*! @brief Constructs a ShaderAttributesMap with a shared IndexedInfos structure. */
        ShaderAttributesMap(IndexedInfos const& infos);
        
        /*! @brief Default copy constructor. */
        ShaderAttributesMap(ShaderAttributesMap const&) = default;
        
        /*! @brief Adds a ShaderAttribute to this map. */
        void add(ShaderAttribute&& attrib);
        
        /*! @brief Returns a copy of the given ShaderAttribute. */
        ShaderAttribute find(std::uint8_t index);
        
        /*! @brief Disable attributes at given index. */
        void disable(std::uint8_t index);
        
        /*! @brief Enable attributes at given index. */
        void enable(std::uint8_t index);
        
        /*! @brief Returns true if ShaderAttribute::enabled is true at given index. */
        bool isEnabled(std::uint8_t index);
        
        /*! @brief Returns indexInfos. */
        IndexedInfos getIndexedInfos() const;
    };
}

#endif // CLEAN_SHADERATTRIBUTE_H