/** \file Core/RenderSurface.cpp
**/

#include "RenderSurface.h"

namespace Clean 
{
    NativeSurface RenderSurface::getNativeHandle() const 
    {
        return handle.load();
    }
    
    NativeSurface RenderSurface::getNativeParent() const 
    {
        return parent.load();
    }
}