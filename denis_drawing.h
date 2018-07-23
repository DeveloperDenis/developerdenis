#if !defined(DENIS_DRAWING_H_)
#define DENIS_DRAWING_H_

#include "denis_math.h"

static inline u32 packColour(v3f colour)
{
	u8 r = (u8)(0xFF * colour.r);
	u8 g = (u8)(0xFF * colour.g);
	u8 b = (u8)(0xFF * colour.b);

	return 0xFF000000 | (r << 16) | (g << 8) | b;
}

//NOTE(denis): draws the bitmap onto the buffer with x and y specified in rect
// clips bitmap width and height to rect width & rect height
//TODO(denis): doesn't draw partial alpha, only full or none
static void drawBitmap(Bitmap* buffer, Bitmap* bitmap, Rect2 rect)
{
	s32 startY = MAX(rect.getTop(), 0);
	s32 endY = MIN(rect.getBottom(), (s32)buffer->height);

	s32 startX = MAX(rect.getLeft(), 0);
	s32 endX = MIN(rect.getRight(), (s32)buffer->width);

	u32 row = 0;
	for (s32 y = startY; y < endY && row < bitmap->height; ++y, ++row)
	{
		u32 col = 0;
		for (s32 x = startX; x < endX && col < bitmap->width; ++x, ++col)
		{
			u32* inPixel = bitmap->pixels + row*bitmap->width + col;
			u32* outPixel = buffer->pixels + y*buffer->width + x;
			
			if (((*inPixel) & (0xFF << 24)) != 0)
			{
				*outPixel = *inPixel;
			}
		}
	}
}
static inline void drawBitmap(Bitmap* buffer, Bitmap* bitmap, v2 pos)
{
	Rect2 rect = Rect2(pos.x, pos.y, bitmap->width, bitmap->height);
	drawBitmap(buffer, bitmap, rect);
}

static inline void fillBuffer(Bitmap* buffer, u32 colour)
{
	for (u32 row = 0; row < buffer->height; ++row)
	{
		for (u32 col = 0; col < buffer->width; ++col)
		{
			*(buffer->pixels + col + row*buffer->width) = colour;
		}
	}
}

//NOTE(denis): (x, y) is the top left of the rectangle
static void drawRect(Bitmap* buffer,
					 s32 x, s32 y, u32 width, u32 height, u32 colour)
{
	if (x + (s32)width < 0 || y + (s32)height < 0)
		return;
	
	u32 startX = MAX(0, x);
	u32 startY = MAX(0, y);
	
	u32 endX = MIN(buffer->width, x + width);
	u32 endY = MIN(buffer->height, y + height);
	
	for (u32 row = startY; row < endY; ++row)
	{
		for (u32 col = startX; col < endX; ++col)
		{
			*(buffer->pixels + row*buffer->width + col) = colour;
		}
	}
}
static inline void drawRect(Bitmap* buffer, Rect2 rect, u32 colour)
{
	drawRect(buffer, rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight(), colour);
}

//NOTE(denis): (x, y) is the centre of the circle
static void drawCircle(Bitmap* buffer, s32 x, s32 y, s32 radius, u32 colour)
{
	if (x + radius < 0 || y + radius < 0)
		return;
	
	s32 startX = MAX(x - radius, 0);
	s32 startY = MAX(y - radius, 0);

	s32 endX = MIN(x + radius, (s32)buffer->width);
	s32 endY = MIN(y + radius, (s32)buffer->height);

	for (s32 row = startY; row < endY; ++row)
	{
		for (s32 col = startX; col < endX; ++col)
		{
			s32 xDiff = col - x;
			s32 yDiff = row - y;

			if (sqrt(xDiff*xDiff + yDiff*yDiff) <= (f32)radius)
			{
				*(buffer->pixels + row*buffer->width + col) = colour;
			}
		}
	}
}
static inline void drawCircle(Bitmap* buffer, v2 pos, s32 radius, u32 colour)
{
	drawCircle(buffer, pos.x, pos.y, radius, colour);
}

static inline void drawPoint(Bitmap* buffer, s32 x, s32 y, u32 colour)
{
	if (x < 0 || x >= (s32)buffer->width || y < 0 || y >= (s32)buffer->height)
		return;

	*(buffer->pixels + y*buffer->width + x) = colour;
}
static inline void drawPoint(Bitmap* buffer, v2 point, u32 colour)
{
	drawPoint(buffer, point.x, point.y, colour);
}

//NOTE(denis): Bresenham's algorithm for line drawing, optimized to only use integer values
static void drawLine(Bitmap* surface, v2 p1, v2 p2, u32 colour)
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

	v2 startP = p1;
	v2 endP = p2;
	
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
