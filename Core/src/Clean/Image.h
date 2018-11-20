/** =======================================================
 *  \file Core/Image.h
 *  \date 10/27/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_IMAGE_H
#define CLEAN_IMAGE_H

#include "Property.h"
#include "PixelSet.h"
#include "FileLoader.h"
#include "Handled.h"

namespace Clean 
{
    struct SizePair
    {
        std::size_t x;
        std::size_t y;
    };
    
    /** @brief Represents a basic view of a PixelSet. 
     *
     * An Image object can be thought of like a particular View of a PixelSet. As a
     * PixelSet is only a flat representation of multiple pixels of a specific format,
     * the Image is a particular position in this PixelSet. 
     *
     * An Image can represent the full PixelSet (i.e. when loading a file). However, an 
     * Image can be created only for a part of another Image. Both Image will share the 
     * same PixelSet but will have different origin/size. 
     * 
     * When submitting data to the Driver, implementor must take in account that Image may
     * represent only a subview of the PixelSet it holds. Generating a Texture from this 
     * object involves packing the pixels with a ROW corresponding to \ref findRowLength but
     * with an image's width corresponding to size.x. For example, use glPixelStore(GL_PACK_ROW_LENGTH)
     * with the value of \ref findRowLength to pack correctly the pixels in an OpenGL Texure. 
     *
    **/
    class Image : public Handled < Image >
    {
        //! @brief Keeps a copy of the raw pixel data. 
        std::shared_ptr < PixelSet > pixels;
        
        //! @brief Origin of the image in the pixel set.
        Property < SizePair > origin;
        
        //! @brief Size of the image in the pixel set. 
        Property < SizePair > size;
        
        //! @brief If this image was created from a file, holds this file path.
        Property < std::string > file;
        
    public:
        
        /*! @brief Constructs an empty image. */
        Image() = default;
        
        /*! @brief Creates a new Image from another Image. */
        Image(Image const& rhs) = default;
        
        /*! @brief Creates a new Image with a different view but the same PixelSet as 
         *  the other Image given. 
        **/
        Image(Image const& rhs, SizePair const& origin, SizePair const& size);
        
        /*! @brief Creates a new Image with a PixelSet, an origin and a size. */
        Image(std::shared_ptr < PixelSet > const& pixels, SizePair const& origin, SizePair const& size);
        
        /*! @brief Destructs the Image. */
        virtual ~Image() = default;
        
        /*! @brief Returns a pointer to the first pixel in the PixelSet from this image's view. 
         *
         * \note This pointer is not guaranteed to be the first pixel in the PixelSet. If the origin
         * is not zero, it is calculated as (first * origin.x) + (lineSize * origin.y).
         *
        **/
        virtual const unsigned char* raw() const;
        
        /*! @brief Returns the size of a line from the original PixelSet. This can be used by other objects
         *  to create a Texture with the correct view from the original PixelSet. Returned value is exprimed
         *  in number of Pixels to skip to go to the next line.
         *
        **/
        virtual const std::size_t findRowLength() const;
        
        /*! @brief Returns the format of the pixel set. */
        virtual const std::uint8_t pixelFormat() const;
        
        /*! @brief Returns the origin of the Image. */
        virtual SizePair const getOrigin() const;
        
        /*! @brief Returns the size of the Image. */
        virtual SizePair const getSize() const;
        
        /*! @brief Changes the origin. */
        virtual void setOrigin(SizePair const& origin);
        
        /*! @brief Changes the size. */
        virtual void setSize(SizePair const& size);
        
        /*! @brief Changes the PixelSet associated to this Image. */
        virtual void setPixelSet(std::shared_ptr < PixelSet > const& pixels);
        
        /*! @brief Returns the actual PixelSet. */
        virtual std::shared_ptr < PixelSet > getPixelSet() const;
        
        /*! @brief Returns the original file this image is from, if existing. */
        virtual std::string getFile() const;
    };
    
    template <>
    class FileLoader < Image > : public FileLoaderInterface 
    {
    public:
        virtual ~FileLoader() = default;
        
        /*! @brief Loads one image from a file. */
        virtual std::shared_ptr < Image > load(std::string const& filepath) const = 0;
    };
}

#endif // CLEAN_IMAGE_H
