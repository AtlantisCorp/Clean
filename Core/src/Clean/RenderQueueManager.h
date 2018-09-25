/** \file Core/RenderQueueManager.h
**/

#ifndef CLEAN_RENDERQUEUEMANAGER_H
#define CLEAN_RENDERQUEUEMANAGER_H

#include "RenderQueue.h"

#include <memory>
#include <cstdint>
#include <vector>
#include <map>
#include <mutex>

namespace Clean 
{
    //! @defgroup RenderQueuePriorityGroup RenderQueue priorities. 
    //! @{
    
    static constexpr const std::uint8_t kRenderQueuePriorityLowest = 0;
    static constexpr const std::uint8_t kRenderQueuePriorityLow = 64;
    static constexpr const std::uint8_t kRenderQueuePriorityMedium = 128;
    static constexpr const std::uint8_t kRenderQueuePriorityHigh = 192;
    static constexpr const std::uint8_t kRenderQueuePriorityHighest = 255;
    
    //! @}
    
    /** @brief Manages all RenderQueues in a driver. 
     *
     * Queues are stored for each priority in a classic std::vector. When accessing a priority,
     * all queues are iterated. To go to the next priority, all queues in the first priority must 
     * have been iterated over. This way, queue with the highest priority are always first. 
     * \see RenderQueueManager::iterator
     *
    **/
    class RenderQueueManager 
    {
        typedef std::shared_ptr < RenderQueue > RQPtr;
        typedef std::vector < RQPtr > RQPtrVec;
        typedef std::uint8_t Priority;
        
        //! @brief RenderQueues stored in this manager. 
        std::map < Priority, RQPtrVec, std::greater < Priority > > queues;
        
        //! @brief Mutex to protect queues. 
        mutable std::mutex queuesMutex;
        
    public:
        
        /*! @brief Constructs a RenderQueueManager. */
        RenderQueueManager() = default;
        
        /*! @brief Destructs the manager. 
         *
         * When destroying this manager, all RenderQueue's shared_ptr are destroyed and RenderQueue::release()
         * is called to ensure the RenderQueue is released, even if another shared_ptr instance remains active. 
         * This is done because a Driver which destroys this manager wants it to release its objects. 
         *
        **/
        ~RenderQueueManager();
        
        /*! @brief Adds a queue. 
         *
         * \param[in] priority Queue priority for the queue. Must be between kRenderQueuePriorityLowest and 
         *      kRenderQueuePriorityHighest. To ensure this, value is automatically clamped in this range. 
         * \param[in] queue Queue we want to add to this manager. If null, manager will not add the queue. 
         *
        **/
        void add(std::uint8_t priority, std::shared_ptr < RenderQueue > const& queue);
        
        /*! @brief Removes the given queue if found. */ 
        void remove(std::shared_ptr < RenderQueue > const& queue);
        
        /*! @brief Releases all queues in the given priority. */
        void clearPriority(std::uint8_t priority);
        
        /*! @brief Releases all queues. */
        void clear();
        
        /*! @brief Finds the queue with given handle, or return null. */
        std::shared_ptr < RenderQueue > findByHandle(std::uint16_t handle);
        
        /*! @brief Convenient function to call a callback for each managed object and locking the
         * managed list. */
        template < typename Callable >
        void forEach(Callable cbk)
        {
            std::lock_guard < std::mutex > lck(queuesMutex);
            
            for (auto& pair : queues) {
                for (auto& queue : pair.second) {
                    cbk(queue);
                }
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
            queuesMutex.lock();
            auto listCopy = queues;
            queuesMutex.unlock();
            
            for (auto& pair : listCopy) {
                for (auto& queue : pair.second) {
                    cbk(queue);
                }
            }
        }
    };
}

#endif // CLEAN_RENDERQUEUEMANAGER_H
