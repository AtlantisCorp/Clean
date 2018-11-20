/** =======================================================
 *  \file Core/PixelSet.h
 *  \date 10/30/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_PIXELSET_H
#define CLEAN_PIXELSET_H

#include "PixelFormat.h"

namespace Clean 
{
    /*! @brief Holds a set of raw pixels. */
    struct PixelSet 
    {
        //! @brief Number of bytes to describe a ROW (full image's line).
        //! A row must be padded to a number of pixels. For example, you can insert one or more blank
        //! pixels but not only one padding byte. 
        std::size_t lineWidth = 0;
        
        //! @brief Number of columns. 
        std::size_t columnsCount = 0;
        
        //! @brief PixelFormat used. 
        std::uint8_t format = kPixelFormatNull;
        
        //! @brief Pixels used.
        unsigned char* data = nullptr;
    };
}

#endif // CLEAN_PIXELSET_H
