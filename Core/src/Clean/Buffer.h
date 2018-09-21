/** \file Core/Buffer.h
**/

#ifndef CLEAN_BUFFER_H
#define CLEAN_BUFFER_H

#include "Handled.h"

namespace Clean
{
    static constexpr const std::uint8_t kBufferIONull = 0;
    static constexpr const std::uint8_t kBufferIOReadOnly = 1;
    static constexpr const std::uint8_t kBufferIOWriteOnly = 2;
    static constexpr const std::uint8_t kBufferIOReadWrite = 3;

    /** @brief Generic buffer interface.
     *
     * A buffer is a generic range from the start address with a size of given bytes
     * number. A *software* buffer is a buffer located into RAM, a *hardware* buffer
     * is located in VRAM.
     *
     * A buffer may hold some data type, but it is not disallowed not to have any type
     * for the data in the buffer. This results the engine will try to guess what is
     * in the buffer. If we give a buffer for component Position (which is 4 floats),
     * it will assume buffer's data is constitued of floats.
     *
    **/
    class Buffer : public Handled < Buffer >
    {
    public:

        /*! @brief Default destructor. */
        virtual ~Buffer() = default;

        /*! @brief Returns a pointer to the data held by this buffer. */
        virtual const void* getData() = 0;

        /*! @brief Locks the buffer for given operation and returns its data pointer. */
        virtual void* lock(std::uint8_t io = kBufferIOReadWrite) = 0;

        /*! @brief Unlocks the buffer. */
        virtual void unlock(std::uint8_t io = kBufferIOReadWrite) = 0;

        /*! @brief Returns buffer's size. */
        virtual std::size_t getSize() const = 0;

        /*! @brief Returns buffer's data type. */
        virtual std::uint8_t getDataType() const = 0;
    };
}

#endif // CLEAN_BUFFER_H
