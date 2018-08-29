/** \file Core/NotificationListener.h
**/

#ifndef CLEAN_NOTIFICATIONLISTENER_H
#define CLEAN_NOTIFICATIONLISTENER_H

#include "Notification.h"

namespace Clean 
{
    /** @brief Basic interface to listen to notifications. */
    class NotificationListener 
    {
    public:
        
        /*! @brief Default constructor. */
        NotificationListener() = default;
        
        /*! @brief Default destructor. */
        virtual ~NotificationListener() = default;
        
        /*! @brief Processes a notification. */
        virtual void process(Notification const&) = 0;
    };
}

#endif // CLEAN_NOTIFICATIONLISTENER_H