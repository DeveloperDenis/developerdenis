//----------------------------------------------------
//    denis_math.h
// Written by Denis Levesque
// NOTE: this will only compile with a C++ compiler
//----------------------------------------------------

#if defined(UP_POSITIVE_Y)
#define LOWER_BY -
#define RAISE_BY +
#else
#define LOWER_BY +
#define RAISE_BY -
#endif

#ifndef DENIS_MATH_H_
#define DENIS_MATH_H_

#include "denis_types.h"

//TODO(denis): remove these eventually?
#define _USE_MATH_DEFINES
#include "math.h"

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x, y) ((x) > (y) ? (x) : (y))
#endif

#define ABS_VALUE(x) ((x) < 0 ? -(x) : (x))

#define CLAMP_MIN(value, min) MIN(value, min)
#define CLAMP_RANGE(value, min, max) ((value) > (min) ? MIN(value, max) : (min))
#define CLAMP_MAX(value, max) MAX(value, max)

#define ARRAY_COUNT(array, type) (sizeof(array)/sizeof(type))

union Vector2
{
	struct
	{
		int32 x;
		int32 y;
	};
	struct
	{
		int32 w;
		int32 h;
	};
};

union Vector2f
{
	struct
	{
		real32 x;
		real32 y;
	};
	struct
	{
		real32 w;
		real32 h;
	};
};

static inline Vector2f V2f(real32 x, real32 y)
{
	Vector2f result = {x, y};
	return result;
}
static inline Vector2f V2f(Vector2 v2)
{
	Vector2f result = {(real32)v2.x, (real32)v2.y};
	return result;
}

static inline Vector2 V2(int32 x, int32 y)
{
    Vector2 result = {x, y};
    return result;
}
static inline Vector2 V2(Vector2f v2f)
{
	Vector2 result = {(int32)v2f.x, (int32)v2f.y};
	return result;
}

Vector2f operator+(Vector2f left, Vector2f right)
{
	Vector2f result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
Vector2 operator+(Vector2 left, Vector2 right)
{
    Vector2 result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    return result;
}

Vector2f operator-(Vector2f left, Vector2f right)
{
	Vector2f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
Vector2 operator-(Vector2 left, Vector2 right)
{
    Vector2 result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    return result;
}

Vector2f operator*(Vector2f left, real32 right)
{
	Vector2f result;
	result.x = left.x * right;
	result.y = left.y * right;
	return result;
}
Vector2 operator*(Vector2 left, int32 right)
{
    Vector2 result;
    result.x = left.x * right;
    result.y = left.y * right;
    return result;
}

Vector2f operator/(Vector2f left, real32 right)
{
	Vector2f result;
	result.x = left.x / right;
	result.y = left.y / right;
	return result;
}
Vector2 operator/(Vector2 left, int32 right)
{
    Vector2 result;
    result.x = left.x / right;
    result.y = left.y / right;
    return result;
}

struct Rect2
{
    Vector2 min; // min is top-left
    Vector2 max; // max is bottom-right

	Rect2(int32 x, int32 y, int32 width, int32 height)
	{
		min = {x, y};
		max = {x + width, y LOWER_BY height};
	};

    Rect2(Vector2 min, Vector2 max)
	{
		this->min = min;
		this->max = max;
	};
	
    int32 getLeft() { return min.x; };
    int32 getRight() { return max.x; };
    int32 getTop() { return min.y; };
    int32 getBottom() { return max.y; };

    int32 getWidth() { return max.x - min.x; };
    int32 getHeight() { return ABS_VALUE(max.y - min.y); };

	void moveLeft(int32 amount) { setX(min.x - amount); }
	void moveRight(int32 amount) { setX(min.x + amount); }
	void moveUp(int32 amount) { setY(min.y RAISE_BY amount); }
	void moveDown(int32 amount) { setY(min.y LOWER_BY amount); }

	void setX(int32 newX)
    {
		int32 width = getWidth();
		min.x = newX;
		max.x = newX + width;
    };
    void setY(int32 newY)
    {
		int32 height = getHeight();
		min.y = newY;
		max.y = newY LOWER_BY height;
    };
	
    void setPos(Vector2 newPos)
    {
		setX(newPos.x);
		setY(newPos.y);
    };
};

struct Rect2f
{
	Vector2f min;
	Vector2f max;

	Rect2f(real32 x, real32 y, real32 width, real32 height)
	{
		min = {x, y};
		max = {x + width, y LOWER_BY height};
	};

	Rect2f(Vector2f min, Vector2f max)
	{
		this->min = min;
		this->max = max;
	};
	
	real32 getLeft() { return min.x; };
	real32 getRight() { return max.x; };
	real32 getTop() { return min.y; };
	real32 getBottom() { return max.y; };

	real32 getWidth() { return max.x - min.x; };
	real32 getHeight() { return ABS_VALUE(max.y - min.y); };

	void setX(real32 newX)
	{
		real32 width = getWidth();
		min.x = newX;
		max.x = newX + width;
	};
	void setY(real32 newY)
	{
		real32 height = getHeight();
		min.y = newY;
		max.y = newY LOWER_BY height;
	};

	void setPos(Vector2f newPos)
	{
		setX(newPos.x);
		setY(newPos.y);
	};
};

#if defined(UP_POSITIVE_Y)
static inline bool pointInRect(Vector2 point, Rect2 rect)
{
	return point.x > rect.getLeft() && point.x < rect.getRight() &&
		point.y < rect.getTop() && point.y > rect.getBottom();
}
#else
static inline bool pointInRect(Vector2 point, Rect2 rect)
{
    return point.x > rect.getLeft() && point.x < rect.getRight() &&
		point.y > rect.getTop() && point.y < rect.getBottom();
}
#endif

//TODO(denis): for now this only checks the smallest rect that contains the given circle
static inline bool pointInCircle(Vector2 point, Vector2 pos, int32 radius)
{
	return point.x > pos.x - radius && point.x < pos.x + radius &&
		point.y > pos.y - radius && point.y < pos.y + radius;
}
static inline bool pointInCircle(Vector2f point, Vector2f pos, int32 radius)
{
	return point.x > pos.x - radius && point.x < pos.x + radius &&
		point.y > pos.y - radius && point.y < pos.y + radius;	
}

#endif
