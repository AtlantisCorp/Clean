/** \file Core/VertexDescriptor.h
**/

#ifndef CLEAN_VERTEXDESCRIPTOR_H
#define CLEAN_VERTEXDESCRIPTOR_H

#include "Buffer.h"
#include "IndexedInfos.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <map>

namespace Clean 
{
    /*! @brief Informations filled before findInfosFor is called. */
    struct VertexComponentPartialInfos
    {
        //! @brief Offset of the component in the Vertex structure. 
        std::ptrdiff_t offset;
        
        //! @brief Stride between two components in the Vertex structure.
        std::ptrdiff_t stride;
    };

    /** @brief Represents a raw information about a VertexComponent for one submesh.
     *
     * It is generated by VertexDescriptor::findInfosFor. See Mesh::findAssociatedDescriptors for more
     * on generating VertexComponentInfos for a ShaderMapper.
     *
    **/
    struct VertexComponentInfos
    {
        //! @brief Offset of the component in the buffer, in bytes.
        std::ptrdiff_t offset;

        //! @brief Stride between two components in the buffer, in bytes. In packed buffers,
        //! this value is zero (no stride between two components).
        std::ptrdiff_t stride;

        //! @brief Number of elements in the buffer, *not* in bytes.
        std::size_t elements;

        //! @brief Buffer associated to this component.
        std::shared_ptr < Buffer > buffer;
    };
    
    //! @defgroup VertexComponentGroup Vertex Components predefined.
    //! @brief Defines some used Vertex Components. All Components >= to kVertexComponentMax are
    //! considered custom components that can be used for any purpose. 
    
    //! @{

    typedef std::uint8_t VertexComponent;
    static constexpr const std::uint8_t kVertexComponentNull = 0;
    static constexpr const std::uint8_t kVertexComponentPosition = 1;
    static constexpr const std::uint8_t kVertexComponentColor = 2;
    static constexpr const std::uint8_t kVertexComponentTexture = 3;
    static constexpr const std::uint8_t kVertexComponentNormal = 4;
    static constexpr const std::uint8_t kVertexComponentTangent = 5;
    static constexpr const std::uint8_t kVertexComponentBiTangent = 6;
    static constexpr const std::uint8_t kVertexComponentOther1 = 7;
    static constexpr const std::uint8_t kVertexComponentOther2 = 8;
    static constexpr const std::uint8_t kVertexComponentOther3 = 9;
    static constexpr const std::uint8_t kVertexComponentMax = 10;
    
    //! @}

    struct VertexDescriptor
    {
        //! @brief Resident data found in Mesh::vdescriptor. It is common for all submeshes present
        //! in the same mesh,as their data representation is assumed to be the same.
        std::map < VertexComponent, VertexComponentPartialInfos > components;

        //! @brief Local data valid only when Mesh::findAssociatedDescriptors or Mesh::findDescriptors are
        //! called to populate this structure. It is different for all submeshes and represents the local data
        //! for a submesh.
        struct { std::size_t offset; std::size_t elements; std::shared_ptr < Buffer > buffer; } localSubmesh;
        
        //! @brief IndexedInfos struct for the driver to draw indexed instanced objects. 
        IndexedInfos indexInfos;

        /*! @brief Populates a VertexComponentInfos structure from components and localSubmesh data. */
        VertexComponentInfos findInfosFor(VertexComponent const& component);

        /*! @brief Returns true if the given component is found in a Vertex. */
        bool has(VertexComponent const& component);
        
        /*! @brief Adds a component to this VertexDescriptor. */
        void addComponent(std::uint8_t component, std::ptrdiff_t offset, std::ptrdiff_t stride);
    };
}

#endif // CLEAN_VERTEXDESCRIPTOR_H
