/** \file Core/Traits.h
**/

#ifndef CLEAN_TRAITS_H
#define CLEAN_TRAITS_H

#include <type_traits>

namespace Clean 
{
    //! @brief Enables std::enable_if_t on all platforms. 
    template < bool B, class T = void >
    using EnableIf = typename std::enable_if < B, T >::type;

    //! @brief Enables std::is_base_of_v on all platforms.
    template < class Base, class Derived >
    inline constexpr bool IsBase = std::is_base_of < Base, Derived >::value;
    
    /** @brief Helper to call a particular member function in a class. */
    template < class Listener >
    class Caller 
    {
    public:
    
        template < typename Callback, class... Args >
        void call(Callback callback, Listener* listener, Args&&... args)
        {
            std::call(callback, listener, std::forward<Args>(args)...);
        }
    
        template < typename Callback, class... Args >
        void operator()(Callback callback, Listener* listener, Args&&... args)
        {
            call(callback, listener, std::forward<Args>(args)...);
        }
    };
}

#endif // CLEAN_TRAITS_H