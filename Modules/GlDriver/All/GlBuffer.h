/** \file GlDriver/GlBuffer.h
**/

#ifndef GLDRIVER_GLBUFFER_H
#define GLDRIVER_GLBUFFER_H

#include "GlInclude.h"
#include <Clean/Buffer.h>

/*! @brief Converts kBufferUsage* constants to GL_*_DRAW constants. */
GLenum GlBufferUsage(std::uint8_t usage);

/** @brief OpenGL implementation of Clean::Buffer. */
class GlBuffer : public Clean::Buffer 
{
    //! @brief Our GL handle. 
    GLint handle;
    
    //! @brief Recorded usage for this buffer. 
    GLenum usage;
    
    //! @brief Recorded type for this buffer. 
    std::uint8_t type;
    
    //! @brief Recorded size for this buffer. 
    GLsizeiptr size;
    
    //! @brief Target used for default binding. kBufferTypeVertex uses GL_ARRAY_BUFFER, kBufferTypeIndex uses 
    //! GL_ElEMENT_ARRAY_BUFFER. Other buffer type may use other targets, and buffer copy may also uses GL_COPY_READ_BUFFER
    //! and GL_COPY_WRITE_BUFFER.
    GLenum target;
    
public:
    
    /*! @brief Constructs a new OpenGL buffer. 
     * 
     * A valid OpenGL Context must be locked and current while creating the buffer. See Driver::makeBuffer
     * for more explanations. 
     *
     * \param[in] glType Type for this buffer. Can be kBufferTypeVertex, kBufferTypeIndex.
     * \param[in] glSize Size for this buffer. 
     * \param[in] ptr Pointer to the beginning of the data we want to copy. 
     * \param[in] glUsage usage for this buffer. 
     *
     * If glSize is zero, no data is allocated and a new handle only is generated for the buffer. If ptr is null,
     * data is allocated for glSize bytes and zero'ed. 
     * 
     * \warn If you want to copy two buffers in VRAM, don't use this function with a locked buffer. Instead, 
     * use Driver::copyBuffers(src, dest). This will copy your two buffers from VRAM correctly. 
     *
    **/
    GlBuffer(Clean::Driver* driver, std::uint8_t glType, GLsizeiptr glSize, GLvoid* ptr, GLenum glUsage);
    
    /*! @brief Destructs the GLBuffer. 
     *  If not released yet, calls releaseResource. Notes that normally, it should be released directly by the 
     *  Driver but in some case, this might be needed.
    **/
    ~GlBuffer();
    
    /*! @brief Returns a null pointer. Use lock/unlock mechanism to map/unmap buffer. */
    const void* getData() const;
    
    /*! @brief Calls glMap with our buffer and returns a pointer to our data. 
     *
     * \param[in] io IO operation for which the buffer should be mapped to. OpenGL implementation may optimize 
     *      buffer mapping depending on wether we only want to read the buffer or if we want to perform writing
     *      or read/write operations. 
     *
    **/
    void* lock(std::uint8_t io = Clean::kBufferIOReadWrite);
    
    /*! @brief Calls glUnmap for our buffer. */
    void unlock(std::uint8_t io = Clean::kBufferIOReadWrite);
    
    /*! @brief Returns the recorded size of the buffer. */
    std::size_t getSize() const;
    
    /*! @brief For now, always return kBufferDataUnknown. */
    std::uint8_t getDataType() const;
    
    /*! @brief Updates the buffer with new data. 
     * 
     * Calls glBufferData and updates the whole buffer with a new size, a new usage and new data. Calling this
     * function with a null pointer fills the buffer with zero. Refers to OpenGL glBufferData documentation for
     * more explanations. 
     * 
     * \note acquire is actually ignored, as memory copy always occurs. 
     *
    **/
    void update(const void* data, std::size_t size, std::uint8_t usage, bool /* acquire */ = false);
    
    /*! @brief Returns buffer's usage. */
    std::uint8_t getUsage() const;
    
protected:
    
    /*! @brief Releases the buffer's handle. */
    void releaseResource();
};

#endif // GLDRIVER_GLBUFFER_H