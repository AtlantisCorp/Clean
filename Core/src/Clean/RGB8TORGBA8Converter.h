/** =======================================================
 *  \file Core/RGB8TORGBA8Converter.h
 *  \date 11/04/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_RGB8TORGBA8CONVERTER_H
#define CLEAN_RGB8TORGBA8CONVERTER_H

#include "PixelSetConverter.h"

namespace Clean 
{
    /*! @brief Converts RGB8 to RGBA8 pixel set. */
    struct RGB8TORGBA8Converter : public PixelSetConverter
    {
        /*! @brief Returns the source pixel format. */
        std::uint8_t srcFormat() const;
        
        /*! @brief Returns the destination pixel format. */
        std::uint8_t destFormat() const;
        
        /*! @brief Creates a new PixelSet where data is converted from srcFormat to destFormat. */
        std::shared_ptr < PixelSet > convert(std::shared_ptr < PixelSet > const& srcPixels) const;
    };
}

#endif // CLEAN_RGB8TORGBA8CONVERTER_H