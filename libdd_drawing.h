#if !defined(DENIS_DRAWING_H_)
#define DENIS_DRAWING_H_

#include "libdd_math.h"
#include "memory_pools.h"

#define GET_PIXEL(bitmap, x, y) ((u32*)((u8*)(bitmap)->pixels + (y)*(bitmap)->stride) + (x))

/*
 * STRUCTS
 */

struct Bitmap
{
    u32* pixels;
	
	v2i dim;
	v2i pos;
	
	// number of bytes in a row
	u32 stride;
};

/*
 * FUNCTIONS
 */

// TODO(denis): not sure about this name
static inline v4f getPremultColour(f32 r, f32 b, f32 g, f32 a)
{
	return v4f(r*a, b*a, g*a, a);
}

static inline v4f unpackColour(u32 colour)
{
	v4f result;
	result.r = (f32)((colour & 0x00FF0000) >> 16) / 255.0f;
	result.g = (f32)((colour & 0x0000FF00) >> 8) / 255.0f;
	result.b = (f32)(colour & 0x000000FF) / 255.0f;
	result.a = (f32)(colour >> 24) / 255.0f;
	
	return result;
}

static inline u32 packColour(v3f colour)
{
	u8 r = (u8)(0xFF * colour.r);
	u8 g = (u8)(0xFF * colour.g);
	u8 b = (u8)(0xFF * colour.b);
	
	return 0xFF000000 | (r << 16) | (g << 8) | b;
}
static inline u32 packColour(v4f colour)
{
	u8 r = (u8)(0xFF * colour.r);
	u8 g = (u8)(0xFF * colour.g);
	u8 b = (u8)(0xFF * colour.b);
	u8 a = (u8)(0xFF * colour.a);
	
	return (a << 24) | (r << 16) | (g << 8) | b;
}

static inline void drawPoint(Bitmap* buffer, s32 x, s32 y, u32 colour)
{
	if (x < 0 || x >= buffer->dim.w || y < 0 || y >= buffer->dim.h)
		return;
	
	*(GET_PIXEL(buffer, x, y)) = colour;
}
static inline void drawPoint(Bitmap* buffer, v2i point, u32 colour)
{
	drawPoint(buffer, point.x, point.y, colour);
}

static inline void fillBitmap(Bitmap* bitmap, u32 colour)
{
	for (u32 row = 0; row < (u32)bitmap->dim.h; ++row)
	{
		for (u32 col = 0; col < (u32)bitmap->dim.w; ++col)
		{
			drawPoint(bitmap, col, row, colour);
		}
	}
}
static inline void fillBitmap(Bitmap* bitmap, v4f colour)
{
	u32 packedColour = packColour(colour);
	fillBitmap(bitmap, packedColour);
}

// TODO(denis): I don't know if I want all this bitmap allocating and de-allocating done
// in this file

// uses a pre-defined memory pool to allocate memory
static inline Bitmap allocBitmap(MemoryPool* pool, s32 width, s32 height)
{
	Bitmap result = {};
	
	result.dim = v2i(width, height);
	result.stride = width*sizeof(u32);
	result.pixels = (u32*)pushBlock(pool, width*height*sizeof(u32));
	
	fillBitmap(&result, 0);
	
	return result;
}
static inline Bitmap allocBitmap(MemoryPool* pool, v2i dim)
{
	return allocBitmap(pool, dim.w, dim.h);
}

// allocates from the heap
static inline Bitmap allocBitmap(s32 width, s32 height)
{
	Bitmap result = {};
	
	result.dim = v2i(width, height);
	result.stride = width * sizeof(u32);
	result.pixels = (u32*)HEAP_ALLOC(width*height*sizeof(u32));
	
	fillBitmap(&result, 0);
	
	return result;
}
static inline Bitmap allocBitmap(v2i dim)
{
	return allocBitmap(dim.w, dim.h);
}

// returns memory taken from a pre-defined memory pool
// NOTE(denis): memory pools are stacks, so this has to be done before allocating more memory
static inline void freeBitmap(MemoryPool* pool, Bitmap* bitmap)
{
	popBlock(pool, bitmap->dim.w*bitmap->dim.h*sizeof(u32));
	bitmap->dim = v2i();
	bitmap->stride = 0;
}

