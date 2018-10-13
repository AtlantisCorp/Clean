/** =======================================================
 *  \file Core/Property.h
 *  \date 10/13/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_PROPERTY_H
#define CLEAN_PROPERTY_H

#include <type_traits>
#include <atomic>
#include <mutex>

namespace Clean 
{
    template < typename Class, typename = void >
    class PropertyLock;
    
    template < typename Class >
    class PropertyLock < Class, typename std::enable_if_t < (sizeof(Class) <= sizeof(void*)) > >
    {
        std::atomic < Class > value;
        mutable Class refValue;
    
    public:
    
        PropertyLock() = default;
        ~PropertyLock() = default;
    
        Class& ref() { refValue = std::atomic_load(&value); return refValue; }
        const Class& ref() const { refValue = std::atomic_load(&value); return refValue; }
    
        void unlock() { std::atomic_store(&value, refValue); }
        void unlock() const { }
    
        Class load() { return std::atomic_load(&value); }
        const Class load() const { return std::atomic_load(&value); }
    
        void store(Class const& rhs) { std::atomic_store(&value, rhs); }
    
        bool isLockFree() const { return value.is_lock_free(); }
    };

    template < typename Class >
    class PropertyLock < Class, typename std::enable_if_t < (sizeof(Class) > sizeof(void*)) > >
    {
        Class value;
        mutable std::mutex mutex;
    
    public:
    
        PropertyLock() = default;
        ~PropertyLock() = default;
    
        Class& ref() { mutex.lock(); return value; }
        const Class& ref() const { mutex.lock(); return value; }
    
        void unlock() const { mutex.unlock(); }
    
        Class load() { std::lock_guard < std::mutex > lck(mutex); return value; }
        const Class load() const { std::lock_guard < std::mutex > lck(mutex); return value; }
    
        void store(Class const& rhs) { std::lock_guard < std::mutex > lck(mutex); value = rhs; };
    
        bool isLockFree() const { return false; }
    };

    /** @brief Templated property class.
     *
     * A Property is an object in a structure or a class which is accessed through atomic or mutex
     * operations, depending on its size. If Double Word swap/exchange operations are available on 
     * the current platform, atomic operations will be extended to double word sized types. 
     *
    **/
    template < typename Class >
    class Property 
    {
        //! @brief Our locked implementation. 
        PropertyLock < Class > lock_;

    public:

        Property() = default;
        virtual ~Property() = default;
    
        /*! @brief Returns a locked reference to the property.
         *
         * \note unlock() must be called after calling this function to allow other thread to access
         * the property. Though, a lock-free Property is optimized for load() instead of lock/unlock.
         *
        **/
        Class& lock() { return lock_.ref(); }
       /*! @brief Returns a locked reference to the property.
        *
        * \note unlock() must be called after calling this function to allow other thread to access
        * the property. Though, a lock-free Property is optimized for load() instead of lock/unlock.
        *
       **/
        const Class& lock() const { return lock_.ref(); }
    
        /*! @brief Unlocks the property. */
        void unlock() { return lock_.unlock(); }
        /*! @brief Unlocks the property. */
        void unlock() const { return lock_.unlock(); }
    
        /*! @brief Copies the actual value of the property and returns it. 
         * 
         * \note When property is lock-free (atomic implementation), this operation is a simple atomic
         * load with default memory order. When non lock-free, internal mutex is locked.
         *
        **/
        Class load() { return lock_.load(); }
       /*! @brief Copies the actual value of the property and returns it. 
        * 
        * \note When property is lock-free (atomic implementation), this operation is a simple atomic
        * load with default memory order. When non lock-free, internal mutex is locked.
        *
       **/
        const Class load() const { return lock_.load(); }
    
        /*! @brief Copies the given value to the stored value. */
        void store(Class const& rhs) { return lock_.store(rhs); }
    
        /*! @brief Returns true if implementation is lock-free (only atomic operations). */
        bool isLockFree() const { return lock_.isLockFree(); }
    };
}

#endif // CLEAN_PROPERTY_H
