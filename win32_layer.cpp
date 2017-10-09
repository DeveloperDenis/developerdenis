#include "denis_win32.h"
#include "denis_types.h"
#include "denis_math.h"

#include "win32_layer.h"

#include "Strsafe.h"
#include "Windowsx.h"
#include "ShellScalingAPI.h"

typedef MAIN_UPDATE_CALL(*mainUpdateCallPtr);

struct BackBuffer
{
    void* data;
    BITMAPINFO bitmapInfo;
};

static bool _running = true;
static HDC _deviceContext;
static uint32 _windowWidth;
static uint32 _windowHeight;
static uint32 _currentTouchPoint;

static BackBuffer _backBuffer;
static Input _input;

//TODO(denis): put these into the Input struct
static Controller _controller;
static Mouse _mouse;

FILETIME getFileWriteTime(char* fileName)
{
    FILETIME result = {};

	char* programPath = getProgramPathName();
    char* fullFilePath = concatStrings(programPath, fileName);

    WIN32_FIND_DATA findData;
    HANDLE file = FindFirstFile(fileName, &findData);

    if (file != INVALID_HANDLE_VALUE)
    {
		result = findData.ftLastWriteTime;
		FindClose(file);
    }

	HEAP_FREE(programPath);
	HEAP_FREE(fullFilePath);
    
    return result;
}