// frees data from the heap
static inline void freeBitmap(Bitmap* bitmap)
{
	HEAP_FREE(bitmap->pixels);
	bitmap->dim = v2i(0, 0);
	bitmap->stride = 0;
}

//NOTE(denis): draws the bitmap onto the buffer with x and y specified in pos
// assumes pos is the top-left coordinate of the bitmap
// clips bitmap width and height to rect width & rect height
// uses pre-multiplied alpha blending
static void drawBitmap(Bitmap* buffer, Bitmap* bitmap, v2i pos)
{
	s32 startY = MAX(pos.y, 0);
	s32 endY = MIN(pos.y + bitmap->dim.h, buffer->dim.h);
	
	s32 startX = MAX(pos.x, 0);
	s32 endX = MIN(pos.x + bitmap->dim.w, buffer->dim.w);
	
	s32 colOffset = MAX(0, 0 - pos.x);
	s32 rowOffset = MAX(0, 0 - pos.y);
	
	u32 row = rowOffset;
	for (s32 y = startY; y < endY; ++y, ++row)
	{
		u32 col = colOffset;
		for (s32 x = startX; x < endX; ++x, ++col)
		{
			// TODO(denis): this is all pretty slow,
			// probably partially because of the double conversion. First u32->f32 then f32->u32
			
			u32* inPixel = GET_PIXEL(bitmap, col, row);
			u32* outPixel = GET_PIXEL(buffer, x, y);
			
			f32 inR = (f32)(((*inPixel) & 0x00FF0000) >> 16);
			f32 inG = (f32)(((*inPixel) & 0x0000FF00) >> 8);
			f32 inB = (f32)((*inPixel) & 0x000000FF);
			f32 inA = (f32)(((*inPixel) & 0xFF000000) >> 24);
			
			f32 outR = (f32)(((*outPixel) & 0x00FF0000) >> 16);
			f32 outG = (f32)(((*outPixel) & 0x0000FF00) >> 8);
			f32 outB = (f32)((*outPixel) & 0x000000FF);
			f32 outA = (f32)(((*outPixel) & 0xFF000000) >> 24);
			
			f32 inAFraction = inA / 255.0f;
			f32 outAFraction = outA / 255.0f;
			
			f32 r = outR*(1.0f - inAFraction) + inR;
			f32 g = outG*(1.0f - inAFraction) + inG;
			f32 b = outB*(1.0f - inAFraction) + inB;
			f32 a = inAFraction + outAFraction*(1.0f - inAFraction);
			
			u32 rByte = (u32)r;
			u32 gByte = (u32)g;
			u32 bByte = (u32)b;
			u32 aByte = (u32)(a * 255.0f);
			
			u32 colour = (aByte << 24) | (rByte << 16) | (gByte << 8) | (bByte);
			
			*outPixel = colour;
		}
	}
}
//NOTE(denis): this assumes x and y are the top-left coordinates
static inline void drawBitmap(Bitmap* buffer, Bitmap* bitmap, u32 x, u32 y)
{
	drawBitmap(buffer, bitmap, v2i(x, y));
}
static inline void drawBitmap(Bitmap* buffer, Bitmap* bitmap, Rect2i rect)
{
	drawBitmap(buffer, bitmap, rect.topLeft());
}

//NOTE(denis): (x, y) is the top left of the rectangle
static void drawRect(Bitmap* buffer, s32 x, s32 y, s32 width, s32 height, u32 colour)
{
	ASSERT(width >= 0 && height >= 0);
	
	if (x + width < 0 || y + height < 0)
		return;
	
	u32 startX = MAX(0, x);
	u32 startY = MAX(0, y);
	
	u32 endX = MIN(buffer->dim.w, x + width);
	u32 endY = MIN(buffer->dim.h, y + height);
	
	for (u32 row = startY; row < endY; ++row)
	{
		for (u32 col = startX; col < endX; ++col)
		{
			drawPoint(buffer, col, row, colour);
		}
	}
}
static inline void drawRect(Bitmap* buffer, v2i pos, v2i dim, u32 colour)
{
	drawRect(buffer, pos.x, pos.y, dim.w, dim.h, colour);
}
static inline void drawRect(Bitmap* buffer, Rect2i rect, u32 colour)
{
	drawRect(buffer, rect.left(), rect.top(), rect.width(), rect.height(), colour);
}

