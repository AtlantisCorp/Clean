/** \file Core/Transaction.cpp
**/

#include "Transaction.h"
#include "Allocate.h"

namespace Clean 
{
    Transaction::Transaction() 
        : type_(0), data_(nullptr), tpoint_(Clock::now())
    {
        
    }
    
    Transaction::Transaction(std::uint8_t t, void* d, Clock::time_point const& tp)
        : type_(t), data_(d), tpoint_(tp)
    {
        
    }
    
    Transaction::~Transaction() 
    {
        if (data_)
            Free((std::uint8_t*)data_);
    }
    
    std::uint8_t Transaction::type() const
    {
        return type_;
    }
    
    void* Transaction::data() 
    {
        return data_;
    }
    
    bool Transaction::valid() const 
    {
        return Clock::now() < tpoint_;
    }
}
