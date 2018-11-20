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
    std::string realPath;
    std::fstream stream = FileSystem::Current().open(filepath, std::ios::in, &realPath);
    
    if (!stream) {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "File %s not found.", filepath.data()));
        return nullptr;
    }
    
    std::string content; Platform::StreamGetContent(stream, content);
    stream.close();
    
    if (!content.size()) {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "File %s is empty.", filepath.data()));
        return nullptr;
    }
    
    int width, height, nrChannels;
    unsigned char* data = stbi_load_from_memory((const unsigned char*)content.c_str(), content.size(), &width, &height, &nrChannels, 4);
    
    if (!data || !width || !height || !nrChannels) {
        NotificationCenter::GetDefault()->send(BuildNotification(kNotificationLevelError, "File %s is corrupted.", filepath.data()));
        return nullptr;
    }
    
    auto pixels = AllocateShared < PixelSet >();
    pixels->lineWidth = width * nrChannels * sizeof(unsigned char);
    pixels->format = (nrChannels == 3) ? kPixelFormatRGB8 : (nrChannels == 4 ? kPixelFormatRGBA8 : kPixelFormatNull);
    pixels->data = data;
    
    auto image = AllocateShared < Image >(pixels, SizePair{ 0, 0 }, SizePair{ static_cast<size_t>(width), static_cast<size_t>(height) });
    assert(image && "Null Allocation occured.");
    
    return image;
}

bool STBILoader::isLoadable(std::string const& ext) const
{
    return ext == "png" || ext == "jpg" || ext == "jpeg" || ext == "gif";
}
