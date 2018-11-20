/** \file Core/TextureManager.h
**/

#ifndef CLEAN_TEXTUREMANAGER_H
#define CLEAN_TEXTUREMANAGER_H

#include "Texture.h"
#include "Manager.h"

namespace Clean 
{
    class TextureManager : public Manager < Texture >
    {
    public:
        
        /*! @brief Calls Texture::release on all objects before destroying. */
        ~TextureManager();
        
        /*! @brief Calls Texture::release and then Manager::reset. */
        void reset();
    };
}

#endif // CLEAN_TEXTUREMANAGER_H