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
// Type Declarations:
union Vector2;
union Vector2f;
union Vector3;
union Vector3f;
union Vector4f;

struct Matrix4f;

struct Rect2;
struct Rect2f;

//---------------------------------------------------------------------------
// Functions Declarations:

static inline bool pointInRect(Vector2 point, Rect2 rect);

static inline bool pointInCircle(Vector2 point, Vector2 pos, int32 radius);
static inline bool pointInCircle(Vector2f point, Vector2f pos, int32 radius);

static inline real32 slope(Vector2 point1, Vector2 point2);
static inline real32 inverseSlope(Vector2 point1, Vector2 point2);

static inline real32 magnitude(Vector2 v);
static inline real32 magnitude(Vector3f v);

static inline Vector3f normalize(Vector3f vector);

static inline real32 dot(Vector3f v1, Vector3f v2);

static inline Vector3f cross(Vector3f v1, Vector3f v2);
static inline Vector4f cross(Vector4f v1, Vector4f v2);


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
		int32 filler;
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
// Matrix types:
// 4 x 4 transformation matrix in column-major style

struct Matrix4f
{
    real32 elements[4][4];

	Vector3f currentScale;

	real32* operator[](uint32 index)
	{
		return elements[index];
	}

	void setRow(uint32 row, Vector3f newValues);

	void setTranslation(real32 x, real32 y, real32 z);
	void setTranslation(Vector3f translation);

	Vector3f getTranslation();

	void translate(real32 x, real32 y, real32 z);
	
	void setScale(real32 x, real32 y, real32 z);
	void setScale(Vector3f newScale);

	Vector3f getScale();
	
	void scale(real32 x, real32 y, real32 z);
	
	//NOTE(denis): these are in radians
	void setRotation(real32 xAngle, real32 yAngle, real32 zAngle);

	void rotate(real32 xAngle, real32 yAngle, real32 zAngle);
};

//---------------------------------------------------------------------------
// Rectangle types:

//TODO(denis): should my rectangles have a centre origin instead of top-left?
//TODO(denis): also, maybe put rectangles into a "denis_geometry.h" file once I get some more geometry code
//TODO(denis): put all types together at the top?
// Rectangle types
struct Rect2
{
    Vector2 min; // min is top-left
    Vector2 max; // max is bottom-right

    Rect2(int32 x, int32 y, int32 width, int32 height);
    Rect2(Vector2 min, Vector2 max);
	
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

    void setX(int32 newX);
    void setY(int32 newY);
    void setPos(Vector2 newPos);
};

struct Rect2f
{
    Vector2f min;
    Vector2f max;

    Rect2f(real32 x, real32 y, real32 width, real32 height);
    Rect2f(Vector2f min, Vector2f max);
	
    real32 getLeft() { return min.x; };
    real32 getRight() { return max.x; };
    real32 getTop() { return min.y; };
    real32 getBottom() { return max.y; };

    real32 getWidth() { return max.x - min.x; };
    real32 getHeight() { return ABS_VALUE(max.y - min.y); };

