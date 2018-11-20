/** =======================================================
 *  \file Core/Image.cpp
 *  \date 10/29/2018
 *  \author luk2010
    ======================================================= **/

#include "Image.h"

namespace Clean 
{
    Image::Image(Image const& rhs, SizePair const& org, SizePair const& sz)
    {
        pixels = std::atomic_load(&rhs.pixels);
        origin.store(org);
        size.store(sz);
    }
    
    Image::Image(std::shared_ptr < PixelSet > const& rhs, SizePair const& org, SizePair const& sz)
    {
        pixels = std::atomic_load(&rhs);
        origin.store(org);
        size.store(sz);
    }
    
    const unsigned char* Image::raw() const 
    {
        auto loadedPixels = std::atomic_load(&pixels);
    
        if (loadedPixels)
        {
            const SizePair loadedOrigin = origin.load();
            const std::size_t formatSize = PixelFormatGetSize(loadedPixels->format);
            const std::size_t lineSize = formatSize * loadedPixels->lineWidth;
            
            return loadedPixels->data + lineSize * loadedOrigin.y + formatSize * loadedOrigin.x;
        }
        
        return nullptr;
    }
    
    const std::size_t Image::findRowLength() const 
    {
        auto loadedPixels = std::atomic_load(&pixels);
        
        if (loadedPixels)
        {
            const std::size_t formatSize = PixelFormatGetSize(loadedPixels->format);
            return loadedPixels->lineWidth / formatSize;
        }
        
        return 0;
    }
    
    const std::uint8_t Image::pixelFormat() const
    {
        auto loadedPixels = std::atomic_load(&pixels);
        return loadedPixels ? loadedPixels->format : kPixelFormatNull;
    }
    
    SizePair const Image::getOrigin() const 
    {
        return origin.load();
    }
    
    SizePair const Image::getSize() const 
    {
        return size.load();
    }
    
    void Image::setOrigin(SizePair const& org) 
    {
        origin.store(org);
    }
    
    void Image::setSize(SizePair const& sz)
    {
        size.store(sz);
    }
    
    void Image::setPixelSet(std::shared_ptr < PixelSet > const& rhs) 
    {
        std::atomic_store(&pixels, rhs);
    }
    
    std::shared_ptr < PixelSet > Image::getPixelSet() const
    {
        return std::atomic_load(&pixels);
    }
    
    std::string Image::getFile() const
    {
        return file.load();
    }
}
