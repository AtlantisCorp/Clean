/** \file Core/Endianness.h
**/

#ifndef CLEAN_ENDIANNESS_H
#define CLEAN_ENDIANNESS_H

#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#   define CLEAN_BIG_ENDIAN
#elif __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#   define CLEAN_LITTLE_ENDIAN
#endif

#if CLEAN_PLATFORM_WIN32
#include <winsock2.h>

#else
#include <netinet/in.h>

#endif

#endif // CLEAN_ENDIANNESS_H