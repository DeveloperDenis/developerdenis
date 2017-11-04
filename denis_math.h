//----------------------------------------------------
//    denis_math.h
// Written by Denis Levesque
// NOTE: this will only compile with a C++ compiler
//----------------------------------------------------

#ifndef DENIS_MATH_H_
#define DENIS_MATH_H_

#include "denis_types.h"

//TODO(denis): remove this eventually?
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

static inline Vector2f V2f(real32 x, real32 y)
{
	Vector2f result = {x, y};
	return result;
}
static inline Vector2 V2(int32 x, int32 y)
{
    Vector2 result = {x, y};
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

struct Rectangle
{
    Vector2 min;
    Vector2 max;

    int32 getLeft() { return min.x;  };
    int32 getRight() { return max.x;  };
    int32 getTop() { return min.y;  };
    int32 getBottom() { return max.y;  };

    int32 getWidth() { return max.x - min.x; };
    int32 getHeight() { return max.y - min.y; };

	void moveLeft(int32 amount) { setX(min.x - amount); }
	void moveRight(int32 amount) { setX(min.x +amount); }
	//NOTE(denis): assumes that down is positive Y
	void moveUp(int32 amount) { setY(min.y - amount); }
	void moveDown(int32 amount) { setY(min.y + amount); }
	
    void setY(int32 newY)
    {
		int32 height = max.y - min.y;
		min.y = newY;
		max.y = newY + height;
    };

    void setX(int32 newX)
    {
		int32 width = max.x - min.x;
		min.x = newX;
		max.x = newX + width;
    };

    void setPos(Vector2 newPos)
    {
		setY(newPos.y);
		setX(newPos.x);
    };
};

static inline Rectangle Rect(int32 x, int32 y, int32 width, int32 height)
{
    Rectangle result;
    result.min.x = x;
    result.min.y = y;
    result.max.x = x + width;
    result.max.y = y + height;

    return result;
}

static inline Rectangle Rect(Vector2 min, Vector2 max)
{
    Rectangle result = {min, max};
    return result;
}

static inline bool pointInRect(Vector2 point, Rectangle rect)
{
    return point.x > rect.getLeft() && point.x < rect.getRight() &&
		point.y > rect.getTop() && point.y < rect.getBottom();
}

//TODO(denis): for now this only checks the smallest rect that contains the given circle
static inline bool pointInCircle(Vector2 point, Vector2 pos, int32 radius)
{
	return point.x > pos.x - radius && point.x < pos.x + radius &&
		point.y > pos.y - radius && point.y < pos.y + radius;
}

#endif
