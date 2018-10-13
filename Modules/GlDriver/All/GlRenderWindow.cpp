/** \file GlDriver/GlRenderWindow.cpp
**/

#include "GlRenderWindow.h"
#include "GlCheckError.h"

#include <Clean/Driver.h>
using namespace Clean;

GlRenderWindow::GlRenderWindow() 
{
    glGenVertexArrays(1, &vao);
}

GlRenderWindow::~GlRenderWindow()
{
    glDeleteVertexArrays(1, &vao);
}

void GlRenderWindow::bind(Driver& driver) const 
{
    glBindVertexArray(vao);
}
