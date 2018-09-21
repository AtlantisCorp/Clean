/** \file Core/NotificationCenter.h
**/

#ifndef CLEAN_NOTIFICATIONCENTER_H
#define CLEAN_NOTIFICATIONCENTER_H

#include "NotificationListener.h"
#include "Notification.h"

#include <cstdint>
#include <memory>
#include <list>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <queue>

namespace Clean
{
    class Core;

    //! @brief Synchroneous mode for NotificationCenter.
    static constexpr const std::uint8_t kNotificationCenterModeSynchroneous = 0;

    //! @brief Asynchroneous mode for NotificationCenter.
    static constexpr const std::uint8_t kNotificationCenterModeAsynchroneous = 1;

    /*! @brief Manages NotificationListener and groups notifications.
     *
     * NotificationCenter is a rally point for all notifications in the engine. A
     * notification is, basically, a message from any of the Clean environnment that
     * can have more or less importance.
     *
     * NotificationListeners are objects that process each notification. The notification
     * center can act in mono or multithread mode. When using multithreaded mode, it enters
     * an infinite loop to send notifications to its listeners.
     *
     * To send a notification, you can use NotificationCenter::send() with the appropriate
     * Notification object. A Notification can be built with BuildNotification() defined in
     * \ref Notification.h.
     *
    **/
    class NotificationCenter final
    {
        //! @brief Listeners associated to this center.
        std::list < std::shared_ptr < NotificationListener > > listeners;

        //! @brief Mode actually used to send notifications. 1 is asynchroneous (multithreaded),
        //! 0 is synchroneous (monothreaded). While asynchroneous is faster on multiple CPUs,
        //! synchronized mode delivers notifications immediately.
        std::uint8_t mode;

        //! @brief When multithreaded, holds the emitting thread loop.
        std::thread loopThread;

        //! @brief When multithreaded, holds a condition to awake the emitting loop.
        std::condition_variable condLoopThread;

        //! @brief Mutex to use with condLoopThread.
        std::mutex condLoopThreadMutex;

        //! @brief Mutex to protect listeners.
        std::mutex listenersMutex;

        //! @brief Flag to exit the loop thread.
        std::atomic_bool exitLoopThread;

        //! @brief In multithreaded mode, stores notifications to send.
        std::queue < Notification > cachedNotifications;

        //! @brief In multithreaded mode, protects cachedNotifications.
        std::mutex cachedNotifMutex;

        //! @brief Default NotificationCenter registered by Core.
        static std::shared_ptr < NotificationCenter > defaultCenter;

        //! Core is a friend to modify defaultCenter.
        friend class Core;

    public:

        /*! @brief Constructs the center. If mode is multithreaded, actually launches the
         * loop thread.
        **/
        NotificationCenter(std::uint8_t m = kNotificationCenterModeAsynchroneous);

        /*! @brief Destruct the center.
         * If in multithreaded mode, wait for the loop thread to exit.
        **/
        ~NotificationCenter();

        /*! @brief Sends given notification through all listeners. */
        void send(Notification const& notif);

        /*! @brief Adds a listener to notifications. */
        void addListener(std::shared_ptr < NotificationListener > const& listener);

        /*! @brief Returns defaultCenter. */
        static std::shared_ptr < NotificationCenter > GetDefault();
    };
}

#endif // CLEAN_NOTIFICATIONCENTER_H
