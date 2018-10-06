/** \file Core/Buffer.h
**/

#ifndef CLEAN_BUFFER_H
#define CLEAN_BUFFER_H

#include "Handled.h"
#include "DriverResource.h"

namespace Clean
{
    //! @defgroup BufferIOOperationsGroup Buffer IO Operations
    /** @brief Defines IO operations for a buffer. 
     *  @{ 
    **/
    
    static constexpr const std::uint8_t kBufferIONull = 0;
    static constexpr const std::uint8_t kBufferIOReadOnly = 1;
    static constexpr const std::uint8_t kBufferIOWriteOnly = 2;
    static constexpr const std::uint8_t kBufferIOReadWrite = 3;
    
    /** @} */
    
    /** @defgroup BufferTypeGroup Buffer generic types 
     *  @{ 
    **/
    
    static constexpr const std::uint8_t kBufferTypeVertex = 0;
    static constexpr const std::uint8_t kBufferTypeIndex = 1;
    
    /** @} */
    
    static constexpr const std::uint8_t kBufferDataUnknown = 0;
    
    static constexpr const std::uint8_t kBufferUsageStatic = 1;
    static constexpr const std::uint8_t kBufferUsageDynamic = 2;
    static constexpr const std::uint8_t kBufferUsageStream = 3;

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
    class Buffer : public DriverResource, public Handled < Buffer >
    {
    public:
        
        /*! @brief Constructs a new Buffer. */
        Buffer(Driver* driver = nullptr);

        /*! @brief Default destructor. */
        virtual ~Buffer() = default;

        /*! @brief Returns a pointer to the data held by this buffer. */
        virtual const void* getData() const = 0;

        /*! @brief Locks the buffer for given operation and returns its data pointer. */
        virtual void* lock(std::uint8_t io = kBufferIOReadWrite) = 0;

        /*! @brief Unlocks the buffer. */
        virtual void unlock(std::uint8_t io = kBufferIOReadWrite) = 0;

        /*! @brief Returns buffer's size. */
        virtual std::size_t getSize() const = 0;

        /*! @brief Returns buffer's data type. */
        virtual std::uint8_t getDataType() const = 0;
        
        /*! @brief Updates the buffer with new data. 
         * 
         * Any pre-existing data is deleted, and a new buffer is created. Depending on acquire value,
         * given data is moved (acquire is true) or copied into a new buffer (acquire is false). While
         * moving only the data is faster and does not imply a new allocation call, it requires the user
         * not to delete this data externally. Using the default copying method if you don't have full
         * control over that memory. 
         *
         * \note If the buffer is not stored into RAM, acquire is ignored and the data is always copied.
         * No data can be moved from RAM to VRAM. 
         *
        **/
        virtual void update(const void* data, std::size_t size, std::uint8_t usage, bool acquire = false) = 0;
        
        /*! @brief Returns the usage for this buffer. */
        virtual std::uint8_t getUsage() const = 0;
        
        /*! @brief Returns true if the buffer is 'bindable'. Any buffer which is created by a driver should
         *  be bindable. However, buffers created in RAM, like GenBuffer, are not bindable.
        **/
        virtual bool isBindable() const = 0;
        
        /*! @brief Binds the buffer to the given driver, if possible. */
        virtual void bind(Driver& driver) const = 0;
        
        /*! @brief Unbinds the buffer from the given driver, if possible. */
        virtual void unbind(Driver& driver) const = 0;
        
        /*! @brief Returns the base type for this buffer. */
        virtual std::uint8_t getType() const = 0;
    };
}

#endif // CLEAN_BUFFER_H
