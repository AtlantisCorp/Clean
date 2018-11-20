/** 
 *  \file StbiLoader/STBILoader.h
 *  \date 02/11/2018
**/

#ifndef STBILOADER_STBILOADER_H
#define STBILOADER_STBILOADER_H

#include <Clean/Image.h>

/** @brief Loads any image file supported by STB Image. */
class STBILoader : public Clean::FileLoader < Clean::Image >
{
public:
    /*! @brief Loads one image from a file. */
    std::shared_ptr < Clean::Image > load(std::string const& filepath) const;
    
    /*! @brief Must return true if the given extension is loadable by this loader. */
    bool isLoadable(std::string const& extension) const;
};

#endif // STBILOADER_STBILOADER_H
