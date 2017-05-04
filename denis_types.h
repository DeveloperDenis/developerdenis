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
#define TRUE true
#define FALSE false
#endif

#endif
