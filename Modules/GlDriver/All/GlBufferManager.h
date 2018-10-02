/** \file GlDriver/GlBufferManager.h
**/

#ifndef GLDRIVER_GLBUFFERMANAGER_H
#define GLDRIVER_GLBUFFERMANAGER_H

#include "GlBuffer.h"
#include <Clean/Manager.h>

/** @brief Manages a set of GlBuffer for GlDriver. */
class GlBufferManager : public Clean::Manager < GlBuffer >
{
public:
    
    /*! @brief Finds a buffer by its handle. */
    std::shared_ptr < GlBuffer > findByHandle(std::size_t handle);
};

#endif // GLDRIVER_GLBUFFERMANAGER_H