// NOTE(denis): these functions assume a premultiplied colour vector, and they do proper
// alpha blending
static inline void drawRect(Bitmap* buffer, s32 x, s32 y, s32 width, s32 height, v4f colour)
{
	ASSERT(width >= 0 && height >= 0);
	
	if (x + width < 0 || y + height < 0)
		return;
	
	u32 startX = MAX(0, x);
	u32 startY = MAX(0, y);
	
	u32 endX = MIN(buffer->dim.w, x + width);
	u32 endY = MIN(buffer->dim.h, y + height);
	
	for (u32 row = startY; row < endY; ++row)
	{
		for (u32 col = startX; col < endX; ++col)
		{
			u32* outPixel = GET_PIXEL(buffer, col, row);
			
			f32 inR = colour.r*255.0f;
			f32 inG = colour.g*255.0f;
			f32 inB = colour.b*255.0f;
			f32 inA = colour.a;
			
			f32 outR = (f32)(((*outPixel) & 0x00FF0000) >> 16);
			f32 outG = (f32)(((*outPixel) & 0x0000FF00) >> 8);
			f32 outB = (f32)((*outPixel) & 0x000000FF);
			f32 outA = (f32)(((*outPixel) & 0xFF000000) >> 24);
			f32 outAFraction = outA / 255.0f;
			
			f32 r = outR*(1.0f - inA) + inR;
			f32 g = outG*(1.0f - inA) + inG;
			f32 b = outB*(1.0f - inA) + inB;
			f32 a = inA + outAFraction*(1.0f - inA);
			
			u32 rByte = (u32)r;
			u32 gByte = (u32)g;
			u32 bByte = (u32)b;
			u32 aByte = (u32)(a * 255.0f);
			
			u32 outColour = (aByte << 24) | (rByte << 16) | (gByte << 8) | (bByte);
			drawPoint(buffer, col, row, outColour);
		}
	}
}
static inline void drawRect(Bitmap* buffer, v2i pos, v2i dim, v4f colour)
{
	drawRect(buffer, pos.x, pos.y, dim.w, dim.h, colour);
}
static inline void drawRect(Bitmap* buffer, Rect2i rect, v4f colour)
{
	drawRect(buffer, rect.left(), rect.top(), rect.width(), rect.height(), colour);
}

// NOTE(denis): this assumes that x and y are the top-left coordinates
static void drawRectOutline(Bitmap* buffer, s32 x, s32 y, s32 width, s32 height,
							u32 colour, s32 borderWidth)
{
	ASSERT(width >= 0 && height >= 0);
	
	if (x + width < 0 || y + height < 0)
		return;
	
	u32 startX = MAX(0, x);
	u32 startY = MAX(0, y);
	
	u32 endX = MIN(buffer->dim.w, x + width);
	u32 endY = MIN(buffer->dim.h, y + height);
	
	// TODO(denis): it would probably be faster to split this into 4 for loops,
	// one for each side of the border
	// or just call drawRectangle 4 times?
	for (u32 row = startY; row < endY; ++row)
	{
		for (u32 col = startX; col < endX; ++col)
		{
			if (((s32)col <= x + borderWidth || (s32)col >= x + width - borderWidth) ||
				((s32)row <= y + borderWidth || (s32)row >= y + height - borderWidth))
			{
				drawPoint(buffer, col, row, colour);
			}
		}
	}
}
// NOTE(denis): this assumes that pos is the top-left coordinate of the rect
static inline void drawRectOutline(Bitmap* buffer, v2i pos, v2i dim, u32 colour, s32 borderWidth)
{
	drawRectOutline(buffer, pos.x, pos.y, dim.w, dim.h, colour, borderWidth);
}
static inline void drawRectOutline(Bitmap* buffer, Rect2i rect, u32 colour, s32 borderWidth)
{
	drawRectOutline(buffer, rect.left(), rect.top(), rect.width(), rect.height(),
					colour, borderWidth);
}
static inline void drawRectOutline(Bitmap* buffer, s32 x, s32 y, s32 width, s32 height,
								   v4f colour, s32 borderWidth)
{
	u32 packedColour = packColour(colour);
	drawRectOutline(buffer, x, y, width, height, packedColour, borderWidth);
}
static inline void drawRectOutline(Bitmap* buffer, v2i pos, v2i dim, v4f colour, s32 borderWidth)
{
	u32 packedColour = packColour(colour);
	drawRectOutline(buffer, pos.x, pos.y, dim.w, dim.h, packedColour, borderWidth);
}
static inline void drawRectOutline(Bitmap* buffer, Rect2i rect, v4f colour, s32 borderWidth)
{
	u32 packedColour = packColour(colour);
	drawRectOutline(buffer, rect.left(), rect.top(), rect.width(), rect.height(), packedColour,
					borderWidth);
}

