/** \file Core/TextureManager.cpp
**/

#include "TextureManager.h"

namespace Clean 
{
    TextureManager::~TextureManager() 
    {
        forEach([](auto texture){ assert(texture); texture->release(); });
    }
    
    void TextureManager::reset()
    {
        forEach([](auto texture){ assert(texture); texture->release(); });
        Manager::reset();
    }
}