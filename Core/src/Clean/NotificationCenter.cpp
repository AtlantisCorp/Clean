/** \file Core/NotificationCenter.cpp
**/

#include "NotificationCenter.h"

namespace Clean
{
    std::shared_ptr < NotificationCenter > NotificationCenter::defaultCenter = nullptr;

    NotificationCenter::NotificationCenter(std::uint8_t m) : mode(m)
    {
        if (mode == kNotificationCenterModeAsynchroneous)
        {
            exitLoopThread.store(false);

            loopThread = std::thread([this](){

                while (!exitLoopThread)
                {
                    Notification notif;
                    cachedNotifications.pop(notif);

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
        terminate();
    }

    void NotificationCenter::send(Notification const& notif)
    {
        if (mode)
        {
            cachedNotifications.push(notif);
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

    std::shared_ptr < NotificationCenter > NotificationCenter::GetDefault()
    {
        return std::atomic_load(&defaultCenter);
    }
    
    void NotificationCenter::terminate()
    {
        if (mode && loopThread.joinable())
        {
            exitLoopThread.store(true);
            cachedNotifications.push(Notification());
            
            loopThread.join();
        }
    }
}
