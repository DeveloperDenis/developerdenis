#if !defined(PLATFORM_LAYER_H_)
#define PLATFORM_LAYER_H_

#include "denis_types.h"
#include "denis_math.h"
#include "keyboard.h"

struct Mouse
{
    v2f pos;
	
    bool leftPressed;
    bool leftWasPressed;
    v2f leftClickStartPos;
	
    bool rightPressed;
    bool rightWasPressed;
    v2f rightClickStartPos;
};

struct Touch
{
    //TODO(denis): eventually make this hold 10 different points?
    u32 numActivePoints;
    v2f points[5];
};

struct Pen
{
    u32 pressure;
	v2f pos;
    bool usingEraser;
};

struct Input
{
    Pen pen;
    Touch touch;
    Mouse mouse;
	Keyboard keyboard;
};

#define PLATFORM_MEDIA_PLAY_FILE(name) void (name)(char* fileName)
typedef PLATFORM_MEDIA_PLAY_FILE(*MediaPlayFilePtr);

enum MediaPlayerState
{
	MEDIA_INVALID,
	MEDIA_PLAYING,
	MEDIA_PAUSED,
	MEDIA_FINISHED
};
#define PLATFORM_MEDIA_GET_STATE(name) MediaPlayerState (name)()
typedef PLATFORM_MEDIA_GET_STATE(*MediaGetStatePtr);

enum CursorType
{
	CURSOR_NONE,
	CURSOR_POINTER,
	CURSOR_HAND,
	CURSOR_TEXT
};
#define PLATFORM_CHANGE_CURSOR(name) void(name)(CursorType newType)
typedef PLATFORM_CHANGE_CURSOR(*ChangeCursorPtr);

//NOTE(denis): platform functions for the app to use
struct Platform
{
	//TODO(denis): add things like thread creation, etc.
	
	ChangeCursorPtr changeCursor;
	
	MediaPlayFilePtr mediaPlayFile;
	MediaGetStatePtr mediaGetState;
};

struct Memory
{
	// used for persistent storage
	u32 size;
	void* mem;
	
	// "transient" memory, cleared at the end of each frame
	u32 tempSize;
	void* tempMem;
};

//TODO(denis): change this to just "APP_INIT"?
#define APP_INIT_CALL(name) void (name)(Platform platform, Memory memory, Bitmap* screen)
#define APP_UPDATE_CALL(name) void (name)(Platform platform, Memory memory, Bitmap* screen, Input* input, f32 t)

#include "project_settings.h"

#if defined(DENIS_WIN32)

#define exportDLL extern "C" __declspec(dllexport)

#elif defined(DENIS_LINUX)

#define exportDLL
//TODO(denis): the linux layer does not currently support dynamic loading of code
#include "linux_layer.cpp"

#endif

#endif
