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

//---------------------------------------------------------------------------
// Vector types:

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
    int32 e[2];
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
    real32 e[2];
};

union Vector3
{
	struct
	{
		int32 x;
		int32 y;
		int32 z;
	};
	struct
	{
		int32 r;
		int32 g;
		int32 b;
	};
	struct
	{
		Vector2 xy;
		int32 z;
	};
	int32 e[3];

	int32 operator[](uint32 index)
	{
		return e[index];
	}
};

union Vector3f
{
	struct
	{
		real32 x;
		real32 y;
		real32 z;
	};
	struct
	{
		real32 r;
		real32 g;
		real32 b;
	};
	real32 e[3];
};

union Vector4f
{
	struct
	{
		real32 x;
		real32 y;
		real32 z;
		real32 w;
	};
	real32 e[4];

	real32& operator[](uint32 index)
	{
		return e[index];
	}
};


//---------------------------------------------------------------------------
// Vector Constructors

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

static inline Vector3 V3(int32 x, int32 y, int32 z)
{
	Vector3 result = {x, y, z};
	return result;
}

static inline Vector3f V3f(real32 x, real32 y, real32 z)
{
	Vector3f result = {x, y, z};
	return result;
}

static inline Vector4f V4f(real32 x, real32 y, real32 z, real32 w)
{
	Vector4f result = {x, y, z, w};
	return result;
}
static inline Vector4f V4f(real32 x, real32 y, real32 z)
{
	Vector4f result = {x, y, z, 1.0f};
	return result;
}
static inline Vector4f V4f(Vector3f v, real32 w)
{
	Vector4f result = {v.x, v.y, v.z, w};
	return result;
}


//---------------------------------------------------------------------------
// Vector operator overloads

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

Vector2f operator+(Vector2f left, Vector2f right)
{
	Vector2f result;
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
Vector2f operator*(Vector2f left, real32 right)
{
	Vector2f result;
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

static inline Vector3 operator+(Vector3 left, Vector3 right)
{
	Vector3 result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}
static inline Vector3 operator-(Vector3 left, Vector3 right)
{
	Vector3 result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
static inline Vector3 operator*(Vector3 left, int32 right)
{
	Vector3 result;
	result.x = left.x * right;
	result.y = left.y * right;
	result.z = left.z * right;
	return result;
}
static inline Vector3 operator/(Vector3 left, int32 right)
{
	Vector3 result;
	result.x = left.x/right;
	result.y = left.y/right;
	result.z = left.z/right;
	return result;
}

static inline Vector3f operator-(Vector3f& v)
{
	Vector3f result;
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;		
	return result;
}
static inline Vector3f operator+(Vector3f left, Vector3f right)
{
	Vector3f result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;	
	return result;
}
static inline Vector3f operator-(Vector3f left, Vector3f right)
{
	Vector3f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
static inline Vector3f operator*(int32 left, Vector3f right)
{
	Vector3f result;
	result.x = right.x * left;
	result.y = right.y * left;
	result.z = right.z * left;
	return result;
}
static inline Vector3f operator*(Vector3f left, int32 right)
{
	return right*left;
}
static inline Vector3f operator*(Vector3f left, real32 right)
{
	Vector3f result;
	result.x = left.x * right;
	result.y = left.y * right;
	result.z = left.z * right;
	return result;
}
static inline Vector3f operator/(Vector3f left, real32 right)
{
	Vector3f result;
	result.x = left.x / right;
	result.y = left.y / right;
	result.z = left.z / right;
	return result;
}
static inline bool operator!=(Vector3f left, Vector3f right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z;
}

static inline Vector4f operator-(Vector4f left, Vector4f right)
{
	Vector4f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	result.w = left.w - right.w;	
	return result;
}
static inline Vector4f operator/(Vector4f left, real32 scalar)
{
	Vector4f result;	
	result.x = left.x / scalar;
	result.y = left.y / scalar;
	result.z = left.z / scalar;
	result.w = left.w / scalar;
	return result;
}

//TODO(denis): should my rectangles have a centre origin instead of top-left?
//TODO(denis): also, maybe put rectangles into a "denis_geometry.h" file once I get some more geometry code
//TODO(denis): put all types together at the top?
// Rectangle types
struct Rect2
{
    Vector2 min; // min is top-left
    Vector2 max; // max is bottom-right

    Rect2(int32 x, int32 y, int32 width, int32 height)
    {
	min = V2(x, y);
	max = V2(x + width, y LOWER_BY height);
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
	min = V2f(x, y);
	max = V2f(x + width, y LOWER_BY height);
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
