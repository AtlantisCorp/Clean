/** \file Core/Allocate.h
**/

#ifndef CLEAN_ALLOCATE_H
#define CLEAN_ALLOCATE_H

#include <memory>
#include <cstdint>
#include <utility>

#ifdef CLEAN_DEBUG
#include <map>
#include <typeinfo>
#include <typeindex>
#include <mutex>
#include <atomic>

#endif // CLEAN_DEBUG

namespace Clean
{
#   ifdef CLEAN_DEBUG
    /** @brief Keeps information about an allocation. */
    struct Allocation final
    {
        //! @brief Pointer associated to the allocation.
        std::uintptr_t address;

        //! @brief Length allocated to this address.
        std::size_t size;

        //! @brief Number of elements.
        std::uint32_t elements;

        //! @brief Type index for the elements allocated.
        std::type_index type;
    };

    /** @brief Keeps track of memory allocation and deallocation. */
    class MemoryTracker final
    {
        //! @brief Total number of bytes allocated.
        std::atomic < std::uint64_t > bytesAllocated;

        //! @brief Total number of bytes deallocated.
        std::atomic < std::uint64_t > bytesDeallocated;

        //! @brief Allocations currently active.
        std::map<std::uintptr_t, Allocation> activeAllocs;

        //! @brief Mutex used to access data.
        std::mutex allocMutex;

    private:

        /*! @brief Private constructor. */
        MemoryTracker() = default;

        /*! @brief Private destructor. */
        ~MemoryTracker() = default;

    public:

        /*! @brief Retrieves the global tracker. */
        static MemoryTracker& Get();

        /*! @brief Pushes an allocation. */
        void pushAllocation(std::uintptr_t address, std::size_t size, std::size_t elements, std::type_index type);

        /*! @brief Pops an allocation. */
        void popAllocation(std::uintptr_t address);

        /*! @brief Returns a copy of activeAllocs. */
        std::map < std::uintptr_t, Allocation > getActiveAllocations() const;

        /*! @brief Returns total number of bytes allocated since beginning. */
        std::uint64_t getTotalBytesAllocated() const;

        /*! @brief Returns total number of bytes deallocated since beginning. */
        std::uint64_t getTotalBytesFreed() const;

        /*! @brief Returns amount of bytes currently allocated. */
        std::uint64_t getCurrentBytesAllocated() const;
    };

    /** @brief Uses MemoryTracker to track allocations and deallocations. */
    template < typename T >
    class Allocator : public std::allocator<T>
    {
    public:

        typedef size_t    size_type;
        typedef ptrdiff_t difference_type;
        typedef T*        pointer;
        typedef const T*  const_pointer;
        typedef T&        reference;
        typedef const T&  const_reference;
        typedef T         value_type;

        template < typename U >
        struct rebind { typedef Allocator<U> other; };

        T* allocate(std::size_t n, std::allocator<void>::value_type* hint = 0)
        {
            static MemoryTracker& tracker = MemoryTracker::Get();
            T* result = std::allocator<T>::allocate(n);
            assert(result && "std::allocator failed.");

            tracker.pushAllocation(static_cast<std::uintptr_t>(result), n*sizeof(T), n, typeid(T));
            return result;
        }

        void deallocate(T* p)
        {
            assert(p && "Null pointer given to deallocate.");
            static MemoryTracker& tracker = MemoryTracker::Get();
            tracker.popAllocation(static_cast<std::uintptr_t>(p));
            std::allocator<T>::deallocate(p);
        }
    };

    /*! @brief Allocates memory with the Clean::Allocator allocator.
     *
     * \param n Number of elements to allocate. Notes all those elements are allocated
     *      and constructed with given args.
    **/
    template < typename Result, typename... Args >
    Result* Allocate(std::size_t n, Args&&... args)
    {
        static Allocator < Result > allocator;
        Result* pointer = allocator.allocate(n);

        for (std::size_t i = 0; i < n; ++i)
            ::new ((void*)pointer+i) Result(std::forward<Args>(args)...);

        return pointer;
    }

    /*! @brief Allocates a new std::shared_ptr initialized with Allocator structure. */
    template < typename Result, typename... Args >
    std::shared_ptr < Result > AllocateShared(Args&&... args)
    {
        static Allocator < Result > allocator;
        return std::allocate_shared<Result>(allocator, std::forward<Args>(args)...);
    }
    
    /*! @brief Free memory with the Clean::Allocator. */
    inline void Free(void* data) 
    {
        static Allocator < void* > allocator;
        allocator.deallocate(data);
    }

#   else
    /*! @brief Allocates memory with std::allocator.
     *
     * \param n Number of elements to allocate. Notes all those elements are allocated
     *      and constructed with given args.
    **/
    template < typename Result, typename... Args >
    Result* Allocate(std::size_t n, Args&&... args)
    {
        static std::allocator < Result > allocator;
        Result* pointer = allocator.allocate(n);

        for (std::size_t i = 0; i < n; ++i)
            ::new ((void*)(pointer+i)) Result(std::forward<Args>(args)...);

        return pointer;
    }

    /*! @brief Allocates a new std::shared_ptr initialized with std::allocator structure. */
    template < typename Result, typename... Args >
    std::shared_ptr < Result > AllocateShared(Args&&... args)
    {
        return std::make_shared<Result>(std::forward<Args>(args)...);
    }
    
    /*! @brief Free memory with the Clean::Allocator. */
    inline void Free(void* data) 
    {
        static std::allocator < void* > allocator;
        allocator.deallocate(data);
    }

#   endif
}

#endif // CLEAN_ALLOCATE_H
