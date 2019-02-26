//----------------------------------------------------
//	  denis_math.h
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

#define CLAMP_MIN(value, min) ((value) < (min) ? min : value)
#define CLAMP_MAX(value, max) ((value) > (max) ? max : value)
#define CLAMP_RANGE(value, min, max) ((value) > (min) ? MIN(value, max) : (min))

#define DEGREE_TO_RAD(angle) ((f32)(angle)*(f32)M_PI / 180.0f)

//---------------------------------------------------------------------------
// Type Declarations:
union v2i;
union v2f;
union v3i;
union v3f;
union v4f;

struct Matrix4f;

struct Rect2;
struct Rect2f;

//----------------------------------------------------------------------------
// Functions Declarations:

static inline bool pointInRect(v2i point, Rect2 rect);

static inline bool pointInCircle(v2i point, v2i pos, s32 radius);
static inline bool pointInCircle(v2f point, v2f pos, s32 radius);

static inline f32 slope(v2i point1, v2i point2);
static inline f32 inverseSlope(v2i point1, v2i point2);

static inline f32 magnitude(v2i v);
static inline f32 magnitude(v3f v);

static inline v2i normalize(v2i v);
static inline v3f normalize(v3f vector);

static inline f32 dot(v3f v1, v3f v2);

static inline v3i cross(v3i v1, v3i v2);
static inline v3f cross(v3f v1, v3f v2);
static inline v4f cross(v4f v1, v4f v2);


//---------------------------------------------------------------------------
// Vector types:

union v2i
{
	v2i();
	v2i(s32 x, s32 y);
	v2i(v2f v);
	
	struct
	{
		s32 x;
		s32 y;
	};
	struct
	{
		s32 w;
		s32 h;
	};
	s32 e[2];

	v2i& operator+=(v2i& right)
	{
		this->x = this->x + right.x;
		this->y = this->y + right.y;
		return *this;
	}
};

union v2f
{
	v2f();
	v2f(f32 x, f32 y);
	v2f(v2i v);
	
	struct
	{
		f32 x;
		f32 y;
	};
	struct
	{
		f32 w;
		f32 h;
	};
	f32 e[2];

	v2f& operator+=(v2f& right)
	{
		this->x = this->x + right.x;
		this->y = this->y + right.y;
		return *this;
	}
};

union v3i
{
	v3i();
	v3i(s32 x, s32 y, s32 z);
	v3i(v2i v, s32 z);

	struct
	{
		s32 x;
		s32 y;
		s32 z;
	};
	struct
	{
		s32 r;
		s32 g;
		s32 b;
	};
	struct
	{
		v2i xy;
		s32 filler;
	};
	s32 e[3];

	s32& operator[](u32 index)
	{
		return e[index];
	}
};

union v3f
{
	v3f();
	v3f(f32 x, f32 y, f32 z);
	v3f(v2f, f32);
	v3f(v4f v);
	
	struct
	{
		f32 x;
		f32 y;
		f32 z;
	};
	struct
	{
		f32 r;
		f32 g;
		f32 b;
	};
	struct
	{
		v2f xy;
		f32 _unused;
	};
	f32 e[3];

	f32& operator[](u32 index)
	{
		return e[index];
	}

	v3f& operator+=(v3f right)
	{
		this->x = this->x + right.x;
		this->y = this->y + right.y;
		this->z = this->z + right.z;
		return *this;
	}
	v3f& operator-=(v3f right)
	{
		this->x = this->x - right.x;
		this->y = this->y - right.y;
		this->z = this->z - right.z;
		return *this;
	}
};

union v4f
{
	v4f();
	v4f(f32 x, f32 y, f32 z);
	v4f(f32 x, f32 y, f32 z, f32 w);
	v4f(v3f v, f32 w);
	v4f(v3f v);
	
	struct
	{
		f32 x;
		f32 y;
		f32 z;
		f32 w;
	};
	struct
	{
		f32 r;
		f32 g;
		f32 b;
		f32 a;
	};
	struct
	{
		v3f xyz;
		f32 _unused;
	};
	f32 e[4];

	f32& operator[](u32 index)
	{
		return e[index];
	}
};

//---------------------------------------------------------------------------
// Matrix types:
// 4 x 4 transformation matrix in column-major style

struct Matrix4f
{
	f32 elements[4][4];

	v3f currentScale;

	f32* operator[](u32 index)
	{
		return elements[index];
	}

