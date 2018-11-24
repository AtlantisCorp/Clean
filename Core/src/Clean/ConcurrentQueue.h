/** @file Core/ConcurrentQueue.h
 *  @date 11/24/2018
**/

#ifndef CLEAN_CONCURRENTQUEUE_H
#define CLEAN_CONCURRENTQUEUE_H

#include <condition_variable>
#include <queue>
#include <mutex>

namespace Clean 
{
    /** @brief Defines a simple MPMC Queue. 
     * 
     * Based on the work from 
     * https://www.justsoftwaresolutions.co.uk/threading/implementing-a-thread-safe-queue-using-condition-variables.html
     * https://github.com/juanchopanza/cppblog/blob/master/Concurrency/Queue/Queue.h
     *
     * While testing the NotificationCenter, it appears std::queue is not designed for Multiple Producer Multiple 
     * Consumer race conditions. For this purpose, a special ConcurrentQueue is created to handle this case. This 
     * implementation is not lock-free. 
     *
    **/
    template < typename Data >
    class ConcurrentQueue 
    {
        //! @brief Our base queue.
        std::queue < Data > queue;
        
        //! @brief Mutex protecting the queue.
        std::mutex mutex;
        
        //! @brief Condition variable to wait for data.
        std::condition_variable condition;
        
    public:
        
        /*! @brief Adds some data to the queue. */
        void push(Data const& data) 
        {
            std::unique_lock < std::mutex > lock(mutex);
            queue.push(data);
            lock.unlock();
            condition.notify_one();
        }
        
        /*! @brief Waits for an available front value and returns it. */
        Data pop() 
        {
            std::unique_lock < std::mutex > lock(mutex);
            while (queue.empty()) condition.wait(lock);
            
            auto value = queue.front();
            queue.pop();
            
            return value;
        }
        
        /*! @brief Waits for an available front value and returns it to the given reference. */
        void pop(Data& value)
        {
            std::unique_lock < std::mutex > lock(mutex);
            while (queue.empty()) condition.wait(lock);
            
            value = queue.front();
            queue.pop();
        }
    };
}

#endif // CLEAN_CONCURRENTQUEUE_H