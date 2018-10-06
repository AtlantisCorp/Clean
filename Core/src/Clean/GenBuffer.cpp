/** \file Core/GenBuffer.cpp
**/

#include "GenBuffer.h"
#include "Allocate.h"

namespace Clean 
{
    GenBuffer::GenBuffer(const void* data, std::size_t size_, std::uint8_t usage_, std::uint8_t type_, bool acquire)
    {
        assert(size_ && "Invalid size given for GenBuffer.");
        
        size.store(size_);
        type.store(type_);
        usage.store(usage_);
        
        if (acquire) {
            pointer = static_cast < std::uint8_t* >(const_cast < void* >(data));
        }
        
        else {
            pointer = Allocate < std::uint8_t >(size_);
            if (!pointer) throw std::bad_alloc();
            if (data) memcpy(pointer, data, size_);
            else memset(pointer, 0, size_);
        }
    }
    
    GenBuffer::~GenBuffer()
    {
        if (pointer) {
            Free(pointer);
        }
    }
    
    const void* GenBuffer::getData() const 
    {
        return static_cast < const void* >(pointer);
    }
    
    void* GenBuffer::lock(std::uint8_t io)
    {
        switch(io)
        {
            case kBufferIOReadOnly:
            mutex.lock_shared();
            return pointer;
            
            case kBufferIOReadWrite:
            case kBufferIOWriteOnly:
            mutex.lock();
            return pointer;
            
            default:
            return nullptr;
        }
    }
    
    void GenBuffer::unlock(std::uint8_t io)
    {
        switch(io)
        {
            case kBufferIOReadOnly:
            mutex.unlock_shared();
            return;
            
            case kBufferIOReadWrite:
            case kBufferIOWriteOnly:
            mutex.unlock();
            return;
        }
    }
    
    std::size_t GenBuffer::getSize() const 
    {
        return size.load();
    }
    
    std::uint8_t GenBuffer::getDataType() const 
    {
        return kBufferDataUnknown;
    }
    
    std::uint8_t GenBuffer::getType() const 
    {
        return type.load();
    }
    
    void GenBuffer::update(const void* data_, std::size_t size_, std::uint8_t usage_, bool acquire)
    {
        void* data = lock(kBufferIOWriteOnly);
        
        if (data) {
            Free((std::uint8_t*)data);
            pointer = nullptr;
            size.store(0);
        }
        
        if (!size_) {
            unlock(kBufferIOWriteOnly);
            return;
        }
        
        if (acquire) {
            pointer = static_cast < std::uint8_t* >(const_cast < void* >(data));
        }
        
        else {
            pointer = Allocate < std::uint8_t >(size_);
            if (!pointer) throw std::bad_alloc();
            if (data_) memcpy(pointer, data_, size_);
            else memset(pointer, 0, size_);
        }
        
        unlock(kBufferIOWriteOnly);
        
        size.store(size_);
        usage.store(usage_);
    }
    
    std::uint8_t GenBuffer::getUsage() const 
    {
        return usage.load();
    }
    
    bool GenBuffer::isBindable() const 
    {
        return false;
    }
    
    void GenBuffer::bind(Driver&) const
    {
        
    }
    
    void GenBuffer::unbind(Driver&) const
    {
        
    }
    
    void GenBuffer::releaseResource()
    {
        if (pointer) {
            Free(pointer);
            pointer = nullptr;
            size.store(0);
        }
    }
}
