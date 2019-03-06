#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Strsafe.h"
#include "Windowsx.h"
#include "timeapi.h"

#include "mfapi.h"
#include "mfidl.h"

#undef Rectangle
#undef near
#undef far

#include "denis_types.h"
#include "denis_drawing.h"
#include "denis_math.h"
#include "denis_strings.h"

#include "platform_layer.h"

typedef APP_INIT_CALL(*appInitCallPtr);
typedef APP_UPDATE_CALL(*appUpdateCallPtr);

#define APP_INIT_FUNCTION_NAME "appInit"
#define APP_UPDATE_FUNCTION_NAME "appUpdate"

struct BackBuffer
{
    void* data;
    BITMAPINFO bitmapInfo;
};

static HWND _windowHandle;

static bool _running = true;
static u32 _windowWidth;
static u32 _windowHeight;
static u32 _currentTouchPoint;

static BackBuffer _backBuffer;
static Input _input;
static Platform _platform;

#define WM_SHUTDOWN_MF_EVENT WM_USER+1

struct MediaPlayerCallback;

static IMFMediaSource* _mediaSource;
static IMFMediaSession* _mediaSession;
static MediaPlayerCallback* _mediaCallback;
static MediaPlayerState _mediaState = MediaPlayerState::MEDIA_INVALID;

#include <shlwapi.h>
struct MediaPlayerCallback : public IMFAsyncCallback
{
	//NOTE(denis): these functions have to be like this, because that's what Windows says
	MediaPlayerCallback() : refCount(1) {}
	STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
	{
		static const QITAB qit[] = 
		{
			QITABENT(MediaPlayerCallback, IMFAsyncCallback),
			{ 0 },
		};
        return QISearch(this, qit, riid, ppv);
	}
	STDMETHODIMP_(ULONG) AddRef()
	{
		return InterlockedIncrement(&refCount);
	}
	STDMETHODIMP_(ULONG) Release()
	{
		ULONG count = InterlockedDecrement(&refCount);
        if (count == 0)
        {
            delete this;
            return 0;
        }
        return count;
	}
	
	HRESULT GetParameters(DWORD*, DWORD*)
	{
		return E_NOTIMPL;
	}
	
	HRESULT Invoke(IMFAsyncResult* asyncResult)
	{
		IMFMediaEvent* event;
		_mediaSession->EndGetEvent(asyncResult, &event);
		
		MediaEventType eventType;
		event->GetType(&eventType);
		
		event->Release();
		
		if (eventType == MESessionClosed)
		{
			_mediaSource->Shutdown();
			_mediaSession->Shutdown();
			
			_mediaSource->Release();
			_mediaSession->Release();
			
			_mediaSource = 0;
			_mediaSession = 0;
			
			PostMessageA(_windowHandle, WM_SHUTDOWN_MF_EVENT, 0, 0);
			
			return S_OK;
		}
		
		//TODO(denis): handle events for pausing and resuming playback
		switch(eventType)
		{
			case MESessionStarted:
			_mediaState = MediaPlayerState::MEDIA_PLAYING;
			break;
			
			case MEEndOfPresentation:
			_mediaState = MediaPlayerState::MEDIA_FINISHED;
			
			_mediaSession->Close();
			break;
		}
		
		// start the wait for the next event in the queue
		_mediaSession->BeginGetEvent(this, 0);
		
		return S_OK;
	}
	
	private:
	long refCount;
};

static PLATFORM_MEDIA_GET_STATE(win32_mediaGetState)
{
	return _mediaState;
}

