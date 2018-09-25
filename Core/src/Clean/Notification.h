/** \file Core/Notification.h
**/

#ifndef CLEAN_NOTIFICATION_H
#define CLEAN_NOTIFICATION_H

#include <cstdint>
#include <string>

namespace Clean 
{
    /*! @brief An object emitted by a NotificationCenter to all its listeners. */
    struct Notification 
    {
        //! @brief Level used for this notification.
        std::uint8_t level;
        
        //! @brief Function from where this notification is sent from.
        std::string function;
        
        //! @brief File from where this notification is sent from. 
        std::string file;
        
        //! @brief Message for this notification. 
        std::string message;
    };
    
    //! @brief Information level, or the lowest level of notification. 
    static constexpr const std::uint8_t kNotificationLevelInfo = 0;
    
    //! @brief Warning level, when something is not as it should be but is not a critical error. 
    static constexpr const std::uint8_t kNotificationLevelWarning = 1;
    
    //! @brief Error level, when something failed. 
    static constexpr const std::uint8_t kNotificationLevelError = 2;
    
    //! @brief Fatal error level, when something failed and the program cannot continue. 
    static constexpr const std::uint8_t kNotificationLevelFatal = 3;
    
    /*! @brief Builds a notification. 
     *
     * \param level Level for the notification. 
     * \param format Message formatted as a valid vsnprintf format message. 
     * \param ... Args for the vsnprintf function. 
     *
     * \note
     * Notification::function is computed with '__FUNCTION__' and Notification::file is computed
     * from '__FILE__' macros. This function is thus a macro to validate function and file from where
     * it is called. 
     *
    **/
#   define BuildNotification(level, format, ...) BuildNotificationAll(level, __FUNCTION__, __FILE__, format, __VA_ARGS__)
    
    /*! @brief Builds a notification. 
     *
     * \param level Level for the notification. 
     * \param function Function that should be displayed by the notification.
     * \param file File from where this function is called.
     * \param format Message formatted as a valid vsnprintf format message.
     * \param ... Args for the vsnprintf function. 
     * 
     * \return A valid Notification object. 
    **/
    Notification BuildNotificationAll(std::uint8_t level, const char* function, const char* file, const char* format, ...);
}

#endif // CLEAN_NOTIFICATION_H
