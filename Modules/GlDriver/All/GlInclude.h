/** \file GlDriver/GlInclude.h
**/

#ifndef GLDRIVER_GLINCLUDE_H
#define GLDRIVER_GLINCLUDE_H

#ifdef CLEAN_PLATFORM_WIN
#   include <gl3.h>

#elif defined(CLEAN_PLATFORM_MACOS)
#   include <OpenGL/OpenGL.h>
#   include <OpenGL/gl3.h>

#elif defined(CLEAN_PLATFORM_LINUX)
#   include <GL/gl3.h>

#endif

#endif // GLDRIVER_GLINCLUDE_H
