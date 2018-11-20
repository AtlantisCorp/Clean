/** =======================================================
 *  \file Core/PixelSetConverter.h
 *  \date 11/04/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_PIXELSETCONVERTER_H
#define CLEAN_PIXELSETCONVERTER_H

#include "PixelSet.h"
#include <memory>

namespace Clean 
{
    /*! @brief Interface to convert a PixelFormat to another. */
    struct PixelSetConverter 
    {
        /*! @brief Default destructor. */
        virtual ~PixelSetConverter() = default;
        
        /*! @brief Returns the source pixel format. */
        virtual std::uint8_t srcFormat() const = 0;
        
        /*! @brief Returns the destination pixel format. */
        virtual std::uint8_t destFormat() const = 0;
        
        /*! @brief Creates a new PixelSet where data is converted from srcFormat to destFormat. */
        virtual std::shared_ptr < PixelSet > convert(std::shared_ptr < PixelSet > const& srcPixels) const = 0;
    };
}

#endif // CLEAN_PIXELSETCONVERTER_H