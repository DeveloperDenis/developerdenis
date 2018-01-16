//TODO(denis): I'm using Xlib right now, but maybe I should switch to using XCB instead since it is said to be much faster?
// though I am not sure if that will matter for my uses of it or not
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/keysymdef.h>

#include <time.h>
#include <stdio.h>

#include "denis_types.h"
#include "platform_layer.h"
#include "denis_drawing.h"

typedef MAIN_UPDATE_CALL(*mainUpdateCallPtr);
extern MAIN_UPDATE_CALL(mainUpdateCall);

#define WINDOW_TITLE "Testing Window"

#define MAX_TIME_PER_FRAME_NS 16666666

//NOTE(denis): this only cares about nano second differences, and it assumes that the differences in nano seconds between two
// times will be small (less than 1 second)
static inline long getElapsedTimeNs(timespec* startTime, timespec* currTime)
{
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, currTime);
    long nanoDiff = currTime->tv_nsec - startTime->tv_nsec;
    if (nanoDiff < 0)
    {
	long diff1 = currTime->tv_nsec;
	long diff2 = 1000000000 - startTime->tv_nsec;
	nanoDiff = diff1 + diff2;
    }

    return nanoDiff;
}

int main(int argc, char** argv)
{
    Display* display = XOpenDisplay(0);
    uint32 screenNum = DefaultScreen(display);
    int rootWindow = DefaultRootWindow(display);

    int screenDepth = 24;
    XVisualInfo visualInfo = {};
    if (!XMatchVisualInfo(display, screenNum, screenDepth, TrueColor, &visualInfo))
    {
	printf("Error matching visual info\n");
	exit(1);
    }

    XSetWindowAttributes windowAttributes = {};
    //TODO(denis): need to add more here as I add more event processing
    windowAttributes.event_mask = StructureNotifyMask|ButtonPressMask|KeyPressMask|KeyReleaseMask;
    windowAttributes.colormap = XCreateColormap(display, rootWindow, visualInfo.visual, AllocNone);

    unsigned long attributeMask = CWBackPixel|CWColormap|CWEventMask;

    Window window = XCreateWindow(display, rootWindow, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0,
				  visualInfo.depth, InputOutput, visualInfo.visual, attributeMask, &windowAttributes);
    if (!window)
    {
	printf("Error creating window\n");
	exit(1);
    }

    //TODO(denis): this doesn't seem to do anything?
    XSizeHints sizeHints = {};
    sizeHints.min_width = WINDOW_WIDTH;
    sizeHints.min_height = WINDOW_HEIGHT;
    XSetWMNormalHints(display, window, &sizeHints);
    
    XStoreName(display, window, WINDOW_TITLE);
    XMapRaised(display, window);

    XFlush(display);

    int32 pixelBits = 32;
    uint32 pixelBytes = pixelBits/8;
    uint32 backBufferSize = WINDOW_WIDTH*WINDOW_HEIGHT*pixelBytes;
    uint32* pixels = (uint32*)malloc(backBufferSize);
    
    XImage* backBuffer = XCreateImage(display, visualInfo.visual, visualInfo.depth, ZPixmap, 0, (char*)pixels,
				      WINDOW_WIDTH, WINDOW_HEIGHT, pixelBits, 0);

    GC graphicsContext = DefaultGC(display, screenNum);

    Bitmap screenBitmap = {};
    screenBitmap.pixels = pixels;
    screenBitmap.width = WINDOW_WIDTH;
    screenBitmap.height = WINDOW_HEIGHT;

    void* memory = calloc(GIGABYTE(1), 1);

    Input input = {};

    timespec clockTime;
    //TODO(denis): can use CLOCK_REALTIME instead?
    //NOTE(denis): CLOCK_REALTIME is affected by the user manually changing the system time
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &clockTime);
    
    bool running = true;
    while(running)
    {
	XEvent event;
	while(XPending(display))
	{
	    XNextEvent(display, &event);

	    switch(event.type)
	    {
		case KeyPress:
		{
#if 0
		    char keyText[255];
		    KeySym key;
		    XLookupString(&event.xkey, keyText, 255, &key, 0);

		    
		    if (keyText[0] == 'q')
		    {
			running = false;
		    }
#endif
		    
		    //TODO(denis): should I use keyText instead of these calls? might be faster?
		    if (event.xkey.keycode == XKeysymToKeycode(display, XK_Left))
		    {
			input.controller.leftPressed = true;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Right))
		    {
			input.controller.rightPressed = true;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Up))
		    {
			input.controller.upPressed = true;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Down))
		    {
			input.controller.downPressed = true;
		    }

		    if (event.xkey.keycode == XKeysymToKeycode(display, XK_space))
		    {
			input.controller.actionPressed = true;
		    }
		} break;

		case KeyRelease:
		{
		    if (event.xkey.keycode == XKeysymToKeycode(display, XK_Left))
		    {
			input.controller.leftPressed = false;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Right))
		    {
			input.controller.rightPressed = false;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Up))
		    {
			input.controller.upPressed = false;
		    }
		    else if (event.xkey.keycode == XKeysymToKeycode(display, XK_Down))
		    {
			input.controller.downPressed = false;
		    }
		    
		    if (event.xkey.keycode == XKeysymToKeycode(display, XK_space))
		    {
			input.controller.actionPressed = false;
		    }
		} break;

		case DestroyNotify:
		{
		    XDestroyWindowEvent* e = (XDestroyWindowEvent*)&event;
		    if (e->window == window)
			running = false;
		} break;
	    }
	}

	mainUpdateCall((Memory*)memory, &screenBitmap, &input);
	
	XPutImage(display, window, graphicsContext, backBuffer, 0, 0, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//TODO(denis): might need to do XFlush(display); here every frame to be sure?

	timespec endTime;
	long nanoDiff = getElapsedTimeNs(&clockTime, &endTime);

	//TODO(denis): busy loop!
	while (nanoDiff < MAX_TIME_PER_FRAME_NS)
	{
	    nanoDiff = getElapsedTimeNs(&clockTime, &endTime);
	}

#if 0
	long elapsedMs = nanoDiff / 1000000;
	printf("time diff: %ld ms\n", elapsedMs);
#endif
	
	clockTime = endTime;
    }

    free(memory);
    
    return 0;
}
