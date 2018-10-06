/**
 *  \file GlDriver/GlRenderQueue
 *  \module GlDriver
 *  \author luk2010
 *  \date 10/06/2018
**/

#ifndef GLDRIVER_GLRENDERQUEUE_H
#define GLDRIVER_GLRENDERQUEUE_H

#include <Clean/RenderQueue.h>

class GlRenderQueue : public Clean::RenderQueue 
{
public:
    
    /*! @brief Constructs the render queue. */
    GlRenderQueue(std::uint8_t type);
    
    /*! @brief Destructs the render queue. */
    ~GlRenderQueue() = default;
    
    /*! @brief Release the resources associated to this render queue. 
     * 
     * Actually in OpenGL i didn't see any related objects for a rendering queue. So we
     * do not have to release anything for now. However some other APIs may allocate or
     * use some specific objects. 
     *
    **/
    void release();
};

#endif // GLDRIVER_GLRENDERQUEUE_H