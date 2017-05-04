//----------------------------------------------------
//    denis_math.h
// Written by Denis Levesque
// NOTE: this will only compile with a C++ compiler
//----------------------------------------------------

#ifndef DENIS_MATH_H_
#define DENIS_MATH_H_

#include "denis_types.h"

struct Vector2
{
    int32 x;
    int32 y;
};

static inline Vector2 V2(int32 x, int32 y)
{
    Vector2 result = { x, y };
    return result;
}

Vector2 operator+(Vector2 left, Vector2 right)
{
    Vector2 result;
    result.x = left.x + right.x;
    result.y = left.y + right.y;
    return result;
}

Vector2 operator-(Vector2 left, Vector2 right)
{
    Vector2 result;
    result.x = left.x - right.x;
    result.y = left.y - right.y;
    return result;
}

Vector2 operator*(Vector2 left, int32 right)
{
    Vector2 result;
    result.x = left.x * right;
    result.y = left.y * right;
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

#endif
