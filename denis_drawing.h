#if !defined(DENIS_DRAWING_H_)
#define DENIS_DRAWING_H_

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
	//TODO(denis): could do this more efficiently instead of looping through the entire
	// bitmap
	
	for (uint32 row = 0; row < buffer->height; ++row)
	{
		for (uint32 col = 0; col < buffer->width; ++col)
		{
			if (row >= (uint32)y && col >= (uint32)x && row < y + height && col < x + width)
			{
				*(buffer->pixels + row*buffer->width + col) = colour;
			}
		}
	}
}
static inline void drawRect(Bitmap* buffer, Rect2 rect, uint32 colour)
{
	drawRect(buffer, rect.getLeft(), rect.getTop(), rect.getWidth(), rect.getHeight(), colour);
}

//NOTE(denis): (x, y) is the centre of the circle
static void drawCircle(Bitmap* buffer, int32 x, int32 y, uint32 radius, uint32 colour)
{
	int32 startX = MAX(x - radius, 0);
	int32 startY = MAX(y - radius, 0);

	uint32 endX = x + radius < 0 ? 0 : x + radius;
	endX = endX > buffer->width ? buffer->width : endX;

	uint32 endY = y + radius < 0 ? 0 : y + radius;
	endY = endY > buffer->height ? buffer->height : endY;

	for (uint32 row = startY; row < endY; ++row)
	{
		for (uint32 col = startX; col < endX; ++col)
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

//TODO(denis): some things need to be fixed/added
// - add line width parameter
// - for line widths, need to take into account the slope when deciding how to widen line
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
