/** =======================================================
 *  \file Core/PixelSetConverterManager.cpp
 *  \date 11/04/2018
 *  \author luk2010
    ======================================================= **/

#include "PixelSetConverterManager.h"
#include "Allocate.h"

#include "RGB8TORGBA8Converter.h"

namespace Clean 
{
    PixelSetConverterManager::PixelSetConverterManager()
    {
        auto RGB8TORGBA8 = AllocateShared < RGB8TORGBA8Converter >();
        add(RGB8TORGBA8);
    }
    
    std::shared_ptr < PixelSetConverter > PixelSetConverterManager::findConverter(std::uint8_t src, std::uint8_t dest) const 
    {
        std::lock_guard < std::mutex > lck(managedListMutex);
        
        auto it = std::find_if(managedList.begin(), managedList.end(), [src, dest](auto c) { 
            assert(c && "Null PixelSetConverter stored.");
            return c->srcFormat() == src && c->destFormat() == dest; });
            
        if (it != managedList.end()) return *it;
        return nullptr;
    }
}