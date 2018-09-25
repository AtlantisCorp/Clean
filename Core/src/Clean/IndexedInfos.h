/** \file Core/IndexedInfos.h
**/

#ifndef CLEAN_INDEXEDINFOS_H
#define CLEAN_INDEXEDINFOS_H

#include "Buffer.h"

#include <memory>

namespace Clean 
{
    /*! @brief Generic informations about an indexed rendering element. 
     *
     * Used by RenderSubCommand, ShaderAttributesMap and VertexDescriptor to communicate indexes
     * informations to the Driver that render its related vertexes structure. It is assumed all
     * indexes are always stored as std::uint32_t, as std::uint16_t might be not enough and 
     * std::uint64_t is too big (who will have 9.223372e18 indexes for one object?).
     *
    **/
    struct IndexedInfos 
    {
        //! @brief Offset in the buffer where elements lie (in number of elements).
        std::size_t offset = 0;
        
        //! @brief Number of elements to draw. 
        std::size_t elements = 0;
        
        //! @brief Buffer to bind to retrieve indexes.
        std::shared_ptr < Buffer > buffer = nullptr;
        
        /*! @brief Constructs an IndexedInfos. */
        IndexedInfos() = default;
        
        /*! @brief Constructs an IndexedInfos. */
        IndexedInfos(std::size_t o, std::size_t e, std::shared_ptr < Buffer > const& b) : offset(o), elements(e), buffer(b) {}
    };
}

#endif // CLEAN_INDEXEDINFOS_H
