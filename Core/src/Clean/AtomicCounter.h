/** =======================================================
 *  \file Core/AtomicCounter.h
 *  \date 11/06/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_ATOMICCOUNTER_H
#define CLEAN_ATOMICCOUNTER_H

#include <atomic>

namespace Clean 
{
    /** @brief Defines a generic atomic counter. */
    template < typename T > 
    class AtomicCounter 
    {
        std::atomic < T > currentValue;
        
    public:
        
        /*! @brief Constructs a new counter with a start value. */
        AtomicCounter(T const& start) : currentValue(start) {}
        
        /*! @brief Returns the current value and adds one to this value. */
        T next() { return currentValue.fetch_add((T)1); }
        
        /*! @brief Undo the 'next' operation, i.e. decrements the counter by one. */
        void undo() { currentValue.fetch_sub(1); }
        
        /*! @brief Reset the counter with a given value. */
        void reset(T const& start) { currentValue.store(start); }
    };
}

#endif // CLEAN_ATOMICCOUNTER_H