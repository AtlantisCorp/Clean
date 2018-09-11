/** \file Core/Manager.h
**/

#ifndef CLEAN_MANAGER_H
#define CLEAN_MANAGER_H

#include <cstdint>
#include <list>
#include <memory>
#include <mutex>

namespace Clean 
{
    /** @brief Base class for Managers. */
    template < class Managed > 
    class Manager 
    {
    protected:
        
        //! @brief List of objects managed. 
        std::list < std::shared_ptr < Managed > > managedList;
        
        //! @brief Protects managedList.
        mutable std::mutex managedListMutex;
        
    public:
        
        /*! @brief Default constructor. */
        Manager() = default;
        
        /*! @brief Default destructor. */
        virtual ~Manager() = default;
        
        /*! @brief Adds a managed object. */
        virtual void add(std::shared_ptr < Managed > const& rhs)
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            managedList.push_back(rhs);
        }
        
        /*! @brief Adds a managed object and asserts it has not already been added. */
        virtual void addOnce(std::shared_ptr < Managed > const& rhs)
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            auto check = std::find(managedList.begin(), managedList.end(), rhs);
            
            if (check == managedList.end()) 
                managedList.push_back(rhs);
        }
        
        /*! @brief Removes a managed object. */
        virtual void erase(std::shared_ptr < Managed > const& rhs)
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            auto check = std::find(managedList.begin(), managedList.end(), rhs);
            
            if (check != managedList.end()) 
                managedList.erase(check);
        }
        
        /*! @brief Returns true if empty. */
        virtual bool empty() const 
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            return managedList.empty();
        }
        
        /*! @brief Returns number of objects. */
        virtual std::size_t count() const 
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            return managedList.size();
        }
        
        /*! @brief Returns a begin iterator. */
        auto begin() -> decltype(managedList.begin()) { return managedList.begin(); }
        /*! @brief Returns a begin iterator. */
        auto begin() const -> decltype(managedList.begin()) { return managedList.begin(); }
        /*! @brief Returns the end iterator. */
        auto end() -> decltype(managedList.end()) { return managedList.end(); }
        /*! @brief Returns the end iterator. */
        auto end() const -> decltype(managedList.end()) { return managedList.end(); }
        
        /*! @brief Convenient function to call a callback for each managed object and locking the 
         * managed list. */
        template < typename Callable >
        void forEach(Callable cbk) 
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            
            for (auto& managed : managedList)
            {
                cbk(managed);
            }
        }
        
        /*! @brief Calls a callback on each managed objects in a non-blocking way.
         *
         * When this function is called, it copies the current list of managed objects and then call
         * the given callback on each object of the copied list. Mutex is unlocked before calling the
         * callback, thus liberating other objects to using this Manager. 
         *
        **/
        template < typename Callback > void forEachCpy(Callback cbk)
        {
            managedListMutex.lock();
            auto listCopy = managedList;
            managedListMutex.unlock();
            
            for (auto& managed : listCopy)
            {
                cbk(managed);
            }
        }
        
        /*! @brief Returns true if the given managed object is already in this manager. */
        bool exists(std::shared_ptr < Managed > const& rhs) const 
        {
            std::lock_guard < std::mutex > lck(managedListMutex);
            return std::find(managedList.begin(), managedList.end(), rhs) != managedList.end();
        }
    };
}

#endif // CLEAN_MANAGER_H