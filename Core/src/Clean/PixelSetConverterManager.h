/** =======================================================
 *  \file Core/PixelSetConverterManager.h
 *  \date 11/04/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_PIXELSETCONVERTERMANAGER_H
#define CLEAN_PIXELSETCONVERTERMANAGER_H

#include "PixelSetConverter.h"
#include "Manager.h"
#include "Singleton.h"

namespace Clean 
{
    class PixelSetConverterManager : public Singleton < PixelSetConverterManager >, public Manager < PixelSetConverter >
    {
    public:
        /*! @brief Constructs the manager and registers all included converters. */
        PixelSetConverterManager();
        
        /*! @brief Finds a PixelSetConverter that converts from src to dest pixel format. */
        std::shared_ptr < PixelSetConverter > findConverter(std::uint8_t src, std::uint8_t dest) const;
    };
}

#endif // CLEAN_PIXELSETCONVERTERMANAGER_H