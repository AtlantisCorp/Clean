/** 
 *  \file GlDriver/GlTexture.h
 *  \date 11/06/2018
**/

#ifndef CLEAN_GLTEXTURE_H
#define CLEAN_GLTEXTURE_H

#include "GlInclude.h"

#include <Clean/Texture.h>
#include <Clean/Image.h>

#include <memory>

/** @brief Specialization of Clean::Texture. */
class GlTexture : public Clean::Texture 
{
    //! @brief This texture's handle. 
    GLuint handle;
    
    //! @brief Target used by this texture (or texture's type).
    GLenum target;
    
public:
    
    /*! @brief Constructs a GlTexture with a handle. */
    GlTexture(Clean::Driver* creator, GLuint hdl, GLenum trg);
    
    /*! @brief Returns the Texture's width of the base level. */
    std::size_t getWidth() const;
    
    /*! @brief Returns the Texture's height of the base level. */
    std::size_t getHeight() const;
    
    /*! @brief Binds the texture. */
    void bind() const;
    
    /*! @brief Uploads data to this texture. */
    bool upload(std::shared_ptr < Clean::Image > const& image);
    
protected:
    
    /*! @brief Implementation of the real resource releasing. */
    void releaseResource();
};

#endif // CLEAN_GLTEXTURE_H