//TODO(denis): have options like whether to start playback right away
static PLATFORM_MEDIA_PLAY_FILE(win32_mediaPlayFile)
{
	// converting to wide characters is annoying
	LPWSTR mediaFile = 0;
	s32 charsNeeded = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, -1, mediaFile, 0);
	mediaFile = (LPWSTR)HEAP_ALLOC(charsNeeded*sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, fileName, -1, mediaFile, charsNeeded*sizeof(wchar_t));
	
	MFStartup(MF_VERSION, MFSTARTUP_FULL);
	
	MFCreateMediaSession(0, &_mediaSession);
	
    _mediaCallback = new MediaPlayerCallback();
	_mediaSession->BeginGetEvent(_mediaCallback, 0);
	
	IMFSourceResolver* sourceResolver;
	MFCreateSourceResolver(&sourceResolver);
	
	//TODO(denis): do I want to do this synchronously, or asynchronously?
	// for now I will choose synchronously
	MF_OBJECT_TYPE objectType;
	IUnknown* sourceInterface;
	sourceResolver->CreateObjectFromURL(mediaFile, MF_RESOLUTION_MEDIASOURCE, 0, &objectType, &sourceInterface);
	
	sourceInterface->QueryInterface(IID_PPV_ARGS(&_mediaSource));
	
	IMFPresentationDescriptor* presentationDescriptor;
	_mediaSource->CreatePresentationDescriptor(&presentationDescriptor);
	
	IMFTopology* topology;
	MFCreateTopology(&topology);
	
	DWORD streamCount;
	presentationDescriptor->GetStreamDescriptorCount(&streamCount);
	for (u32 i = 0; i < streamCount; ++i)
	{
		BOOL isSelected;
		
		IMFStreamDescriptor* streamDescriptor;
		presentationDescriptor->GetStreamDescriptorByIndex(i, &isSelected, &streamDescriptor);
		
		if (isSelected)
		{
			IMFMediaTypeHandler* typeHandler;
			streamDescriptor->GetMediaTypeHandler(&typeHandler);
			
			GUID majorType;
			typeHandler->GetMajorType(&majorType);
			
			IMFActivate* activateObject;			
			if (majorType == MFMediaType_Video)
			{
				MFCreateVideoRendererActivate(_windowHandle, &activateObject);
			}
			else if (majorType == MFMediaType_Audio)
			{
				MFCreateAudioRendererActivate(&activateObject);
			}
			else
			{
				OutputDebugStringA("Unknown major media type found\n");
				continue;
			}
			
			IMFTopologyNode* sourceNode;
			MFCreateTopologyNode(MF_TOPOLOGY_SOURCESTREAM_NODE, &sourceNode);
			sourceNode->SetUnknown(MF_TOPONODE_SOURCE, _mediaSource);
			sourceNode->SetUnknown(MF_TOPONODE_PRESENTATION_DESCRIPTOR, presentationDescriptor);
			sourceNode->SetUnknown(MF_TOPONODE_STREAM_DESCRIPTOR, streamDescriptor);
			
			topology->AddNode(sourceNode);
			
			IMFTopologyNode* outputNode;
			MFCreateTopologyNode(MF_TOPOLOGY_OUTPUT_NODE, &outputNode);
			outputNode->SetObject(activateObject);
			outputNode->SetUINT32(MF_TOPONODE_NOSHUTDOWN_ON_REMOVE, FALSE);
			
			topology->AddNode(outputNode);
			
			sourceNode->ConnectOutput(0, outputNode, 0);
			
			activateObject->Release();
			sourceNode->Release();
			outputNode->Release();
		}
		
		streamDescriptor->Release();
	}
	
	_mediaSession->SetTopology(0, topology);
	
	PROPVARIANT propVariantStart;
	PropVariantInit(&propVariantStart);
	_mediaSession->Start(0, &propVariantStart);
	
	sourceResolver->Release();
	sourceInterface->Release();
	topology->Release();
	presentationDescriptor->Release();
	PropVariantClear(&propVariantStart);
}

//NOTE(denis): user must free the returned string
static char* getProgramPathName()
{
    char *result = 0;
	
    TCHAR fileNameBuffer[MAX_PATH+1];
    DWORD getFileNameResult = GetModuleFileName(NULL, fileNameBuffer, MAX_PATH+1);
    if (getFileNameResult != 0 && GetLastError() != ERROR_INSUFFICIENT_BUFFER)
    {
		char filePath[MAX_PATH+1] = {};
		u32 indexOfLastSlash = 0;
		for (int i = 0; i < MAX_PATH && fileNameBuffer[i] != 0; ++i)
		{
			if (fileNameBuffer[i] == '\\')
				indexOfLastSlash = i;
		}
		
		copyIntoString(filePath, fileNameBuffer, 0, indexOfLastSlash);
		
		result = duplicateString(filePath);
    }
    else
    {
		//TODO(denis): try again with a bigger buffer?
    }
	
    return result;
}

