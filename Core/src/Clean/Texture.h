/** =======================================================
 *  \file Core/Texture.h
 *  \date 11/06/2018
 *  \author luk2010
    ======================================================= **/

#ifndef CLEAN_TEXTURE_H
#define CLEAN_TEXTURE_H

#include "DriverResource.h"
#include "Handled.h"

namespace Clean 
{
    /** @brief Interface to define a Texture object. */
    class Texture : public DriverResource, public Handled < Texture >
    {
    protected:
        
        //! @brief Internal format that should be used by the Texture when creating and uploading
        //! its data to the GPU memory. Notes that this format may not be used depending on how
        //! the driver will handle pixels.
        std::uint8_t internalFormat = 0;
        
    public:
        using DriverResource::DriverResource;
        
        /*! @brief Default destructor. */
        virtual ~Texture() = default;
        
        /*! @brief Returns the Texture's width. */
        virtual std::size_t getWidth() const = 0;
        
        /*! @brief Returns the Texture's height. */
        virtual std::size_t getHeight() const = 0;
        
        /*! @brief Binds the texture. */
        virtual void bind() const = 0;
    };
}

#endif // CLEAN_TEXTURE_H
