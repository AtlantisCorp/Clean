/** =======================================================
 *  \file Core/ImageManager.h
 *  \date 10/27/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_IMAGEMANAGER_H
#define CLEAN_IMAGEMANAGER_H

#include "Manager.h"
#include "Image.h"
#include "Singleton.h"

namespace Clean 
{
    class ImageManager : public Manager < Image >, public Singleton < ImageManager >
    {
    public:
        
        /*! @brief Loads an image from a file. */
        std::shared_ptr < Image > load(std::string const& filepath);
        
        /*! @brief Finds an image loaded for the given file, or returns null. */
        std::shared_ptr < Image > findFile(std::string const& filepath);
    };
}

#endif // CLEAN_IMAGEMANAGER_H