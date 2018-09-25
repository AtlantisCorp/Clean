/** \file Core/RenderQueueManager.cpp
**/

#include "RenderQueueManager.h"
#include <algorithm>

namespace Clean 
{
    RenderQueueManager::~RenderQueueManager() 
    {
        // NOTES: Destructors should not be thread-safe. A destructor should be called only when the object
        // is not in use. In this case, RenderQueueManager should be owned only by Driver, which will destroy
        // this object in the main thread. Thus, no need for thread-safety. 
        
        for (auto& pair : queues) 
        {
            for (auto& queue : pair.second)
            {
                assert(queue && "Null RenderQueue stored.");
                queue->release();
            }
        }
    }
    
    void RenderQueueManager::add(std::uint8_t priority, std::shared_ptr < RenderQueue > const& queue)
    {
        priority = std::clamp(priority, kRenderQueuePriorityLowest, kRenderQueuePriorityHighest);
        if (!queue) return;
        
        std::scoped_lock < std::mutex > lck(queuesMutex);
        queues[priority].push_back(queue);
    }
    
    void RenderQueueManager::remove(std::shared_ptr < RenderQueue > const& queue)
    {
        std::scoped_lock < std::mutex > lck(queuesMutex);
        
        for (auto& pair : queues)
        {
            auto it = std::find(pair.second.begin(), pair.second.end(), queue);
            
            if (it != pair.second.end())
            {
                pair.second.erase(it);
                return;
            }
        }
    }
    
    void RenderQueueManager::clearPriority(std::uint8_t priority)
    {
        std::scoped_lock < std::mutex > lck(queuesMutex);
        auto it = queues.find(priority);
        if (it == queues.end()) return;
        
        auto& vector = it->second;
        
        for (auto& queue : vector) {
            assert(queue && "Null RenderQueue stored.");
            queue->release();
        }
        
        vector.clear();
    }
    
    void RenderQueueManager::clear()
    {
        std::scoped_lock < std::mutex > lck(queuesMutex);
        
        for (auto& pair : queues) 
        {
            for (auto& queue : pair.second)
            {
                assert(queue && "Null RenderQueue stored.");
                queue->release();
            }
        }
        
        queues.clear();
    }
    
    std::shared_ptr < RenderQueue > RenderQueueManager::findByHandle(std::uint16_t handle)
    {
        std::scoped_lock < std::mutex > lck(queuesMutex);
        
        for (auto& pair : queues) 
        {
            for (auto& queue : pair.second)
            {
                assert(queue && "Null RenderQueue stored.");
                if (queue->getHandle() == handle) return queue;
            }
        }
        
        return nullptr;
    }
}
