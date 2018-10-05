/** \file Core/GenBuffer.h
**/

#ifndef CLEAN_GENBUFFER_H
#define CLEAN_GENBUFFER_H

#include "Buffer.h"

#include <shared_mutex>

namespace Clean 
{
    /** @brief Generic RAM buffer. 
     *
     * This is a simple implementation of \ref Buffer for RAM buffers. As they don't need any API
     * to be managed, they are called Generic buffers. They can hold any type of data, and are locked
     * using a std::shared_mutex. Locking with kBufferIOReadOnly uses lock_shared() function to allow
     * multiple read accesses that are not causing data races.
     *
    **/
    class GenBuffer final : public Buffer
    {
        //! @brief Pointer to the data of this buffer. 
        std::uint8_t* pointer;
        
        //! @brief Size of the current buffer. 
        std::atomic < std::size_t > size;
        
        //! @brief Type of this buffer. 
        std::atomic < std::uint8_t > type;
        
        //! @brief Usage of this buffer. 
        std::atomic < std::uint8_t > usage;
        
        //! @brief Shared mutex to lock for read/write operations. 
        mutable std::shared_mutex mutex;
        
    public:
        
        /*! @brief Constructs a GenBuffer with data and size. 
         * 
         * \param[in] data Data to use to initialize the buffer. If null, the buffer is allocated 
         *      and zero'd. If acquire is true, data is not copied but acquired by this buffer. 
         * \param[in] size_ Size of data, in bytes. If zero, an assertion check will fail.
         * \param[in] usage_ Usage for the buffer. Can be kBufferUsageStatic/Dynamic/Stream.
         * \param[in] type_ Type to use for this buffer. Default is kBufferTypeVertex. 
         * \param[in] acquire True if this buffer acquire the given memory without copying it. Default
         *      is false. 
         *
        **/
        GenBuffer(const void* data, std::size_t size_, 
            std::uint8_t usage_ = kBufferUsageDynamic, 
            std::uint8_t type_ = kBufferTypeVertex, 
            bool acquire = false);
        
        /*! @brief Delete copy constructor. */
        GenBuffer(GenBuffer const&) = delete;
        
        /*! @brief Destructs the GenBuffer and clears its data if needed. */
        ~GenBuffer();
        
        /*! @brief Returns a pointer to the data held by this buffer. */
        const void* getData() const;

        /*! @brief Locks the buffer for given operation and returns its data pointer. */
        void* lock(std::uint8_t io = kBufferIOReadWrite);

        /*! @brief Unlocks the buffer. */
        void unlock(std::uint8_t io = kBufferIOReadWrite);

        /*! @brief Returns buffer's size. */
        std::size_t getSize() const;

        /*! @brief Returns kBufferDataUnknown. */
        std::uint8_t getDataType() const;
        
        /*! @brief Returns type. */
        std::uint8_t getType() const;
        
        /*! @brief Updates the buffer with new data. 
         * 
         * Any pre-existing data is deleted, and a new buffer is created. Depending on acquire value,
         * given data is moved (acquire is true) or copied into a new buffer (acquire is false). While
         * moving only the data is faster and does not imply a new allocation call, it requires the user
         * not to delete this data externally. Using the default copying method if you don't have full
         * control over that memory.
         *
        **/
        void update(const void* data_, std::size_t size_, std::uint8_t usage_, bool acquire = false);
       
        /*! @brief Returns the usage for this buffer. */
        std::uint8_t getUsage() const;
        
       /*! @brief Returns always false. **/
       virtual bool isBindable() const;
       
       /*! @brief Does nothing. */
       virtual void bind(Driver& driver) const;
       
       /*! @brief Does nothing. */
       virtual void unbind(Driver& driver) const;
    };
}

#endif // CLEAN_GENBUFFER_H