	void setRow(u32 row, v3f newValues);
	void setRow(u32 row, v4f newValues);

	void setTranslation(f32 x, f32 y, f32 z);
	void setTranslation(v3f translation);

	v3f getTranslation();

	void translate(f32 x, f32 y, f32 z);
	
	void setScale(f32 x, f32 y, f32 z);
	void setScale(v3f newScale);

	v3f getScale();
	
	void scale(f32 x, f32 y, f32 z);
	
	//NOTE(denis): these are in radians
	void setRotation(f32 xAngle, f32 yAngle, f32 zAngle);

	void rotate(f32 xAngle, f32 yAngle, f32 zAngle);
};

//---------------------------------------------------------------------------
// Rectangle types:

//TODO(denis): should my rectangles have a centre origin instead of top-left?
//TODO(denis): also, maybe put rectangles into a "denis_geometry.h" file once I get some more geometry code
//TODO(denis): put all types together at the top?
// Rectangle types
struct Rect2
{
	v2i min; // min is top-left
	v2i max; // max is bottom-right

	Rect2(s32 x, s32 y, s32 width, s32 height);
	Rect2(v2i min, v2i max);
	
	s32 getLeft() { return min.x; };
	s32 getRight() { return max.x; };
	s32 getTop() { return min.y; };
	s32 getBottom() { return max.y; };

	s32 getWidth() { return max.x - min.x; };
	s32 getHeight() { return ABS_VALUE(max.y - min.y); };

	void moveLeft(s32 amount) { setX(min.x - amount); }
	void moveRight(s32 amount) { setX(min.x + amount); }
	void moveUp(s32 amount) { setY(min.y RAISE_BY amount); }
	void moveDown(s32 amount) { setY(min.y LOWER_BY amount); }

	void setX(s32 newX);
	void setY(s32 newY);
	void setPos(v2i newPos);
};

struct Rect2f
{
	v2f min;
	v2f max;

	Rect2f(f32 x, f32 y, f32 width, f32 height);
	Rect2f(v2f min, v2f max);
	
	f32 getLeft() { return min.x; };
	f32 getRight() { return max.x; };
	f32 getTop() { return min.y; };
	f32 getBottom() { return max.y; };

	f32 getWidth() { return max.x - min.x; };
	f32 getHeight() { return ABS_VALUE(max.y - min.y); };

	void setX(f32 newX);
	void setY(f32 newY);
	void setPos(v2f newPos);
};

//---------------------------------------------------------------------------
// Vector Constructors

v2i::v2i()
{
	this->x = x;
	this->y = y;
}
v2i::v2i(s32 x, s32 y)
{
	this->x = x;
	this->y = y;
}
v2i::v2i(v2f v)
{
	this->x = (s32)v.x;
	this->y = (s32)v.y;
}

v2f::v2f()
{
	this->x = 0.0f;
	this->y = 0.0f;
}
v2f::v2f(f32 x, f32 y)
{
	this->x = x;
	this->y = y;
}
v2f::v2f(v2i v)
{
	this->x = (f32)v.x;
	this->y = (f32)v.y;
}

v3i::v3i()
{
	this->x = 0;
	this->y = 0;
	this->z = 0;
}
v3i::v3i(s32 x, s32 y, s32 z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
v3i::v3i(v2i v, s32 z)
{
	this->x = v.x;
	this->y = v.y;
	this->z = z;
}

v3f::v3f()
{
	this->x = 0.0f;
	this->y = 0.0f;
	this->z = 0.0f;
}
v3f::v3f(f32 x, f32 y, f32 z)
{
	this->x = x;
	this->y = y;
	this->z = z;
}
v3f::v3f(v2f v, f32 z)
{
	this->x = v.x;
	this->y = v.y;
	this->z = z;
}
v3f::v3f(v4f v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
}

v4f::v4f() : x(0.0f), y(0.0f), z(0.0f), w(0.0f)
{
}
v4f::v4f(f32 x, f32 y, f32 z)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = 1.0f;
}
v4f::v4f(f32 x, f32 y, f32 z, f32 w)
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}
v4f::v4f(v3f v, f32 w)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = w;
}
v4f::v4f(v3f v)
{
	this->x = v.x;
	this->y = v.y;
	this->z = v.z;
	this->w = 1.0f;
}


//---------------------------------------------------------------------------
// Vector operator overloads

