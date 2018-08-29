/** \file Core/Exception.cpp
**/

#include "Exception.h"

namespace Clean 
{
    Exception::Exception(std::string const& msg) 
        : message(msg)
    {
        
    }
    
    const char* Exception::what() const noexcept
    {
        return message.data();
    }
}