//NOTE(denis): (x, y) is the centre of the circle
static void drawCircle(Bitmap* buffer, s32 x, s32 y, s32 radius, u32 colour)
{
	if (x + radius < 0 || y + radius < 0)
		return;
	
	s32 startX = MAX(x - radius, 0);
	s32 startY = MAX(y - radius, 0);
	
	s32 endX = MIN(x + radius, buffer->dim.w);
	s32 endY = MIN(y + radius, buffer->dim.h);
	
	for (s32 row = startY; row < endY; ++row)
	{
		for (s32 col = startX; col < endX; ++col)
		{
			s32 xDiff = col - x;
			s32 yDiff = row - y;
			
			if (sqrt(xDiff*xDiff + yDiff*yDiff) <= (f32)radius)
			{
				drawPoint(buffer, col, row, colour);
			}
		}
	}
}
static inline void drawCircle(Bitmap* buffer, v2i pos, s32 radius, u32 colour)
{
	drawCircle(buffer, pos.x, pos.y, radius, colour);
}


//NOTE(denis): Bresenham's algorithm for line drawing, optimized to only use integer values
static void drawLine(Bitmap* surface, v2i p1, v2i p2, u32 colour)
{
	s32 rise = p2.y - p1.y;
	s32 run = p2.x - p1.x;
	
	//NOTE(denis): special horizontal line case
	if (rise == 0)
	{
		s32 x1 = p1.x;
		s32 x2 = p2.x;
		
		if (x2 < x1)
		{
			x1 = p2.x;
			x2 = p1.x;
		}
		
		for (s32 x = x1; x < x2; ++x)
		{
			drawPoint(surface, x, p1.y, colour);
		}
		
		return;
	}
	
	//NOTE(denis): special vertical line case
	if (run == 0)
	{
		s32 y1 = p1.y;
		s32 y2 = p2.y;
		
		if (y2 < y1)
		{
			y1 = p2.y;
			y2 = p1.y;
		}
		
		for (s32 y = y1; y < y2; ++y)
		{
			drawPoint(surface, p1.x, y, colour);
		}
		
		return;
	}
	
	//NOTE(denis): typical line cases
	f32 m = (f32)rise / (f32)run;
	s32 stepValue = m >= 0.0f ? 1 : -1;
	
	s32 offset = 0;
	
	v2i startP = p1;
	v2i endP = p2;
	
	if (ABS_VALUE(rise) > ABS_VALUE(run)) // vertical line
	{
		s32 delta = ABS_VALUE(run) * 2;
		s32 threshold = ABS_VALUE(rise);
		s32 thresholdIncrement = threshold * 2;
		
		if (p2.y < p1.y)
		{
			startP = p2;
			endP = p1;
		}
		
		s32 x = startP.x;
		
		for (s32 y = startP.y; y < endP.y; ++y)
		{
			drawPoint(surface, x, y, colour);
			
			offset += delta;
			if (offset >= threshold)
			{
				x += stepValue;
				threshold += thresholdIncrement;
			}
		}
		
	}
	else // horizontal line
	{
		s32 delta = ABS_VALUE(rise) * 2;
		s32 threshold = ABS_VALUE(run);
		s32 thresholdIncrement = threshold*2;
		
		if (p2.x < p1.x)
		{
			startP = p2;
			endP = p1;
		}
		
		s32 y = startP.y;
		
		for (s32 x = startP.x; x < endP.x; ++x)
		{
			drawPoint(surface, x, y, colour);
			
			offset += delta;
			if (offset >= threshold)
			{
				y += stepValue;
				threshold += thresholdIncrement;
			}
		}
	}
}

#endif
