#if !defined(PLATFORM_LAYER_H_)
#define PLATFORM_LAYER_H_

#include "denis_types.h"
#include "denis_math.h"

//TODO(denis): this doesn't take into account that the pitch may be different from the width of
// the image
//TODO(denis): maybe this should be defined in denis_drawing.h?
struct Bitmap
{
    uint32* pixels;

    uint32 width;
    uint32 height;
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
    Vector2 pos;
	
    bool leftPressed;
    bool leftWasPressed;
    Vector2 leftClickStartPos;

    bool rightPressed;
    bool rightWasPressed;
    Vector2 rightClickStartPos;
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
    Controller controller;
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
#if defined(DENIS_WIN32)
#include "main.h"
#elif defined(DENIS_LINUX)
#include "../code/main.h"
#endif

#define exportDLL extern "C" __declspec(dllexport)
#define MAIN_UPDATE_CALL(name) void (name)(Memory* memory, Bitmap* screen, Input* input)

#endif
