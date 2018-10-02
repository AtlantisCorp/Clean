/** \file GlDriver/GlInclude.h
**/

#ifndef GLDRIVER_GLINCLUDE_H
#define GLDRIVER_GLINCLUDE_H

#ifdef CLEAN_PLATFORM_WIN
#   include <gl.h>

#elif defined(CLEAN_PLATFORM_LINUX)
#   include <GL/gl.h>

#elif defined(CLEAN_PLATFORM_OSX)
#   include <OpenGL/OpenGL.h>

#endif

#endif // GLDRIVER_GLINCLUDE_H