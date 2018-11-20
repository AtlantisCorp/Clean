/** =======================================================
 *  \file Core/PixelFormat.cpp
 *  \date 10/30/2018
 *  \author luk2010
    ======================================================= **/

#include "PixelFormat.h"

namespace Clean 
{
    std::size_t PixelFormatGetSize(std::uint8_t format)
    {
        switch(format)
        {
            case kPixelFormatRGB8:
            return sizeof(std::uint8_t)*3;
            
            case kPixelFormatRGBA8:
            return sizeof(std::uint8_t)*4;
            
            default:
            return 0;
        }
    }
    
    std::string PixelFormatToString(std::uint8_t format)
    {
        switch (format)
        {
            case kPixelFormatRGB8: return "RGB8";
            case kPixelFormatRGBA8: return "RGBA8";
            
            default:
            return std::string();
        }
    }
}