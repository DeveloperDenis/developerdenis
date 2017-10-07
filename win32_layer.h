#ifndef WIN32_LAYER_H_
#define WIN32_LAYER_H_

#include "denis_types.h"

struct Bitmap
{
	void* pixels;
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
};

struct Touch
{
	
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
};

//TODO(denis): think of a better way to do this
//NOTE: user of this header must provide a main.h file that contains the following:
// - declaration of struct "Memory" that is used to store data between frames
// - struct "STATIC_SETTINGS" that has members
//        -> static char* WINDOW_TITLE
//        -> static char* DLL_FILE_NAME
//        -> static uint32 FPS_TARGET
#include "main.h"

#define MAIN_UPDATE_CALL(name) void (name)(Memory* memory, Bitmap* screen, Input* input)
#define exportDLL extern "C" __declspec(dllexport)

#endif
