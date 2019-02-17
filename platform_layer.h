#if !defined(PLATFORM_LAYER_H_)
#define PLATFORM_LAYER_H_

#include "denis_types.h"
#include "denis_math.h"

//TODO(denis): maybe this should be defined in denis_drawing.h?
struct Bitmap
{
    u32* pixels;

	u32 width;
    u32 height;

	// number of bytes in a row
	u32 stride;
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

#define PLATFORM_MEDIA_PLAY_FILE(name) void (name)(char* fileName)
typedef PLATFORM_MEDIA_PLAY_FILE(*mediaPlayFilePtr);

enum MediaPlayerState
{
	MEDIA_INVALID,
	MEDIA_PLAYING,
	MEDIA_PAUSED,
	MEDIA_FINISHED
};
#define PLATFORM_MEDIA_GET_STATE(name) MediaPlayerState (name)()
typedef PLATFORM_MEDIA_GET_STATE(*mediaGetStatePtr);

//NOTE(denis): platform functions for the app to use
struct Platform
{
	//TODO(denis): add things like thread creation, etc.
	
	mediaPlayFilePtr mediaPlayFile;
	mediaGetStatePtr mediaGetState;
};

//NOTE(denis): this must be defined by the user program
struct Memory;

//TODO(denis): change this to just "APP_INIT"?
#define APP_INIT_CALL(name) void (name)(Platform platform, Memory* mem, Bitmap* screen)
#define APP_UPDATE_CALL(name) void (name)(Platform platform, Memory* mem, Bitmap* screen, Input* input, f32 t)

#include "project_settings.h"

#if defined(DENIS_WIN32)

#define exportDLL extern "C" __declspec(dllexport)

#elif defined(DENIS_LINUX)

#define exportDLL
//TODO(denis): the linux layer does not currently support dynamic loading of code
#include "linux_layer.cpp"

#endif

#endif
