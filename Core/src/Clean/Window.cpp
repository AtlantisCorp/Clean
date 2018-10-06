/** \file Core/Window.cpp
**/

#include "Window.h"

namespace Clean 
{
    Window::Window()
        : RenderSurface(NULL, NULL)
    {
        
    }
    
    Window::Window(NativeSurface handle, NativeSurface parent) 
        : RenderSurface(handle, parent)
    {
        
    }
}