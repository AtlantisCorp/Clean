/** \file Core/Exception.h
**/

#ifndef CLEAN_EXCEPTION_H
#define CLEAN_EXCEPTION_H

#include <exception>
#include <string>

namespace Clean 
{
    /*! @brief Base class for a Clean exception. */
    class Exception : public std::exception 
    {
        //! @brief Message returned by what. 
        std::string message;
        
    public:
        
        /*! @brief Construct an exception. */
        Exception(std::string const& msg);
        
        /*! @brief Returns message. */
        const char* what() const noexcept;
    };
    
    /*! @brief Emitted when a pointer required non null is null. */
    class NullPointerException : public Exception 
    {
    public:
        using Exception::Exception;
    };
}

#endif // CLEAN_EXCEPTION_H