static FILETIME getFileWriteTime(char* fileName)
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

static LRESULT CALLBACK win32_messageCallback(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
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
			
			if (_mediaSession)
			{
				//TODO(denis): rescale media session related stuff
			}
			else
			{
				//TODO(denis): just call "invalidate region" or something instead?
				RedrawWindow(_windowHandle, 0, 0, RDW_INVALIDATE|RDW_INTERNALPAINT);
			}
		} break;
		
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC deviceContext = BeginPaint(windowHandle, &ps);
			
			StretchDIBits(deviceContext, 0, 0, _windowWidth, _windowHeight,
						  0, 0, _backBuffer.bitmapInfo.bmiHeader.biWidth, ABS_VALUE(_backBuffer.bitmapInfo.bmiHeader.biHeight),
						  _backBuffer.data, &_backBuffer.bitmapInfo, DIB_RGB_COLORS, SRCCOPY);
			
			RECT windowRect = { 0, 0, (LONG)_windowWidth, (LONG)_windowHeight };
			ValidateRect(windowHandle, &windowRect);
			
			//TODO(denis): call IMFVideoDisplayControl::RepaintVideo when videos are playing
			
			EndPaint(windowHandle, &ps);
		} break;
		
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		{
			if (wParam == VK_UP)
			{
				_input.controller.upPressed = true;
			}
			else if (wParam == VK_DOWN)
			{
				_input.controller.downPressed = true;
			}
			else if (wParam == VK_LEFT)
			{
				_input.controller.leftPressed = true;
			}
			else if (wParam == VK_RIGHT)
			{
				_input.controller.rightPressed = true;
			}
			
			if (wParam == VK_SPACE)
			{
				_input.controller.actionPressed = true;
				_input.controller.actionWasPressed = false;
			}
		} break;
		
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
			if (wParam == VK_UP)
			{
				_input.controller.upPressed = false;
			}
			else if (wParam == VK_DOWN)
			{
				_input.controller.downPressed = false;
			}
			else if (wParam == VK_LEFT)
			{
				_input.controller.leftPressed = false;
			}
			else if (wParam == VK_RIGHT)
			{
				_input.controller.rightPressed = false;
			}
			
			if (wParam == VK_SPACE)
			{
				_input.controller.actionPressed = false;
				_input.controller.actionWasPressed = true;
			}
		} break;
		
		case WM_MOUSEMOVE:
		{
			v2i mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			_input.mouse.pos = mousePos;
		} break;
		
		case WM_LBUTTONDOWN:
		{
			v2i mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			_input.mouse.pos = mousePos;
			_input.mouse.leftClickStartPos = mousePos;
			
			_input.mouse.leftWasPressed = false;
			_input.mouse.leftPressed = true;
		} break;
		
		case WM_LBUTTONUP:
		{
			v2i mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			_input.mouse.pos = mousePos;
			
			_input.mouse.leftWasPressed = true;
			_input.mouse.leftPressed = false;
		} break;
		
		case WM_RBUTTONDOWN:
		{
			v2i mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			_input.mouse.pos = mousePos;
			_input.mouse.rightClickStartPos = mousePos;
			
			_input.mouse.rightWasPressed = false;
			_input.mouse.rightPressed = true;
		} break;
		
		case WM_RBUTTONUP:
		{
			v2i mousePos = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
			_input.mouse.pos = mousePos;
			
			_input.mouse.rightWasPressed = true;
			_input.mouse.rightPressed = false;
		} break;
		
		//NOTE(denis): used for touch and pen input
		
		case WM_POINTERUPDATE:
		{
			u32 pointerID = GET_POINTERID_WPARAM(wParam);
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
						
						_input.pen.pos = v2i(penPos.x, penPos.y);
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
			u32 pointerID = GET_POINTERID_WPARAM(wParam);
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
		
		case WM_SHUTDOWN_MF_EVENT:
		{
			_mediaCallback->Release();
			_mediaCallback = 0;
			MFShutdown();
			
			//TODO(denis): set _mediaState to MEDIA_INVALID ?
		} break;
		
		default:
		{
			result = DefWindowProc(windowHandle, message, wParam, lParam);
		} break;
    }
	
    return result;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE, LPSTR /*cmdLine*/, int)
{	
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = win32_messageCallback;
    windowClass.hInstance = instance;
    windowClass.hCursor = LoadCursor(0, IDC_ARROW);
    windowClass.lpszClassName = "win32WindowClass";
	
	//TODO(denis): is this needed, or wanted?
    //windowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	
    if (!RegisterClassEx(&windowClass))
    {
		OutputDebugString("Error creating window class\n");
		return 1;
    }
	
    DWORD windowStyles = WS_OVERLAPPEDWINDOW|WS_VISIBLE;
    if (!WINDOW_RESIZABLE)
    {
		windowStyles = windowStyles ^ (WS_THICKFRAME | WS_MAXIMIZEBOX);
    }
	
    RECT windowRect = {0, 0, (LONG)WINDOW_WIDTH, (LONG)WINDOW_HEIGHT};
    AdjustWindowRectEx(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0);
	
	u32 windowWidth = windowRect.right - windowRect.left;
	u32 windowHeight = windowRect.bottom - windowRect.top;
	
	u32 screenWidth = (u32)GetSystemMetrics(SM_CXSCREEN);
	u32 screenHeight = (u32)GetSystemMetrics(SM_CYSCREEN);
	
    _windowHandle =
		CreateWindowEx(0, windowClass.lpszClassName, WINDOW_TITLE,
					   windowStyles,
					   screenWidth/2 - windowWidth/2, screenHeight/2 - windowHeight/2,
					   windowWidth, windowHeight,
					   0, 0, instance, 0);
	
    if (!_windowHandle)
    {
		OutputDebugString("Error creating window\n");
		return 1;
    }
	
    //NOTE(denis): load in the dll and the functions we need from it
    //TODO(denis): for this project I haven't changed the working directory yet
    //TODO(denis): since the change in working directory, this CopyFile call doesn't work properly
    CopyFile(DLL_FILE_NAME, "running.dll", FALSE);
    HMODULE mainDLL = LoadLibraryA("running.dll");
    if (!mainDLL)
    {
		OutputDebugStringA("Error loading main dll\n");
		return 1;
    }
	
	appInitCallPtr appInit = (appInitCallPtr)GetProcAddress(mainDLL, APP_INIT_FUNCTION_NAME);
	appUpdateCallPtr appUpdate = (appUpdateCallPtr)GetProcAddress(mainDLL, APP_UPDATE_FUNCTION_NAME);
    if (!appUpdate)
    {
		OutputDebugStringA("Error loading appUpdate or appInit functions\n");
		return 1;
    }
	
    FILETIME lastDLLTime = getFileWriteTime(DLL_FILE_NAME);
	
    //TODO(denis): should probably let the user set the size of this
    void* mainMemory = VirtualAlloc(0, GIGABYTE(1), MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	
	u32 sleepGranularity = 1; // in ms
	bool granularSleep = timeBeginPeriod(sleepGranularity) == TIMERR_NOERROR;
	
    LARGE_INTEGER countFrequency;
    QueryPerformanceFrequency(&countFrequency); //NOTE(denis): counts/second
	
    LARGE_INTEGER lastCounts;
    QueryPerformanceCounter(&lastCounts); //NOTE(denis): counts
	
    //TODO(denis): might want to do this eventually
    //SetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);
	
	Input oldInput = {};
    _input.mouse.leftClickStartPos = v2i(-1, -1);
    _input.mouse.rightClickStartPos = v2i(-1, -1);
	
	Bitmap screen;
	screen.pixels = (u32*)_backBuffer.data;
	screen.width = _backBuffer.bitmapInfo.bmiHeader.biWidth;
	screen.height = ABS_VALUE(_backBuffer.bitmapInfo.bmiHeader.biHeight);
	screen.stride = screen.width*sizeof(u32);
	
	_platform.mediaPlayFile = win32_mediaPlayFile;
	_platform.mediaGetState = win32_mediaGetState;
	
	f32 timeS = 0.0f;
	f32 secondsPerFrame = 1.0f / (f32)FPS_TARGET;
	
	appInit(_platform, (Memory*)mainMemory, &screen);
	
    while (_running)
    {
		FILETIME currentWriteTime = getFileWriteTime(DLL_FILE_NAME);
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
				CopyFile(DLL_FILE_NAME, "running.dll", FALSE);
				mainDLL = LoadLibraryA("running.dll");
			    appUpdate = (appUpdateCallPtr)GetProcAddress(mainDLL, APP_UPDATE_FUNCTION_NAME);
				
				ASSERT(appUpdate);
			}
			else
				FindClose(lock);
		}
		
		MSG message;
		while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
		{
			// NOTE(denis): this is here because sometimes messages aren't dispatched properly for some reason
			if (message.message == WM_QUIT)
				_running = false;
			
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		screen.pixels = (u32*)_backBuffer.data;
		screen.width = _backBuffer.bitmapInfo.bmiHeader.biWidth;
		screen.height = ABS_VALUE(_backBuffer.bitmapInfo.bmiHeader.biHeight);
		screen.stride = screen.width*sizeof(u32);
		
	    appUpdate(_platform, (Memory*)mainMemory, &screen, &_input, (f32)timeS);
		
		if (!_mediaSession)
			RedrawWindow(_windowHandle, 0, 0, RDW_INVALIDATE|RDW_INTERNALPAINT);
		
		LARGE_INTEGER currentCounts;
		QueryPerformanceCounter(&currentCounts);
	    u64 timePassed = currentCounts.QuadPart - lastCounts.QuadPart;
	    timeS = (f32)timePassed / (f32)countFrequency.QuadPart;
		
		//NOTE(denis): the epsilon is an attempt to lessen the effects of random spikes
		//f32 epsilon = 0.01f;
		if (timeS < secondsPerFrame)
		{
			u32 millisecondsToSleep = (u32)((secondsPerFrame - timeS)*1000.0f);
			if (granularSleep && millisecondsToSleep > 0)
			{
				Sleep(millisecondsToSleep);
				
			}
			
			while (timeS < secondsPerFrame)
			{
				QueryPerformanceCounter(&currentCounts);
				timePassed = currentCounts.QuadPart - lastCounts.QuadPart;
				timeS = (f32)timePassed / (f32)countFrequency.QuadPart;
			}
		}
		
#if 0
		char timeBuffer[100];
		StringCbPrintf(timeBuffer, 100, "%f\n", timeMs);
		OutputDebugString(timeBuffer);
#endif
		lastCounts = currentCounts;
		
		_currentTouchPoint = 0;
		_input.touch = {};
		
		bool disableLeftWasPressed = _input.mouse.leftWasPressed && oldInput.mouse.leftPressed;
		bool disableRightWasPressed = _input.mouse.rightWasPressed && oldInput.mouse.rightPressed;
		bool disableActionWasPressed = _input.controller.actionWasPressed && oldInput.controller.actionPressed;
		
		oldInput = _input;
		if (disableLeftWasPressed)
		{
			_input.mouse.leftWasPressed = false;
			_input.mouse.leftClickStartPos = v2i(-1, -1);
		}
		if (disableRightWasPressed)
		{
			_input.mouse.rightWasPressed = false;
			_input.mouse.rightClickStartPos = v2i(-1, -1);
		}
		if (disableActionWasPressed)
			_input.controller.actionWasPressed = false;
	}
	
	timeEndPeriod(sleepGranularity);
	DestroyWindow(_windowHandle);
	
	return 0;
}