LRESULT CALLBACK win32_messageCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result = 0;
	
    switch(message)
    {
		case WM_DESTROY:
		{
			_running = false;
		} break;

		case WM_QUIT:
		{
			_running = false;
		} break;

		case WM_SIZE:
		{
			RECT clientRect;
			GetClientRect(windowHandle, &clientRect);
			_windowWidth = clientRect.right - clientRect.left;
			_windowHeight = clientRect.bottom - clientRect.top;

			if (_backBuffer.data)
			{
				HEAP_FREE(_backBuffer.data);
			}

			_backBuffer.bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			_backBuffer.bitmapInfo.bmiHeader.biWidth = clientRect.right;
			_backBuffer.bitmapInfo.bmiHeader.biHeight = -clientRect.bottom; //NOTE(denis): positive means origin in lower-left, negative means origin in upper-left
			_backBuffer.bitmapInfo.bmiHeader.biPlanes = 1;
			_backBuffer.bitmapInfo.bmiHeader.biBitCount = 32;
			_backBuffer.bitmapInfo.bmiHeader.biCompression = BI_RGB;

			//TODO(denis): error checking?
			_backBuffer.data = HEAP_ALLOC(_windowWidth*_windowHeight*(_backBuffer.bitmapInfo.bmiHeader.biBitCount/8));
		} break;

		case WM_PAINT:
		{
			StretchDIBits(_deviceContext, 0, 0, _windowWidth, _windowHeight,
						  0, 0, _backBuffer.bitmapInfo.bmiHeader.biWidth, ABS_VALUE(_backBuffer.bitmapInfo.bmiHeader.biHeight),
						  _backBuffer.data, &_backBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);

			RECT windowRect = { 0, 0, (LONG)_windowWidth, (LONG)_windowHeight };
			ValidateRect(windowHandle, &windowRect);
		} break;

		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_UP)
			{
				_controller.upPressed = true;
			}
			else if (wParam == VK_DOWN)
			{
				_controller.downPressed = true;
			}
			else if (wParam == VK_LEFT)
			{
				_controller.leftPressed = true;
			}
			else if (wParam == VK_RIGHT)
			{
				_controller.rightPressed = true;
			}
		} break;

		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (wParam == VK_UP)
			{
				_controller.upPressed = false;
			}
			else if (wParam == VK_DOWN)
			{
				_controller.downPressed = false;
			}
			else if (wParam == VK_LEFT)
			{
				_controller.leftPressed = false;
			}
			else if (wParam == VK_RIGHT)
			{
				_controller.rightPressed = false;
			}			
		} break;

		case WM_MOUSEMOVE:
		{
			//TODO(denis): check if the mouse buttons are down?
			_mouse.x = GET_X_LPARAM(lParam);
			_mouse.y = GET_Y_LPARAM(lParam);
		} break;

		case WM_LBUTTONDOWN:
		{
			//TODO(denis): do stuff
		} break;

		case WM_LBUTTONUP:
		{
			//TODO(denis): do stuff
		} break;

		//NOTE(denis): used for touch and pen input
		
		case WM_POINTERUPDATE:
		{
			uint32 pointerID = GET_POINTERID_WPARAM(wParam);
			POINTER_INPUT_TYPE inputType;
			
		    if (GetPointerType(pointerID, &inputType))
			{
				switch (inputType)
				{
					case PT_TOUCH:
					{
						//TODO(denis): for now we only process up to five points
						if (_currentTouchPoint >= 4)
							break;
						
						POINTER_TOUCH_INFO touchInfo = {};
						GetPointerTouchInfo(pointerID, &touchInfo);

						RECT touchRect = touchInfo.rcContactRaw;
						POINT touchPoint;
						if (touchInfo.touchMask & TOUCH_MASK_CONTACTAREA)
						{
							touchPoint.x =
								touchRect.left + (touchRect.right - touchRect.left)/2;
							touchPoint.y =
								touchRect.top + (touchRect.bottom - touchRect.top)/2;
						}
						else
						{
							touchPoint.x = touchRect.left;
						    touchPoint.y = touchRect.top;
						}

						ScreenToClient(windowHandle, &touchPoint);
						_input.touch.points[_currentTouchPoint].x = touchPoint.x;
						_input.touch.points[_currentTouchPoint].y = touchPoint.y;
						
						++_currentTouchPoint;
						_input.touch.numActivePoints = _currentTouchPoint;
					} break;

					case PT_PEN:
					{
						POINTER_PEN_INFO penInfo = {};
						GetPointerPenInfo(pointerID, &penInfo);

						POINT penPos = penInfo.pointerInfo.ptPixelLocationRaw;
						ScreenToClient(windowHandle, &penPos);

						_input.pen.x = penPos.x;
						_input.pen.y = penPos.y;

						_input.pen.usingEraser = penInfo.penFlags & PEN_FLAG_ERASER;
						
						if (penInfo.penMask & PEN_MASK_PRESSURE)
						{
						    _input.pen.pressure = penInfo.pressure;
						}
						else
						{
							_input.pen.pressure = 0;
						}
					} break;
				}
			}
		} break;

		case WM_POINTERDOWN:
		{
			uint32 pointerID = GET_POINTERID_WPARAM(wParam);
			POINTER_INPUT_TYPE inputType;

		    if (GetPointerType(pointerID, &inputType))
			{
				switch (inputType)
				{
					case PT_TOUCH:
					{
						
					} break;

					case PT_PEN:
					{
						
					} break;
				}	
			}
		} break;

		case WM_POINTERUP:
		{
			//TODO(denis): do something?
		} break;
		
		default:
		{
			result = DefWindowProc(windowHandle, message, wParam, lParam);
		} break;
    }

    return result;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
	if (STATIC_SETTINGS::WINDOW_TITLE == 0 || STATIC_SETTINGS::DLL_FILE_NAME == 0 ||
		STATIC_SETTINGS::FPS_TARGET == 0)
	{
		ASSERT(!"You must initialize STATIC_SETTINGS struct properly");
	}
		
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = win32_messageCallback;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = "TestingWindowsClass";
	
    if (!RegisterClassEx(&windowClass))
    {
		OutputDebugString("Error creating window class\n");
		return 1;
    }
	
    HWND windowHandle =
		CreateWindowEx(0, windowClass.lpszClassName, STATIC_SETTINGS::WINDOW_TITLE,
					   WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					   CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, 0, 0,
					   instance, 0);
	
    if (!windowHandle)
    {
		OutputDebugString("Error creating window\n");
		return 1;
    }

    //NOTE(denis): don't need to release this because this is our window's private DC
    _deviceContext = GetDC(windowHandle);

    //NOTE(denis): load in the dll and the functions we need from it
	//TODO(denis): for this project I haven't changed the working directory yet
	//TODO(denis): since the change in working directory, this CopyFile call doesn't work properly
    CopyFile(STATIC_SETTINGS::DLL_FILE_NAME, "running.dll", FALSE);
    HMODULE mainDLL = LoadLibraryA("running.dll");
    if (!mainDLL)
    {
		OutputDebugStringA("Error loading main dll\n");
		return 1;
    }
	
    mainUpdateCallPtr mainUpdateCall = (mainUpdateCallPtr)GetProcAddress(mainDLL, "mainUpdateCall");
    if (!mainUpdateCall)
    {
		OutputDebugStringA("Error loading mainUpdateCall function\n");
		return 1;
    }

    FILETIME lastDLLTime = getFileWriteTime(STATIC_SETTINGS::DLL_FILE_NAME);

	//TODO(denis): should probably let the user set the size of this
    void* mainMemory = VirtualAlloc(0, GIGABYTE(1), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);

	LARGE_INTEGER countFrequency;
	QueryPerformanceFrequency(&countFrequency); //NOTE(denis): counts/second
	
	LARGE_INTEGER lastCounts;
	QueryPerformanceCounter(&lastCounts); //NOTE(denis): counts

	//TODO(denis): might want to do this eventually
	//SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
	
    while (_running)
    {
		FILETIME currentWriteTime = getFileWriteTime(STATIC_SETTINGS::DLL_FILE_NAME);
		if (CompareFileTime(&lastDLLTime, &currentWriteTime) == -1)
		{
			//NOTE(denis): the lock is used to prevent DLL reloading before the PDB file
			// has been created
			WIN32_FIND_DATA lockData;
			HANDLE lock = FindFirstFile("pdb.lock", &lockData);

			if (lock == INVALID_HANDLE_VALUE)
			{
				// lastDLLTime is earlier
				FreeLibrary(mainDLL);
				//TODO(denis): this part is an exact copy of the above, REFACTOR
				CopyFile(STATIC_SETTINGS::DLL_FILE_NAME, "running.dll", FALSE);
				mainDLL = LoadLibraryA("running.dll");
				mainUpdateCall = (mainUpdateCallPtr)GetProcAddress(mainDLL, "mainUpdateCall");
			}
			else
				FindClose(lock);
		}
	
		MSG message;
		while (PeekMessage(&message, windowHandle, 0, 0, PM_REMOVE))
		{
			// NOTE(denis): this is here because sometimes messages aren't dispatched properly for some reason
			if (message.message == WM_QUIT)
				_running = false;
		        
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
	    Bitmap screen;
		screen.pixels = _backBuffer.data;
		screen.width = _backBuffer.bitmapInfo.bmiHeader.biWidth;
		screen.height = ABS_VALUE(_backBuffer.bitmapInfo.bmiHeader.biHeight);
	    
		mainUpdateCall((Memory*)mainMemory, &screen, &_input);
		
		RedrawWindow(windowHandle, 0, 0, RDW_INTERNALPAINT);

		LARGE_INTEGER currentCounts;
		QueryPerformanceCounter(&currentCounts);
		uint64 timePassed = currentCounts.QuadPart - lastCounts.QuadPart;
		real64 timeMs = (real64)timePassed * 1000.0 / (real64)countFrequency.QuadPart;

		//TODO(denis): probably don't do a busy loop
		//TODO(denis): the epsilon is an attempt to lessen the effects of random spikes
		// I should look into why the random spikes occur, and if I can prevent them
		real32 epsilon = 0.01f;
		while (timeMs < (real64)1/(real64)STATIC_SETTINGS::FPS_TARGET * 1000.0 - epsilon)
		{
			QueryPerformanceCounter(&currentCounts);
			timePassed = currentCounts.QuadPart - lastCounts.QuadPart;
			timeMs = (real64)timePassed * 1000.0 / (real64)countFrequency.QuadPart;
		}
#if 0
		char timeBuffer[100];
	    StringCbPrintf(timeBuffer, 100, "%f\n", timeMs);
	    OutputDebugString(timeBuffer);
#endif
		lastCounts = currentCounts;

		_currentTouchPoint = 0;
		_input.touch = {};
    }
	
    DestroyWindow(windowHandle);
	
    return 0;
}
