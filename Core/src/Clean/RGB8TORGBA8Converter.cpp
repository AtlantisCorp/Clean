/** =======================================================
 *  \file Core/RGB8TORGBA8Converter.cpp
 *  \date 11/04/2018
 *  \author luk2010
    ======================================================= **/

#include "RGB8TORGBA8Converter.h"
#include "Allocate.h"

namespace Clean 
{
    std::uint8_t RGB8TORGBA8Converter::srcFormat() const 
    { 
        return kPixelFormatRGB8; 
    }
    
    std::uint8_t RGB8TORGBA8Converter::destFormat() const 
    { 
        return kPixelFormatRGBA8; 
    }
    
    std::shared_ptr < PixelSet > RGB8TORGBA8Converter::convert(std::shared_ptr < PixelSet > const& srcPixels) const
    {
        assert(srcPixels && srcPixels->data && "Null PixelSet given.");
        std::size_t pixelsWidthCount = srcPixels->lineWidth / 3;
        std::size_t destPixelsWidthCount = pixelsWidthCount * 4;
        std::size_t destPixelsSize = destPixelsWidthCount * srcPixels->columnsCount;
        unsigned char* buffer = Allocate < unsigned char >(destPixelsSize);
        
        for (std::size_t x = 0; x < pixelsWidthCount; ++x)
        {
            for (std::size_t y = 0; y < srcPixels->columnsCount; ++y)
            {
                std::size_t pixelNum = pixelsWidthCount * y + x;
                std::ptrdiff_t srcOffset = pixelNum * 3;
                std::ptrdiff_t destOffset = pixelNum * 4;
                memcpy(buffer + destOffset, srcPixels->data + srcOffset, sizeof(unsigned char) * 3);
                buffer[destOffset + 1] = 255;
            }
        }
        
        auto destPixels = AllocateShared < PixelSet >();
        destPixels->lineWidth = destPixelsWidthCount;
        destPixels->columnsCount = srcPixels->columnsCount;
        destPixels->format = kPixelFormatRGBA8;
        destPixels->data = buffer;
        
        return destPixels;
    }
}