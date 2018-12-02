/** 
 * \file GlDriver/GlInclude.cpp
**/

#include "GlInclude.h"
#include <cstring>

int GlIsExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if ( where || *extension == '\0' )
    return 0;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for ( start = extList; ; ) {
    where = strstr( start, extension );

    if ( !where )
      break;

    terminator = where + strlen( extension );

    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return 1;

    start = terminator;
  }

  return 0;
}