v2i operator+(v2i left, v2i right)
{
	v2i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
v2i operator-(v2i left, v2i right)
{
	v2i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
v2i operator*(v2i left, s32 right)
{
	v2i result;
	result.x = left.x * right;
	result.y = left.y * right;
	return result;
}
v2i operator*(v2i left, u32 right)
{
	return left * (s32)right;
}
v2i operator*(v2i left, f32 right)
{
	v2i result;
	result.x = (s32)(left.x * right);
	result.y = (s32)(left.y * right);
	return result;
}
v2i operator/(v2i left, s32 right)
{
	v2i result;
	result.x = left.x / right;
	result.y = left.y / right;
	return result;
}
v2i operator/(v2i left, f32 right)
{
	v2i result;
	result.x = (s32)(left.x / right);
	result.y = (s32)(left.y / right);
	return result;
}

v2f operator+(v2f left, v2f right)
{
	v2f result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	return result;
}
v2f operator-(v2f left, v2f right)
{
	v2f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	return result;
}
v2f operator*(v2f left, f32 right)
{
	v2f result;
	result.x = left.x * right;
	result.y = left.y * right;
	return result;
}
v2f operator/(v2f left, f32 right)
{
	v2f result;
	result.x = left.x / right;
	result.y = left.y / right;
	return result;
}

static inline v3i operator+(v3i left, v3i right)
{
	v3i result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	return result;
}
static inline v3i operator-(v3i left, v3i right)
{
	v3i result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
static inline v3i operator*(v3i left, s32 right)
{
	v3i result;
	result.x = left.x * right;
	result.y = left.y * right;
	result.z = left.z * right;
	return result;
}
static inline v3i operator/(v3i left, s32 right)
{
	v3i result;
	result.x = left.x/right;
	result.y = left.y/right;
	result.z = left.z/right;
	return result;
}

static inline v3f operator-(v3f& v)
{
	v3f result;
	result.x = -v.x;
	result.y = -v.y;
	result.z = -v.z;		
	return result;
}
static inline v3f operator+(v3f left, v3f right)
{
	v3f result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;	
	return result;
}
static inline v3f operator-(v3f left, v3f right)
{
	v3f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	return result;
}
static inline v3f operator*(s32 left, v3f right)
{
	v3f result;
	result.x = right.x * left;
	result.y = right.y * left;
	result.z = right.z * left;
	return result;
}
static inline v3f operator*(v3f left, s32 right)
{
	return right*left;
}
static inline v3f operator*(v3f left, f32 right)
{
	v3f result;
	result.x = left.x * right;
	result.y = left.y * right;
	result.z = left.z * right;
	return result;
}
static inline v3f operator*(f32 left, v3f right)
{
	return right*left;
}
static inline v3f operator/(v3f left, f32 right)
{
	v3f result;
	result.x = left.x / right;
	result.y = left.y / right;
	result.z = left.z / right;
	return result;
}
static inline bool operator!=(v3f left, v3f right)
{
	return left.x != right.x || left.y != right.y || left.z != right.z;
}

static inline v4f operator+(v4f left, v4f right)
{
	v4f result;
	result.x = left.x + right.x;
	result.y = left.y + right.y;
	result.z = left.z + right.z;
	result.w = left.w + right.w;
	return result;
}
static inline v4f operator-(v4f left, v4f right)
{
	v4f result;
	result.x = left.x - right.x;
	result.y = left.y - right.y;
	result.z = left.z - right.z;
	result.w = left.w - right.w;	
	return result;
}
static inline v4f operator/(v4f left, f32 scalar)
{
	v4f result;
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
	result.currentScale = v3f(1.0f, 1.0f, 1.0f);

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

static inline Matrix4f getXRotationMatrix(f32 xAngle)
{
	Matrix4f xRotation = getIdentityMatrix4f();
	xRotation[1][1] = (f32)cos(xAngle);
	xRotation[1][2] = (f32)-sin(xAngle);
	xRotation[2][1] = (f32)sin(xAngle);
	xRotation[2][2] = (f32)cos(xAngle);

	return xRotation;
}
static inline Matrix4f getYRotationMatrix(f32 yAngle)
{
	Matrix4f yRotation = getIdentityMatrix4f();
	yRotation[0][0] = (f32)cos(yAngle);
	yRotation[0][2] = (f32)sin(yAngle);
	yRotation[2][0] = (f32)-sin(yAngle);
	yRotation[2][2] = (f32)cos(yAngle);

	return yRotation;
}
static inline Matrix4f getZRotationMatrix(f32 zAngle)
{
	Matrix4f zRotation = getIdentityMatrix4f();
	zRotation[0][0] = (f32)cos(zAngle);
	zRotation[0][1] = (f32)-sin(zAngle);
	zRotation[1][0] = (f32)sin(zAngle);
	zRotation[1][1] = (f32)cos(zAngle);

	return zRotation;
}

//--------------------------------------------------------------------------
// Matrix Operator Overloads

//TODO(denis): not efficient at all!
static inline Matrix4f operator*(Matrix4f left, Matrix4f right)
{
	Matrix4f result = M4f();

	for (u32 row = 0; row < 4; ++row)
	{
		for (u32 col = 0; col < 4; ++col)
		{
			f32 sum = 0;
			for (u32 element = 0; element < 4; ++element)
			{
				sum += left[row][element] * right[element][col];
			}
			result[row][col] = sum;
		}
	}
	
	return result;
}

static inline v4f operator*(Matrix4f left, v4f right)
{
	v4f result;

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
static inline v3f operator*(Matrix4f left, v3f right)
{
	v4f result = left * v4f(right, 1.0f);
	return result.xyz;
}

//--------------------------------------------------------------------------
// Matrix Member Functions

void Matrix4f::setRow(u32 row, v3f newValues)
{
	if (row > 3)
		return;

	elements[row][0] = newValues.x;
	elements[row][1] = newValues.y;
	elements[row][2] = newValues.z;
}
void Matrix4f::setRow(u32 row, v4f newValues)
{
	if (row > 3)
		return;

	elements[row][0] = newValues.x;
	elements[row][1] = newValues.y;
	elements[row][2] = newValues.z;
	elements[row][3] = newValues.w;
}

void Matrix4f::setTranslation(f32 x, f32 y, f32 z)
{
	elements[0][3] = x;
	elements[1][3] = y;
	elements[2][3] = z;
}
void Matrix4f::setTranslation(v3f translation)
{
	setTranslation(translation.x, translation.y, translation.z);
}

v3f Matrix4f::getTranslation()
{
	v3f result(elements[0][3], elements[1][3], elements[2][3]);
	return result;
}

void Matrix4f::translate(f32 x, f32 y, f32 z)
{
	elements[0][3] += x;
	elements[1][3] += y;
	elements[2][3] += z;
}

void Matrix4f::setScale(f32 x, f32 y, f32 z)
{
	//NOTE(denis): first we need to remove the current scale
	elements[0][0] /= currentScale.x;
	elements[1][1] /= currentScale.y;
	elements[2][2] /= currentScale.z;

	elements[0][0] *= x;
	elements[1][1] *= y;
	elements[2][2] *= z;

	currentScale = v3f(x, y, z);
}
void Matrix4f::setScale(v3f newScale)
{
	setScale(newScale.x, newScale.y, newScale.z);
}

v3f Matrix4f::getScale()
{
	return currentScale;
}

void Matrix4f::scale(f32 x, f32 y, f32 z)
{
	setScale(currentScale.x * x, currentScale.y * y, currentScale.z * z);
}

//NOTE(denis): these are in radians
void Matrix4f::setRotation(f32 xAngle, f32 yAngle, f32 zAngle)
{
	v3f savedTranslation = getTranslation();
	v3f savedScale = currentScale;
	currentScale = v3f(1.0f, 1.0f, 1.0f);

	Matrix4f xRotation = getXRotationMatrix(xAngle);
	Matrix4f yRotation = getYRotationMatrix(yAngle);
	Matrix4f zRotation = getZRotationMatrix(zAngle);
	Matrix4f transformation = zRotation * yRotation * xRotation;

	for (u32 i = 0; i < 4; ++i)
	{
		for (u32 j = 0; j < 4; ++j)
		{
			elements[i][j] = transformation.elements[i][j];
		}
	}
	
	setTranslation(savedTranslation);
	setScale(savedScale);
}

//TODO(denis): probably super inefficient
void Matrix4f::rotate(f32 xAngle, f32 yAngle, f32 zAngle)
{
	v3f savedTranslation = getTranslation();
	v3f savedScale = currentScale;
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

Rect2::Rect2(s32 x, s32 y, s32 width, s32 height)
{
    min = v2i(x, y);
    max = v2i(x + width, y LOWER_BY height);
}
Rect2::Rect2(v2i min, v2i max)
{
	this->min = min;
	this->max = max;
}

void Rect2::setX(s32 newX)
{
	s32 width = getWidth();
	min.x = newX;
	max.x = newX + width;
}
void Rect2::setY(s32 newY)
{
	s32 height = getHeight();
	min.y = newY;
	max.y = newY LOWER_BY height;
}
void Rect2::setPos(v2i newPos)
{
	setX(newPos.x);
	setY(newPos.y);
}


Rect2f::Rect2f(f32 x, f32 y, f32 width, f32 height)
{
	min = v2f(x, y);
	max = v2f(x + width, y LOWER_BY height);
}
Rect2f::Rect2f(v2f min, v2f max)
{
	this->min = min;
	this->max = max;
}

void Rect2f::setX(f32 newX)
{
	f32 width = getWidth();
	min.x = newX;
	max.x = newX + width;
}
void Rect2f::setY(f32 newY)
{
	f32 height = getHeight();
	min.y = newY;
	max.y = newY LOWER_BY height;
}
void Rect2f::setPos(v2f newPos)
{
	setX(newPos.x);
	setY(newPos.y);
}

//--------------------------------------------------------------------------
// General Function Definitions

static inline v3f clampV3f(v3f v, f32 min, f32 max)
{
	v3f result = v;
	result.x = CLAMP_RANGE(v.x, min, max);
	result.y = CLAMP_RANGE(v.y, min, max);
	result.z = CLAMP_RANGE(v.z, min, max);
	return result;
}

#if defined(UP_POSITIVE_Y)
static inline bool pointInRect(v2 point, Rect2 rect)
{
	return point.x > rect.getLeft() && point.x < rect.getRight() &&
		point.y < rect.getTop() && point.y > rect.getBottom();
}
#else
static inline bool pointInRect(v2i point, Rect2 rect)
{
	return point.x > rect.getLeft() && point.x < rect.getRight() &&
		point.y > rect.getTop() && point.y < rect.getBottom();
}
#endif

//TODO(denis): for now this only checks the smallest rect that contains the given circle
static inline bool pointInCircle(v2i point, v2i pos, s32 radius)
{
	return point.x > pos.x - radius && point.x < pos.x + radius &&
		point.y > pos.y - radius && point.y < pos.y + radius;
}
static inline bool pointInCircle(v2f point, v2f pos, s32 radius)
{
	return point.x > pos.x - radius && point.x < pos.x + radius &&
		point.y > pos.y - radius && point.y < pos.y + radius;	
}

f32 slope(v2i point1, v2i point2)
{
	return (f32)(point2.y - point1.y) / (f32)(point2.x - point1.x);
}
f32 inverseSlope(v2i point1, v2i point2)
{
	return (f32)(point2.x - point1.x) / (f32)(point2.y - point1.y);
}

static inline f32 magnitude(v2i v)
{
	return (f32)sqrt((f32)(v.x*v.x + v.y*v.y));
}
static inline f32 magnitude(v3f v)
{
	return (f32)sqrt(v.x*v.x + v.y*v.y + v.z*v.z);
}

static inline v2i normalize(v2i v)
{
	v2i result = {};

	f32 vectorMagnitude = magnitude(v);
	if (vectorMagnitude > 0.0f)
		result = v/vectorMagnitude;
	
	return result;
}
static inline v3f normalize(v3f vector)
{
	v3f result = {};
	
	f32 vectorMagnitude = magnitude(vector);
	if (vectorMagnitude > 0)
		result = vector/vectorMagnitude;
	
	return result;
}

static inline v3i cross(v3i v1, v3i v2)
{
	v3i result;

	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;

	return result;
}
static inline v3f cross(v3f v1, v3f v2)
{
	v3f result;
	
	result.x = v1.y*v2.z - v1.z*v2.y;
	result.y = v1.z*v2.x - v1.x*v2.z;
	result.z = v1.x*v2.y - v1.y*v2.x;

	return result;
}
static inline v4f cross(v4f v1, v4f v2)
{
	v3f reducedVector1(v1.x, v1.y, v1.z);
	v3f reducedVector2(v2.x, v2.y, v2.z);

	v4f result(cross(reducedVector1, reducedVector2), 1.0f);
	return result;
}

static inline f32 dot(v3f v1, v3f v2)
{
	f32 result = v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	return result;
}

static inline v3f hadamard(v3f v1, v3f v2)
{
	v3f result(v1.x*v2.x, v1.y*v2.y, v1.z*v2.z);
	return result;
}

#endif