    void setX(real32 newX);
    void setY(real32 newY);
    void setPos(Vector2f newPos);
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

//--------------------------------------------------------------------------
// Matrix Helper Functions

static inline Matrix4f getIdentityMatrix4f()
{
	Matrix4f result = {};
	result.currentScale = V3f(1.0f, 1.0f, 1.0f);

	result[0][0] = 1.0f;
	result[1][1] = 1.0f;
	result[2][2] = 1.0f;
	result[3][3] = 1.0f;
	
	return result;
}

static inline Matrix4f M4f()
{
	Matrix4f result = getIdentityMatrix4f();
	return result;
}

static inline Matrix4f getXRotationMatrix(real32 xAngle)
{
	Matrix4f xRotation = getIdentityMatrix4f();
	xRotation[1][1] = (real32)cos(xAngle);
	xRotation[1][2] = (real32)-sin(xAngle);
	xRotation[2][1] = (real32)sin(xAngle);
	xRotation[2][2] = (real32)cos(xAngle);

	return xRotation;
}
static inline Matrix4f getYRotationMatrix(real32 yAngle)
{
	Matrix4f yRotation = getIdentityMatrix4f();
	yRotation[0][0] = (real32)cos(yAngle);
	yRotation[0][2] = (real32)sin(yAngle);
	yRotation[2][0] = (real32)-sin(yAngle);
	yRotation[2][2] = (real32)cos(yAngle);

	return yRotation;
}
static inline Matrix4f getZRotationMatrix(real32 zAngle)
{
	Matrix4f zRotation = getIdentityMatrix4f();
	zRotation[0][0] = (real32)cos(zAngle);
	zRotation[0][1] = (real32)-sin(zAngle);
	zRotation[1][0] = (real32)sin(zAngle);
	zRotation[1][1] = (real32)cos(zAngle);

	return zRotation;
}

//--------------------------------------------------------------------------
// Matrix Operator Overloads

//TODO(denis): not efficient at all!
static inline Matrix4f operator*(Matrix4f left, Matrix4f right)
{
	Matrix4f result = M4f();

	for (uint32 row = 0; row < 4; ++row)
	{
		for (uint32 col = 0; col < 4; ++col)
		{
			real32 sum = 0;
			for (uint32 element = 0; element < 4; ++element)
			{
				sum += left[row][element] * right[element][col];
			}
			result[row][col] = sum;
		}
	}
	
	return result;
}

static inline Vector4f operator*(Matrix4f left, Vector4f right)
{
	Vector4f result;

	result.x = left[0][0]*right.x + left[0][1]*right.y + left[0][2]*right.z + left[0][3]*right.w;
	result.y = left[1][0]*right.x + left[1][1]*right.y + left[1][2]*right.z + left[1][3]*right.w;
	result.z = left[2][0]*right.x + left[2][1]*right.y + left[2][2]*right.z + left[2][3]*right.w;
	result.w = left[3][0]*right.x + left[3][1]*right.y + left[3][2]*right.z + left[3][3]*right.w;
	
	if (result.w != 1.0f)
	{
		result = result/result.w;
	}
	
	return result;
}
static inline Vector4f operator*(Matrix4f left, Vector3f right)
{
	return left * V4f(right, 1.0f);
}

//--------------------------------------------------------------------------
// Matrix Member Functions

void Matrix4f::setRow(uint32 row, Vector3f newValues)
{
	if (row > 3)
		return;

	elements[row][0] = newValues.x;
	elements[row][1] = newValues.y;
	elements[row][2] = newValues.z;
}

void Matrix4f::setTranslation(real32 x, real32 y, real32 z)
{
	elements[0][3] = x;
	elements[1][3] = y;
	elements[2][3] = z;
}
void Matrix4f::setTranslation(Vector3f translation)
{
	setTranslation(translation.x, translation.y, translation.z);
}

Vector3f Matrix4f::getTranslation()
{
	Vector3f result = {elements[0][3], elements[1][3], elements[2][3]};
	return result;
}

void Matrix4f::translate(real32 x, real32 y, real32 z)
{
	elements[0][3] += x;
	elements[1][3] += y;
	elements[2][3] += z;
}

void Matrix4f::setScale(real32 x, real32 y, real32 z)
{
	//NOTE(denis): first we need to remove the current scale
	elements[0][0] /= currentScale.x;
	elements[1][1] /= currentScale.y;
	elements[2][2] /= currentScale.z;

	elements[0][0] *= x;
	elements[1][1] *= y;
	elements[2][2] *= z;

	currentScale = V3f(x, y, z);
}
void Matrix4f::setScale(Vector3f newScale)
{
	setScale(newScale.x, newScale.y, newScale.z);
}

Vector3f Matrix4f::getScale()
{
	return currentScale;
}

void Matrix4f::scale(real32 x, real32 y, real32 z)
{
	setScale(currentScale.x * x, currentScale.y * y, currentScale.z * z);
}

//NOTE(denis): these are in radians
void Matrix4f::setRotation(real32 xAngle, real32 yAngle, real32 zAngle)
{
	Vector3f savedTranslation = getTranslation();
	Vector3f savedScale = currentScale;
	currentScale = V3f(1.0f, 1.0f, 1.0f);

	Matrix4f xRotation = getXRotationMatrix(xAngle);
	Matrix4f yRotation = getYRotationMatrix(yAngle);
	Matrix4f zRotation = getZRotationMatrix(zAngle);
	Matrix4f transformation = zRotation * yRotation * xRotation;

	*this = transformation;
	setTranslation(savedTranslation);
	setScale(savedScale);
}

//TODO(denis): probably super inefficient
void Matrix4f::rotate(real32 xAngle, real32 yAngle, real32 zAngle)
{
	Vector3f savedTranslation = getTranslation();
	Vector3f savedScale = currentScale;
	setScale(1.0f, 1.0f, 1.0f);
		
	Matrix4f xRotation = getXRotationMatrix(xAngle);
	Matrix4f yRotation = getYRotationMatrix(yAngle);
	Matrix4f zRotation = getZRotationMatrix(zAngle);
	Matrix4f transformation = zRotation * yRotation * xRotation;
		
	*this = operator*(transformation, *this);
		
	setTranslation(savedTranslation);
	setScale(savedScale);
}

//--------------------------------------------------------------------------
// Rectangle Member Functions

Rect2::Rect2(int32 x, int32 y, int32 width, int32 height)
{
	min = V2(x, y);
	max = V2(x + width, y LOWER_BY height);
}
Rect2::Rect2(Vector2 min, Vector2 max)
{
	this->min = min;
	this->max = max;
}

void Rect2::setX(int32 newX)
{
	int32 width = getWidth();
	min.x = newX;
	max.x = newX + width;
}
void Rect2::setY(int32 newY)
{
	int32 height = getHeight();
	min.y = newY;
	max.y = newY LOWER_BY height;
}
void Rect2::setPos(Vector2 newPos)
{
	setX(newPos.x);
	setY(newPos.y);
}


Rect2f::Rect2f(real32 x, real32 y, real32 width, real32 height)
{
	min = V2f(x, y);
	max = V2f(x + width, y LOWER_BY height);
}
Rect2f::Rect2f(Vector2f min, Vector2f max)
{
	this->min = min;
	this->max = max;
}

void Rect2f::setX(real32 newX)
{
	real32 width = getWidth();
	min.x = newX;
	max.x = newX + width;
}
void Rect2f::setY(real32 newY)
{
	real32 height = getHeight();
	min.y = newY;
	max.y = newY LOWER_BY height;
}
void Rect2f::setPos(Vector2f newPos)
{
	setX(newPos.x);
	setY(newPos.y);
}

//--------------------------------------------------------------------------
// General Function Definitions

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

real32 slope(Vector2 point1, Vector2 point2)
{
	return (real32)(point2.y - point1.y) / (real32)(point2.x - point1.x);
}
real32 inverseSlope(Vector2 point1, Vector2 point2)
{
	return (real32)(point2.x - point1.x) / (real32)(point2.y - point1.y);
}

static inline real32 magnitude(Vector2 v)
{
	return sqrt((real32)(v.x*v.x + v.y*v.y));
}
static inline real32 magnitude(Vector3f v)
{
	return sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline Vector3f normalize(Vector3f vector)
{
	Vector3f result = {};
	
	real32 vectorMagnitude = magnitude(vector);
	if (vectorMagnitude > 0)
		result = vector/vectorMagnitude;
	
	return result;
}

static inline Vector3f cross(Vector3f v1, Vector3f v2)
{
	Vector3f result;
	
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;

	return result;
}
static inline Vector4f cross(Vector4f v1, Vector4f v2)
{
	Vector3f reducedVector1 = V3f(v1.x, v1.y, v1.z);
	Vector3f reducedVector2 = V3f(v2.x, v2.y, v2.z);

	Vector4f result = V4f(cross(reducedVector1, reducedVector2), 1.0f);
	return result;
}

static inline real32 dot(Vector3f v1, Vector3f v2)
{
	real32 result = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return result;
}

#endif
