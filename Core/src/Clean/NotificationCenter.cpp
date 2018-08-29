/** \file Core/NotificationCenter.cpp
**/

#include "NotificationCenter.h"

namespace Clean 
{
    NotificationCenter::NotificationCenter(std::uint8_t m) : mode(m)
    {
        if (mode)
        {
            exitLoopThread.store(false);
            
            loopThread = std::thread([this](){
                
                while (!exitLoopThread)
                {
                    cachedNotifMutex.lock();
                    bool empty = cachedNotifications.empty();
                    
                    if (empty)
                    {
                        cachedNotifMutex.unlock();
                        std::unique_lock < std::mutex > lock(condLoopThreadMutex);
                        condLoopThread.wait(lock);
                        cachedNotifMutex.lock();
                    }
                    
                    auto notif = cachedNotifications.front();
                    cachedNotifications.pop();
                    cachedNotifMutex.unlock();
                    
                    std::lock_guard < std::mutex > lock(listenersMutex);
                    
                    for (auto& listener : listeners)
                    {
                        assert(listener && "Null listener stored.");
                        listener->process(notif);
                    }
                }
                
            });
        }
    }
    
    NotificationCenter::~NotificationCenter()
    {
        if (mode)
        {
            exitLoopThread.store(true);
            
            {
                std::unique_lock < std::mutex > lock(condLoopThreadMutex);
                condLoopThread.signal_all();
            }
            
            loopThread.join();
        }
    }
    
    void NotificationCenter::send(Notification const& notif)
    {
        if (mode)
        {
            {
                std::lock_guard < std::mutex > lck(cachedNotifMutex);
                cachedNotifMutex.push(notif);
            }
            
            {
                std::unique_lock < std::mutex > lck(condLoopThreadMutex);
                condLoopThread.signal_all();
            }
        }
        
        else 
        {
            std::lock_guard < std::mutex > lock(listenersMutex);
            
            for (auto& listener : listeners)
            {
                assert(listener && "Null listener stored.");
                listener->process(notif);
            }
        }
    }
    
    void NotificationCenter::addListener(std::shared_ptr < NotificationListener > const& listener)
    {
        std::lock_guard < std::mutex > lock(listenersMutex);
        listeners.push_back(listener);
    }
}