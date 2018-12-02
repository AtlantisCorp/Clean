/** \file GlDriver/GlRenderWindow.cpp
**/

#include "GlRenderWindow.h"
#include "GlCheckError.h"

#include <Clean/Driver.h>
using namespace Clean;

GlRenderWindow::GlRenderWindow(GlPtrTable const& tbl) : gl(tbl) 
{
    gl.genVertexArrays(1, &vao);
}

GlRenderWindow::GlRenderWindow(GlRenderWindow const& rhs) : gl(rhs.gl)
{
    gl.genVertexArrays(1, &vao);
}

GlRenderWindow::~GlRenderWindow()
{
    gl.deleteVertexArrays(1, &vao);
}

void GlRenderWindow::bind(Driver& driver) const 
{
    gl.bindVertexArray(vao);
}
