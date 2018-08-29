/** \file Core/Version.cpp
**/

#include "Version.h"
#include "Platform.h"
#include "Endianness.h"

namespace Clean 
{
    static constexpr const std::size_t kVersionComponents = 4;
    
    Version Version::FromString(std::string const& format)
    {
        std::size_t it = 0;
        
        auto splitted = Platform::Split(format, '.', Platform::kSplitIncludesEmpties);
        assert(splitted.size() == 4 && "Invalid Version format.");
        
        for (auto& comp : splitted)
        {
            data[it] = std::stoi(comp, nullptr, 10);
            it++;
        }
    }
    
    std::string Version::toString() const 
    {
        return std::to_string(major) + "." 
             + std::to_string(minor) + "." 
             + std::to_string(patch) + "." 
             + std::to_string(build);
    }
    
    bool operator == (Version const& lhs, Version const& rhs)
    {
        // Notes: Big endian platforms lets us pack two std::uint32_t in their 
        // natural order (first is high, second is low). However, little endian 
        // platforms makes us store two std::uint32_t in reverse order: first is 
        // low, second is high. Thus, we must convert those numbers to network byte
        // order, which is big endian. 
        
#       ifdef CLEAN_BIG_ENDIAN
        return lhs.high == rhs.high 
            && lhs.low == rhs.low;
        
#       elif defined(CLEAN_LITTLE_ENDIAN)
        const std::uint64_t args[4] = {
            htonll(lhs.high), htonll(rhs.high),
            htonll(lhs.low), htonll(rhs.low)
        };
        
        return args[0] == args[1] 
            && args[2] == args[3];
        
#       endif
    }
    
    bool operator != (Version const& lhs, Version const& rhs)
    {
        return !(lhs == rhs);
    }
    
    bool operator >  (Version const& lhs, Version const& rhs)
    {
        // Notes: Big endian platforms lets us pack two std::uint32_t in their 
        // natural order (first is high, second is low). However, little endian 
        // platforms makes us store two std::uint32_t in reverse order: first is 
        // low, second is high. Thus, we must convert those numbers to network byte
        // order, which is big endian. 
        
#       ifdef CLEAN_BIG_ENDIAN
        if (lhs.high > rhs.high) return true;
        if (lhs.high < rhs.high) return false;
        if (lhs.low > rhs.low) return true;
        return false;
        
#       elif defined(CLEAN_LITTLE_ENDIAN)
        const std::uint64_t args[4] = {
            htonll(lhs.high), htonll(rhs.high),
            htonll(lhs.low), htonll(rhs.low)
        };
        
        if (args[0] > args[1]) return true;
        if (args[0] < args[1]) return false;
        if (args[2] > args[3]) return true;
        return false;
        
#       endif
    }
    
    bool operator >= (Version const& lhs, Version const& rhs)
    {
        return (lhs == rhs) || (lhs > rhs);
    }
    
    bool operator <  (Version const& lhs, Version const& rhs)
    {
        return rhs > lhs;
    }
    
    bool operator <= (Version const& lhs, Version const& rhs)
    {
        return !(lhs > rhs);
    }
}