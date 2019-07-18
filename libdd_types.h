#ifndef DENIS_TYPES_H_
#define DENIS_TYPES_H_

#include <stdint.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

typedef float f32;
typedef double f64;

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

#include <stdlib.h>

#define HEAP_ALLOC(size) calloc(1, size)
#define HEAP_FREE(pointer) free(pointer)

#if defined(DEBUG)
#define ASSERT(x)				\
do							\
{							\
	if (!(x))					\
	*(s8*)0 = 0;		\
} while(false)

#else
#define ASSERT(x)
#endif

#define INVALID_DEFAULT_CASE default: { ASSERT(false); } break

#define KILOBYTE(num) ((num) * (u64)1024)
#define MEGABYTE(num) (KILOBYTE(num) * (u64)1024)
#define GIGABYTE(num) (MEGABYTE(num) * (u64)1024)

#define ARRAY_COUNT(array) (sizeof(array)/sizeof((array)[0]))

#define SWAP(a, b, type) do { \
	type c = a; \
	a = b; \
	b = c; \
} while(0)

// copied from float.h
#define F32_MAX_VALUE 3.402823466e+38F

#define S32_MIN_VALUE 0x80000000
#define S32_MAX_VALUE 0x7FFFFFFF

#endif
