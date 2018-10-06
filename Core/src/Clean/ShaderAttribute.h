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
    
    //! @defgroup ShaderAttribGroup Shader's attributes type
    /** @brief Defines generic base type for each attribute's component. For example, a position has
     *  four floats, so its type is kShaderAttribFloat. Some custom types may be handled. 
     * @{
    **/
    
    static constexpr const std::uint8_t kShaderAttribNull = 0;
    static constexpr const std::uint8_t kShaderAttribI8 = 1;
    static constexpr const std::uint8_t kShaderAttribU8 = 2;
    static constexpr const std::uint8_t kShaderAttribI16 = 3;
    static constexpr const std::uint8_t kShaderAttribU16 = 4;
    static constexpr const std::uint8_t kShaderAttribI32 = 5;
    static constexpr const std::uint8_t kShaderAttribU32 = 6;
    static constexpr const std::uint8_t kShaderAttribHalfFloat = 7;
    static constexpr const std::uint8_t kShaderAttribFloat = 8;
    static constexpr const std::uint8_t kShaderAttribDouble = 9;
    
    //! @}
    
    /** @brief Generic attribute in a shader. */
    struct ShaderAttribute final
    {
        //! @brief Index used in the shader to identify the attribute.
        std::uint8_t index = 0;
        
        //! @brief Type of the components to use. 
        std::uint8_t type = kShaderAttribNull;
        
        //! @brief Number of components per generic vertex attribute. Must be 1, 2, 3, or 4.
        std::uint8_t components = 0;
        
        //! @brief Offset in the buffer to localize first data, in bytes. 
        std::ptrdiff_t offset = 0;
        
        //! @brief Stride between two data, in bytes.
        std::ptrdiff_t stride = 0;
        
        //! @brief Buffer used to bind data. 
        std::shared_ptr < Buffer > buffer = nullptr;
        
        //! @brief Tells if the attribute is enabled.
        bool enabled = false;
        
        /*! @brief Constructs a default enabled ShaderAttribute. */
        static ShaderAttribute Enabled(std::uint8_t index, std::uint8_t type, std::uint8_t components,
                                       std::ptrdiff_t offset, std::ptrdiff_t stride,
                                       std::shared_ptr < Buffer > const& buffer);
        
        /*! @brief Constructs a default disabled ShaderAttribute. */
        ShaderAttribute() = default; 
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
        
        //! @brief When not indexed, stores the number of elements to draw.
        std::size_t elements;
        
    public:
        
        /*! @brief Default constructor. */
        ShaderAttributesMap();
        
        /*! @brief Constructs a ShaderAttributesMap with a number of vertexes to draw. */
        ShaderAttributesMap(std::size_t count);
        
        /*! @brief Constructs a ShaderAttributesMap with a shared IndexedInfos structure. */
        ShaderAttributesMap(IndexedInfos const& infos);
        
        /*! @brief Default copy constructor. */
        ShaderAttributesMap(ShaderAttributesMap const&) = default;
        
        /*! @brief Adds a ShaderAttribute to this map. */
        void add(ShaderAttribute&& attrib);
        
        /*! @brief Returns a copy of the given ShaderAttribute. */
        ShaderAttribute find(std::uint8_t index) const;
        
        /*! @brief Disable attributes at given index. */
        void disable(std::uint8_t index);
        
        /*! @brief Enable attributes at given index. */
        void enable(std::uint8_t index);
        
        /*! @brief Returns true if ShaderAttribute::enabled is true at given index. */
        bool isEnabled(std::uint8_t index);
        
        /*! @brief Returns indexInfos. */
        IndexedInfos getIndexedInfos() const;
        
        /*! @brief Changes elements. */
        void setElements(std::size_t count);
        
        /*! @brief Returns elements. */
        std::size_t getElements() const;
        
        /*! @brief Returns true if there are one or more attributes. */
        bool isValid() const;
        
        /*! @brief Returns number of ShaderAttributes in this map. */
        std::size_t countAttributes() const;
    };
}

#endif // CLEAN_SHADERATTRIBUTE_H
