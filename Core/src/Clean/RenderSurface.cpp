/** \file Core/RenderSurface.cpp
**/

#include "RenderSurface.h"

namespace Clean 
{
    RenderSurface::RenderSurface(NativeSurface hdl, NativeSurface p) 
    {
        handle.store(hdl);
        parent.store(p);
    }
    
    NativeSurface RenderSurface::getNativeHandle() const 
    {
        return handle.load();
    }
    
    NativeSurface RenderSurface::getNativeParent() const 
    {
        return parent.load();
    }
    
    void RenderSurface::resetHandles(NativeSurface h, NativeSurface p)
    {
        handle.store(h);
        parent.store(p);
    }
}