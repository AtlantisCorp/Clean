/** \file GlDriver/Cocoa/OSXPrerequisites.h
**/

#ifndef GLDRIVER_OSXPREREQUISITES_H
#define GLDRIVER_OSXPREREQUISITES_H

#ifndef CLEAN_WINDOW_COCOA
#   error "Macro CLEAN_WINDOW_COCOA is not defined. Are you compiling on OSX?"
#endif 

// We includes OpenGL Framework. Clean/Language.h is used to know if we are compiling from
// C++ or from OBJ-C/OBJ-C++. When we have OBJ-C, we include Cocoa Framework. However, in 
// C++ only files, we declares 'id' as the generic 'void*' type. In fact, OBJ-C id is equivalent
// to a type of size uintptr_t, which is equivalent to void*. 

// ARC, C++ and OBJ-C++: To correctly use 'id' OBJ-C types with Clang's ARC, you must compile your
// implementations of all classes that use 'id' with the OBJ-C++ compiler. This let ARC do its 
// retain/release trick without making us think about it. However, returning an OBJ-C object to a C++
// only class should be forbidden, or the object should be returned with __unsafe_unretained or __weak
// properties (but really i don't know very much yet).

#include <OpenGL.h>
#include <Clean/Language.h>

#ifdef CLEAN_LANG_OBJC
#   include <Foundation/Foundation.h>
#   include <Cocoa/Cocoa.h>

#elif defined(CLEAN_LANG_CPP)
    typedef void* id;

#endif // CLEAN_LANG_*

#endif // GLDRIVER_OSXPREREQUISITES_H