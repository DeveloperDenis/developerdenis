#if !defined(PLATFORM_LAYER_H_)
#define PLATFORM_LAYER_H_

#include "denis_types.h"
#include "denis_math.h"

//TODO(denis): this doesn't take into account that the pitch may be different from the width of
// the image
//TODO(denis): maybe this should be defined in denis_drawing.h?
struct Bitmap
{
    u32* pixels;

    u32 width;
    u32 height;
};

struct Controller
{
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;

    bool actionPressed;
};

struct Mouse
{
    v2 pos;
	
    bool leftPressed;
    bool leftWasPressed;
    v2 leftClickStartPos;

    bool rightPressed;
    bool rightWasPressed;
    v2 rightClickStartPos;
};

struct Touch
{
    //TODO(denis): eventually make this hold 10 different points?
    u32 numActivePoints;
    v2 points[5];
};

struct Pen
{
    u32 pressure;
	v2 pos;
    bool usingEraser;
};

struct Input
{
    Pen pen;
    Touch touch;
    Mouse mouse;
    Controller controller;
};

//NOTE(denis): this must be defined by the user program
struct Memory;

#define APP_INIT_CALL(name) void (name)(Memory* memory, Bitmap* screen)
#define APP_UPDATE_CALL(name) void (name)(Memory* memory, Bitmap* screen, Input* input)

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
#if defined(DENIS_WIN32)

#include "../src/main.h"
#define exportDLL extern "C" __declspec(dllexport)

#elif defined(DENIS_LINUX)

#include "../src/main.h"
#define exportDLL
#include "linux_layer.cpp"

#endif

#endif
