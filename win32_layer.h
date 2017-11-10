#ifndef WIN32_LAYER_H_
#define WIN32_LAYER_H_

#include "denis_types.h"
#include "denis_math.h"

//TODO(denis): this doesn't take into account that the pitch may be different from the width of
// the image
struct Bitmap
{
	uint32* pixels;

	uint32 width;
	uint32 height;
};

//TODO(denis): lump Controller, Mouse, Touch input, and Pen input into a single "Input" struct?
// and we only pass the input struct into the mainUpdateCall?
struct Controller
{
	bool upPressed;
	bool downPressed;
	bool leftPressed;
	bool rightPressed;
};

struct Mouse
{
	int32 x;
	int32 y;

	bool leftButtonPressed;
};

struct Touch
{
	//TODO(denis): eventually make this hold 10 different points?
	uint32 numActivePoints;
	Vector2 points[5];
};

struct Pen
{
	uint32 pressure;
	uint32 x;
	uint32 y;
	bool usingEraser;
};

struct Input
{
	Pen pen;
	Touch touch;
	Mouse mouse;
};

//TODO(denis): think of a better way to do this
//NOTE: user of this header must provide a main.h file that contains the following:
// - declaration of struct "Memory" that is used to store data between frames
// - struct "STATIC_SETTINGS" that has members
//        -> static char* WINDOW_TITLE
//        -> static uint32 WINDOW_WIDTH
//        -> static uint32 WINDOW_HEIGHT
//        -> static bool WINDOW_RESIZABLE
//        -> static char* DLL_FILE_NAME
//        -> static uint32 FPS_TARGET
#include "main.h"

#define MAIN_UPDATE_CALL(name) void (name)(Memory* memory, Bitmap* screen, Input* input)
#define exportDLL extern "C" __declspec(dllexport)

#endif
