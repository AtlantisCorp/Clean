/** \file Core/Notification.cpp
**/

#include "Notification.h"

#include <cstdarg>
#include <cassert>
#include <cstring>

namespace Clean 
{
    Notification BuildNotificationAll(std::uint8_t level, const char* function, const char* file, const char* format, ...)
    {
        assert(function && file && format && "Null string given to Clean::BuildNotificationAll.");

        char buffer[1024];
        memset(buffer, 0, 1024);
        
        va_list ap;
        va_start(ap, format);
        vsnprintf(buffer, 1024, format, ap);
        va_end(ap);
        
        Notification result;
        result.level = level;
        result.function = function;
        result.file = file;
        result.message = buffer;
        return result;
    }
}