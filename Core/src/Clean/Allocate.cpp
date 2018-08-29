/** \file Core/Allocate.cpp
**/

#include "Allocate.h"

namespace Clean 
{
#   ifdef CLEAN_DEBUG
    MemoryTracker& MemoryTracker::Get()
    {
        static MemoryTracker instance;
        return instance;
    }
    
    void MemoryTracker::pushAllocation(std::uintptr_t address, std::size_t size, std::size_t elements, std::type_index type)
    {
        bytesAllocated += size;
        Allocation alloc = { address, size, elements, type };
        std::lock_guard < std::mutex > lock(allocMutex);
        activeAllocs[address] = alloc;
    }
    
    void MemoryTracker::popAllocation(std::uintptr_t address)
    {
        std::lock_guard < std::mutex > lock(allocMutex);
        auto it = activeAllocs.find(address);
        assert(it != activeAllocs.end() && "Invalid address deallocated to MemoryTracker.");
        
        bytesDeallocated += (*it).second.size;
        activeAllocs.erase(it);
    }
    
    std::map < std::uintptr_t, Allocation > MemoryTracker::getActiveAllocations() const 
    {
        std::lock_guard < std::mutex > lock(allocMutex);
        return activeAllocs;
    }
    
    std::uint64_t MemoryTracker::getTotalBytesAllocated() const 
    {
        return bytesAllocated.load();
    }
    
    std::uint64_t MemoryTracker::getTotalBytesFreed() const 
    {
        return bytesDeallocated.load();
    }
    
    std::uint64_t MemoryTracker::getCurrentBytesAllocated() const 
    {
        return bytesAllocated.load() - bytesDeallocated.load();
    }
        
#   endif
}