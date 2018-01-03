//----------------------------------------------------
//    denis_types.h
// Written by Denis Levesque
//----------------------------------------------------

#ifndef DENIS_TYPES_H_
#define DENIS_TYPES_H_

#include <stdint.h>

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef float real32;
typedef double real64;

#ifndef __cplusplus
typedef enum { FALSE, TRUE } bool;
#else

#ifndef TRUE
#define TRUE true
#endif

#ifndef FALSE
#define FALSE false
#endif

#endif

 // HEAP_ALLOC & HEAP_FREE
#if defined(_WIN32)
 
#if defined(WINDOWS_UWP_APP)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#define HEAP_ALLOC(size) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size)
#define HEAP_FREE(ptr) HeapFree(GetProcessHeap(), 0, ptr)

#else
//TODO(denis): do I want to do it this way?
#include "denis_win32.h"
#endif
 
#else
 
#include <stdlib.h>
#define HEAP_ALLOC(size) malloc(size)
#define HEAP_FREE(pointer) free(pointer)
 
#endif

//TODO(denis): dunno if this should be here, also might want this to be more featured
#if defined(DEBUG)
#define ASSERT(x)				\
	do							\
	{							\
	  if (!(x))					\
		  *(int8*)0 = 0;		\
	} while(false)

#else
#define ASSERT(x)
#endif

#define KILOBYTE(num) ((num) * (uint64)1024)
#define MEGABYTE(num) (KILOBYTE(num) * (uint64)1024)
#define GIGABYTE(num) (MEGABYTE(num) * (uint64)1024)

#endif
