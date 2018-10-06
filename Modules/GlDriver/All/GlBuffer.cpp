/** \file GlDriver/GlBuffer.cpp
**/

#include "GlBuffer.h"
#include "GlCheckError.h"

#include <Clean/NotificationCenter.h>
using namespace Clean;

GLenum GlBufferUsage(std::uint8_t usage) 
{
    switch(usage)
    {
        case kBufferUsageStatic: return GL_STATIC_DRAW;
        case kBufferUsageDynamic: return GL_DYNAMIC_DRAW;
        case kBufferUsageStream: return GL_STREAM_DRAW;
        default: return GL_INVALID_ENUM;
    }
}

GlBuffer::GlBuffer(Driver* driver, std::uint8_t glType, GLsizeiptr glSize, GLvoid* ptr, GLenum glUsage)
    : Buffer(driver), usage(glUsage), type(glType), size(glSize)
{
    glGenBuffers(1, &handle);
    assert(handle && "OpenGL can't create more buffer handles.");
    
    if (type == kBufferTypeVertex) target = GL_ARRAY_BUFFER;
    else if (type == kBufferTypeIndex) target = GL_ELEMENT_ARRAY_BUFFER;
    else target = GL_INVALID_ENUM;
    
    if (glSize) {
        glBindBuffer(target, handle);
        
        if (ptr) {
            glBufferData(target, size, ptr, usage);
        }
        else {
            glBufferData(target, size, NULL, usage);
        }
    }
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError, "OpenGL returned error: %s.", error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
}

GlBuffer::~GlBuffer()
{
    if (!released) {
        releaseResource();
    }
}

const void* GlBuffer::getData() const 
{
    return nullptr;
}

void* GlBuffer::lock(std::uint8_t io)
{
    if (!handle) return nullptr;
    glBindBuffer(target, handle);
    
    switch(io)
    {
        case kBufferIOReadOnly: return glMapBuffer(target, GL_READ_ONLY);
        case kBufferIOWriteOnly: return glMapBuffer(target, GL_WRITE_ONLY);
        case kBufferIOReadWrite: return glMapBuffer(target, GL_READ_WRITE);
        default: return nullptr;
    }
}

void GlBuffer::unlock(std::uint8_t)
{
    glUnmapBuffer(target);
}

std::size_t GlBuffer::getSize() const 
{
    return static_cast < std::size_t >(size);
}

std::uint8_t GlBuffer::getDataType() const 
{
    return kBufferDataUnknown;
}

void GlBuffer::update(const void* data, std::size_t sz, std::uint8_t usg, bool /* acquire */)
{
    if (!handle) {
        glGenBuffers(1, &handle);
        released = false;
    }
    
    glBindBuffer(target, handle);
    glBufferData(target, static_cast < GLsizeiptr >(sz), static_cast < const GLvoid* >(data), GlBufferUsage(usg));
    
    GlError error = GlCheckError();
    if (error.error != GL_NO_ERROR) {
        Notification notif = BuildNotification(kNotificationLevelError, "OpenGL returned error: %s.", error.string.data());
        NotificationCenter::GetDefault()->send(notif);
    }
    
    size = static_cast < GLsizeiptr >(sz);
    usage = GlBufferUsage(usg);
}

std::uint8_t GlBuffer::getUsage() const 
{
    switch(usage)
    {
        case GL_STATIC_DRAW: 
        case GL_STATIC_READ:
        case GL_STATIC_COPY: return kBufferUsageStatic;
        
        case GL_DYNAMIC_DRAW:
        case GL_DYNAMIC_READ:
        case GL_DYNAMIC_COPY: return kBufferUsageDynamic;
        
        case GL_STREAM_DRAW:
        case GL_STREAM_READ:
        case GL_STREAM_COPY: return kBufferUsageStream;
        
        default: return 0;
    }
}

bool GlBuffer::isBindable() const 
{
    return handle != 0;
}

void GlBuffer::bind(Driver&) const 
{
    glBindBuffer(target, handle);
}

void GlBuffer::unbind(Driver&) const 
{
    glBindBuffer(target, 0);
}

std::uint8_t GlBuffer::getType() const
{
    return type;
}

void GlBuffer::releaseResource()
{
    glDeleteBuffers(1, &handle);
    handle = 0;
    usage = GL_INVALID_ENUM;
    size = 0;
    released = true;
}
