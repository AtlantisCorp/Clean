/** \file Core/Handled.h
**/

#ifndef CLEAN_HANDLED_H
#define CLEAN_HANDLED_H

#include <atomic>
#include <cstdint>

namespace Clean
{
    /** @brief Defines a very basic handled class.
     *
     * Any object that may have a unique handle for each class instance can derive from
     * this class and access to the static ::Next() method to generate a new handle. Handle
     * type must be chosen depending on the number of handles which may be generated.
     *
     * \note Once a Handled is initialized with a handle, it cannot be changed at anytime. Derived
     * classes must use getHandle() to have this handle.
     *
    **/
    template < class Requester, class HandleType = std::size_t >
    class Handled
    {
    protected:

#       ifdef __cpp_lib_atomic_is_always_lock_free
        //! @brief Only lock-free handle type should be used.
        static_assert(std::atomic<HandleType>::is_always_lock_free, "HandleType is not lockfree.");
#       endif

        /*! @brief Returns the next unique handle. */
        static HandleType Next() {
            static std::atomic < HandleType > nextHdl = 0;
            HandleType value = nextHdl.fetch_add(1);
            return value;
        }

    private:

        //! @brief Handle for this instance.
        std::atomic < HandleType > handle;

    public:

        /*! @brief Constructs a Handled with a newly generated handle. */
        Handled() : handle(Next()) {}
        /*! @brief Constructs a Handled with a given handle. */
        explicit Handled(HandleType const& handle) : handle(handle) {}

        /*! @brief Returns the object's handle. */
        HandleType getHandle() const { return handle.load(); }
    };
}

#endif // CLEAN_HANDLED_H
