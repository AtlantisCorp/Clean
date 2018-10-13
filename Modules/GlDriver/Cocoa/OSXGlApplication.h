/** \file GlDriver/Cocoa/OSXGlApplication.h
**/

#ifndef GLDRIVER_OSXGLAPPLICATION_H
#define GLDRIVER_OSXGLAPPLICATION_H

#include "OSXPrerequisites.h"

#ifdef __OBJC__
/** @brief Custom interface to port NSApplication into multithread mode. */
@interface OSXGlApplication : NSApplication 
@end

#endif // __OBJ_C__

/*! @brief Checks wether an NSApp object is available. If not available, this means the program's user
 *  did not create any NSApplication object.
 *
 * When NSApp is not created before creating an OSXGlRenderWindow, this module automatically creates a
 * NSApplication with a delegate to run initialization of the application and creates a valid NSApp.
 *
**/
void OSXGlCheckNSApplicationRunning();

#endif // GLDRIVER_OSXGLAPPLICATION_H
