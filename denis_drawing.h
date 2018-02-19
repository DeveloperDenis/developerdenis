#if !defined(DENIS_DRAWING_H_)
#define DENIS_DRAWING_H_

#include "denis_math.h"

//NOTE(denis): draws the bitmap onto the buffer with x and y specified in rect
// clips bitmap width and height to rect width & rect height
//TODO(denis): doesn't draw partial alpha, only full or none
static void drawBitmap(Bitmap* buffer, Bitmap* bitmap, Rect2 rect)
{
	int32 startY = MAX(rect.getTop(), 0);
	int32 endY = MIN(rect.getBottom(), (int32)buffer->height);

	int32 startX = MAX(rect.getLeft(), 0);
	int32 endX = MIN(rect.getRight(), (int32)buffer->width);

	uint32 row = 0;
	for (int32 y = startY; y < endY && row < bitmap->height; ++y, ++row)
	{
		uint32 col = 0;
		for (int32 x = startX; x < endX && col < bitmap->width; ++x, ++col)
		{
			uint32* inPixel = bitmap->pixels + row*bitmap->width + col;
			uint32* outPixel = buffer->pixels + y*buffer->width + x;

			if (((*inPixel) & (0xFF << 24)) != 0)
			{
				*outPixel = *inPixel;
			}
		}
	}
}
static inline void drawBitmap(Bitmap* buffer, Bitmap* bitmap, Vector2 pos)
{
	Rect2 rect = Rect2(pos.x, pos.y, bitmap->width, bitmap->height);
	drawBitmap(buffer, bitmap, rect);
}

static inline void fillBuffer(Bitmap* buffer, uint32 colour)
{
    for (uint32 row = 0; row < buffer->height; ++row)
    {
	for (uint32 col = 0; col < buffer->width; ++col)
	{
	    *(buffer->pixels + col + row*buffer->width) = colour;
	}
    }
}

//NOTE(denis): (x, y) is the top left of the rectangle
static void drawRect(Bitmap* buffer,
		     int32 x, int32 y, uint32 width, uint32 height, uint32 colour)
{
    if (x + (int32)width < 0 || y + (int32)height < 0)
	return;
    
    uint32 startX = MAX(0, x);
    uint32 startY = MAX(0, y);

    uint32 endX = MIN(buffer->width, x + width);
    uint32 endY = MIN(buffer->height, y + height);
	
    for (uint32 row = startY; row < endY; ++row)
    {
	for (uint32 col = startX; col < endX; ++col)
	{
	    *(buffer->pixels + row*buffer->width + col) = colour;
	}
    }
}
static inline void drawRect(Bitmap* buffer, Rect2 rect, uint32 colour)
{
    drawRect(buffer, rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight(), colour);
}

//NOTE(denis): (x, y) is the centre of the circle
static void drawCircle(Bitmap* buffer, int32 x, int32 y, int32 radius, uint32 colour)
{
    if (x + radius < 0 || y + radius < 0)
	return;
    
    int32 startX = MAX(x - radius, 0);
    int32 startY = MAX(y - radius, 0);

    int32 endX = MIN(x + radius, (int32)buffer->width);
    int32 endY = MIN(y + radius, (int32)buffer->height);

    for (int32 row = startY; row < endY; ++row)
    {
	for (int32 col = startX; col < endX; ++col)
	{
	    int32 xDiff = col - x;
	    int32 yDiff = row - y;

	    if (sqrt(xDiff*xDiff + yDiff*yDiff) <= (real32)radius)
	    {
		*(buffer->pixels + row*buffer->width + col) = colour;
	    }
	}
    }
}
static inline void drawCircle(Bitmap* buffer, Vector2 pos, int32 radius, uint32 colour)
{
	drawCircle(buffer, pos.x, pos.y, radius, colour);
}

static inline void drawPoint(Bitmap* buffer, int32 x, int32 y, uint32 colour)
{
	if (x < 0 || x >= (int32)buffer->width || y < 0 || y >= (int32)buffer->height)
		return;

	*(buffer->pixels + y*buffer->width + x) = colour;
}

//TODO(denis): some things need to be fixed/added
// - add line width parameter
// - for line widths, need to take into account the slope when deciding how to widen line
// - also this is probably overly verbose and inefficient
// if this is deemed too slow, look into "Bresenham's line algorithm"
static void drawLine(Bitmap* buffer, Vector2 point1, Vector2 point2, uint32 colour)
{
    Vector2 startPoint;
    Vector2 endPoint;
	
    int32 rise = point2.y - point1.y;
    int32 run = point2.x - point1.x;

    real32 slope;
	
    if (ABS_VALUE(rise) > ABS_VALUE(run))
    {
	if (point1.y < point2.y)
	{
	    startPoint = point1;
	    endPoint = point2;
	}
	else
	{
	    startPoint = point2;
	    endPoint = point1;
	}

	if (startPoint.y >= (int32)buffer->height || endPoint.y < 0)
	    return;

	uint32 startY = MAX(startPoint.y, 0);
	uint32 endY = MIN(endPoint.y, (int32)buffer->height);

	rise = endPoint.y - startPoint.y;
	run = endPoint.x - startPoint.x;

	if (run == 0)
	    slope = 1.0f;
	else
	    slope = (real32)rise/(real32)run;
		
	for (uint32 y = startY; y < endY; ++y)
	{
	    int32 x;
	    if (slope == 1.0f)
	    {
		x = startPoint.x;
	    }
	    else
	    {
		x = (int32)((real32)(y - startPoint.y)/slope) + startPoint.x;
	    }

	    if (x > 0 && x < (int32)buffer->width)
		*(buffer->pixels + y*buffer->width + x) = colour;
	}
    }
    else
    {
	if (point1.x < point2.x)
	{
	    startPoint = point1;
	    endPoint = point2;
	}
	else
	{
	    startPoint = point2;
	    endPoint = point1;
	}

	if (startPoint.x >= (int32)buffer->width || endPoint.x < 0)
	    return;

	uint32 startX = MAX(startPoint.x, 0);
	uint32 endX = MIN(endPoint.x, (int32)buffer->width);

	rise = endPoint.y - startPoint.y;
	run = endPoint.x - startPoint.x;

	slope = (real32)rise/(real32)run;
		
	for (uint32 x = startX; x < endX; ++x)
	{
	    int32 y = (int32)(slope*(x - startPoint.x)) + startPoint.y;

	    if (y > 0 && y < (int32)buffer->height)
		*(buffer->pixels + y*buffer->width + x) = colour;
	}
    }
}

#endif
