/** \file GlDriver/GlBuffer.cpp
**/

#include "GlBuffer.h"
#include "GlCheckError.h"
#include "GlDriver.h"

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

GlBuffer::GlBuffer(GlDriver* driver, std::uint8_t glType, GLsizeiptr glSize, GLvoid* ptr, GLenum glUsage)
    : Buffer(driver), gl(driver->glTable), usage(glUsage), type(glType), size(glSize)
{
    gl.genBuffers(1, &handle);
    assert(handle && "OpenGL can't create more buffer handles.");
    
    if (type == kBufferTypeVertex) target = GL_ARRAY_BUFFER;
    else if (type == kBufferTypeIndex) target = GL_ELEMENT_ARRAY_BUFFER;
    else target = GL_INVALID_ENUM;
    
    if (glSize) {
        gl.bindBuffer(target, handle);
        
        if (ptr) {
            gl.bufferData(target, size, ptr, usage);
        }
        else {
            gl.bufferData(target, size, NULL, usage);
        }
    }
    
    GlError error = GlCheckError(gl.getError);
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
    gl.bindBuffer(target, handle);
    
    switch(io)
    {
        case kBufferIOReadOnly: return gl.mapBuffer(target, GL_READ_ONLY);
        case kBufferIOWriteOnly: return gl.mapBuffer(target, GL_WRITE_ONLY);
        case kBufferIOReadWrite: return gl.mapBuffer(target, GL_READ_WRITE);
        default: return nullptr;
    }
}

void GlBuffer::unlock(std::uint8_t)
{
    gl.unmapBuffer(target);
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
        gl.genBuffers(1, &handle);
        released = false;
    }
    
    gl.bindBuffer(target, handle);
    gl.bufferData(target, static_cast < GLsizeiptr >(sz), static_cast < const GLvoid* >(data), GlBufferUsage(usg));
    
    GlError error = GlCheckError(gl.getError);
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
    gl.bindBuffer(target, handle);
}

void GlBuffer::unbind(Driver&) const 
{
    gl.bindBuffer(target, 0);
}

std::uint8_t GlBuffer::getType() const
{
    return type;
}

void GlBuffer::releaseResource()
{
    gl.deleteBuffers(1, &handle);
    handle = 0;
    usage = GL_INVALID_ENUM;
    size = 0;
    released = true;
}
