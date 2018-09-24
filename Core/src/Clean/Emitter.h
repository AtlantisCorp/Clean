/** \file Core/Emitter.h
**/

#ifndef CLEAN_EMITTER_H
#define CLEAN_EMITTER_H

#include "Traits.h"

#include <list>
#include <shared_mutex>
#include <mutex>
#include <queue>
#include <thread>

namespace Clean 
{
    //! @defgroup EventEmittingPolicy Event Emitting Policy Constants
    //! @{
    
    static constexpr const std::uint8_t kEventEmittingPolicyAsync = 1;
    static constexpr const std::uint8_t kEventEmittingPolicySync = 2;
    
    //! @}

    /** @brief Generic description of an emitter. 
     *
     * An emitter can sends any event to its dedicated listener. Basically, for each 
     * pair Listener/Callback an event is sent. An event doesn't require the listener
     * to answer anything. Thus, an emitter stores its listeners but only in a weak_ptr
     * (the emitter doesn't own its listeners). 
     *
     * Monothreaded mode: When eventEmittingPolicy is kEventEmittingPolicyAsync, the emitter
     * sends its event to listeners in the same thread as the caller. 
     *
     * Multithreaded mode: When eventEmittingPolicy is kEventEmittingPolicySync, the emitter
     * sends its event to listeners in a different thread, enabling asynchroneous event handling. 
     * In Multithreaded mode, threads generated are never detached to ensure a correct termination. 
     * Protected method flushSendThreads joins all threads within a limit defined by sendThreadsLimit.
     * This limit is the maximum number of threads the emitter can handle simultaneously. If more than 
     * this limit number of threads are launched, emitter will join all of them before finishing its
     * work. 
    **/
    template < class Listener >
    class Emitter 
    {
        //! @brief Helper to call a listener's callback.
        Caller < Listener > caller;
    
        //! @brief Listeners registered in this emitter. 
        std::list < std::weak_ptr < Listener > > listeners;
    
        //! @brief Protects listeners. A shared mutex lets multiple threads send an event
        //! by using this emitter. This may be rare but it also let event sending faster. 
        std::shared_mutex listenersMutex;
    
        //! @brief Queue of threads sent by this emitter.
        std::queue < std::thread > sendThreads;
    
        //! @brief Protects the thread's queue.
        std::mutex sendThreadsMutex;
    
        //! @brief Policy to event emition.
        std::atomic < std::uint8_t > eventEmittingPolicy;
    
        //! @brief Limits for the number of threads that can be launched simultaneously.
        std::atomic < std::size_t > sendThreadsLimit;
    
    public:
    
        /*! @brief Default constructor. */
        Emitter() : eventEmittingPolicy(kEventEmittingPolicyAsync), sendThreadsLimit(20)
        {
        
        }
    
        /*! @brief Registers a new listener for this emitter. */
        void addListener(std::shared_ptr < Listener > const& listener)
        {
            std::scoped_lock < std::mutex > lck(listenersMutex);
            listeners.push_back(listener);
        }
    
        /*! @brief Registers a new listener if its type is derived from this emitter's listener type. */
        template < class DerivedListener, EnableIf < IsBase < Listener, DerivedListener > >* = nullptr >
        void addListener(std::shared_ptr < DerivedListener > const& listener)
        {
            addListener(std::static_pointer_cast < Listener >(listener));
        } 
    
        /*! @brief Removes a listener from this emitter. */
        void removeListener(std::shared_ptr < Listener > const& listener)
        {
            std::scoped_lock < std::mutex > lck(listenersMutex);
            auto it = std::find_if(listeners.begin(), listeners.end(), [&listener](std::weak_ptr < Listener > const& wl) {
                return wl.lock() == listener;
            });
            listeners.erase(it);
        }
    
        /*! @brief Removes all listeners. */
        void resetListeners() 
        {
            std::scoped_lock lck(listenersMutex);
            listeners.clear();
        }
    
        /*! @brief Sends an event to all listeners to this emitter. */
        template < typename Callback, class EventT >
        void send(Callback callback, EventT const& event)
        {   
            if (eventEmittingPolicy.load() == kEventEmittingPolicyAsync)
            {   
                listenersMutex.lock_shared();
                
                auto listenersCopy = listeners;
                auto callerCopy = caller;
            
                std::thread sendThread = std::thread([callback, event, listenersCopy, callerCopy](){
                
                    for (auto listener : listenersCopy)
                    {
                        auto slistener = listener.lock();
                        if (!slistener) continue;
                    
                        callerCopy(callback, slistener.get(), event);
                    }
                
                });
            
                listenersMutex.unlock_shared();
            
                std::scoped_lock lck2(sendThreadsMutex);
                sendThreads.push(std::move(sendThread));
                flushSendThreads();
            }
        
            else if (eventEmittingPolicy.load() == kEventEmittingPolicySync)
            {
                std::scoped_lock lck(listenersMutex);
                for (auto listener : listeners)
                {
                    auto slistener = listener.lock();
                    if (!slistener) continue;
                
                    caller(callback, slistener.get(), event);
                }
            }
        }
    
        /*! @brief Changes eventEmittingPolicy. */
        void setEventEmittingPolicy(std::uint8_t value)
        {
            eventEmittingPolicy.store(value);
        }
    
        /*! @brief Returns eventEmittingPolicy. */
        std::uint8_t getEventEmittingPolicy() const 
        {
            return eventEmittingPolicy.load();
        }
    
        /*! @brief Changes sendThreadsLimit. */
        void setSendThreadsLimit(std::size_t limit)
        {
            sendThreadsLimit.store(limit);
        }
    
        /*! @brief Returns sendThreadsLimit. */
        std::size_t getSendThreadsLimit() const 
        {
            return sendThreadsLimit.load();
        }
    
        /*! @brief Returns number of listeners. */
        std::size_t getListenersCount() const 
        {
            std::scoped_lock lck(listenersMutex);
            return listeners.size();
        }
    
    protected:
    
        /*! @brief Joins threads in the thread's queue if its limits has been reached. */
        void flushSendThreads()
        {
            if (sendThreads.size() > sendThreadsLimit.load())
            { 
                bool quit = false;
                std::size_t currentFinished = 0;
                std::size_t limit = sendThreadsLimit.load();
            
                do
                {            
                    std::thread lastThread = std::move(sendThreads.front());
                    sendThreads.pop();
                
                    try
                    {
                        if (lastThread.joinable())
                            lastThread.join();
                    
                        currentFinished++;
                    }
                
                    catch(std::system_error const& e)
                    {
                        quit = true;
                    }
                
                }
                while (!quit && currentFinished < limit && !sendThreads.empty());
            }
        }
    };
}

#endif // CLEAN_EMITTER_H
