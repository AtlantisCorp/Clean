/** 
 *  \file StbiLoader/STBILoader.cpp
 *  \date 02/11/2018
**/

#include "STBILoader.h"
#include "stb_image.h"

#include <Clean/Core.h>
#include <Clean/Allocate.h>
#include <Clean/NotificationCenter.h>
#include <Clean/Platform.h>
#include <Clean/Image.h>
using namespace Clean;

std::shared_ptr < Image > STBILoader::load(std::string const& filepath) const 
{
    std::string realPath = FileSystem::Current().findRealPath(filepath);
    
    if (realPath.empty())
    {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "File %s not found.", filepath.data()));
        return nullptr;
    }
    
    // NOTES: For now, we use stbi_load() to load RGB data. No Alpha is permitted. If we want to support
    // RGBA format, we have to add an option to the loader for the user to specify if he wants alpha
    // or not. I am thinking about a custom option or defining this directly in the loading function.
    
    stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load(realPath.data(), &width, &height, &nrChannels, STBI_rgb_alpha);
    assert(data && "Invalid image loading.");
    
    auto pixels = AllocateShared < PixelSet >();
    pixels->lineWidth = width * 4 * sizeof(unsigned char);
    pixels->format = kPixelFormatRGBA8;
    pixels->data = data;
    
    auto image = AllocateShared < Image >(pixels, SizePair{ 0, 0 }, SizePair{ static_cast<size_t>(width), static_cast<size_t>(height) });
    assert(image && "Null Allocation occured.");
    
    return image;
}

bool STBILoader::isLoadable(std::string const& ext) const
{
    return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif";
}
