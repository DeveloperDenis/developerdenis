#if !defined(DENIS_DRAWING_H_)
#define DENIS_DRAWING_H_

static void fillBuffer(Bitmap* buffer, uint32 colour)
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
// - when lines are drawn vertical or close to vertical they nearly disappear
// - for line widths, need to take into account the slope when deciding how to widen line
static void drawLine(Bitmap* buffer, Vector2 point1, Vector2 point2, uint32 colour)
{
	Vector2 startPoint, endPoint;

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

	//TODO(denis): what if endPoint.x - startPoint.x approaches 0? need to fix
	real32 slope =
		((real32)endPoint.y - (real32)startPoint.y)/((real32)endPoint.x - (real32)startPoint.x);

	startPoint.x = MAX(startPoint.x, 0);
	endPoint.x = MIN(endPoint.x, (int32)buffer->width);
	
	if (endPoint.x < 0)
		return;
	
	for (int32 x = startPoint.x; x < endPoint.x; ++x)
	{
		int32 y = (int32)((x - startPoint.x) * slope);
		y = startPoint.y + y;

		if (y < 0 || y > (int32)buffer->height)
			continue;

		//IMPORTANT TODO(denis): this still occasionally accesses out of bounds memory,
		//so there must be some sort of problem with the clamping I do
		*(buffer->pixels + y*buffer->width + x) = colour;
	}
}

#endif
