
#include "window.h"
#include "texture.h"
#include "draw/shader.h"
#include "gui/gui.h"
#include "gui/font.h"
#include "draw/shadow.h"
#include "math/3dmath.h"
#include "utils.h"

bool g_quit = false;
double g_frameinterval = 0.0f;
int g_width = INI_WIDTH;
int g_height = INI_HEIGHT;
int g_bpp = INI_BPP;
bool g_fullscreen = false;
Resolution g_selectedRes;
vector<Resolution> g_resolution;
vector<int> g_bpps;
Vec2i g_mouse;
Vec2i g_mousestart;
bool g_keyintercepted = false;
bool g_keys[256];
bool g_mousekeys[3];
double g_currentTime;
double g_lastTime = 0.0f;		// This will hold the time from the last frame
double g_framesPerSecond = 0.0f;		// This will store our fps
double g_instantFPS = 0.0f;
long long g_lasttime = GetTickCount();
float g_zoom = 1;

void AddRes(int w, int h)
{
	Resolution r;
	r.width = w;
	r.height = h;
	g_resolution.push_back(r);
}

void EnumerateDisplay()
{
#ifdef PLATFORM_WIN32
	DEVMODE dm;
	int index=0;
	while(0 != EnumDisplaySettings(NULL, index++, &dm))
	{
		Resolution r;
		r.width = dm.dmPelsWidth;
		r.height = dm.dmPelsHeight;

		bool found = false;

		for(int i=0; i<g_resolution.size(); i++)
		{
			if(g_resolution[i].width == r.width && g_resolution[i].height == r.height)
			{
				found = true;
				break;
			}
		}

		if(!found)
			g_resolution.push_back(r);

		found = false;

		int bpp = dm.dmBitsPerPel;

		for(int i=0; i<g_bpps.size(); i++)
		{
			if(g_bpps[i] == bpp)
			{
				found = true;
				break;
			}
		}

		if(!found)
			g_bpps.push_back(bpp);
	}
#endif
}

void Resize(int width, int height)
{
	if(height == 0)
		height = 1;

	glViewport(0, 0, width, height);

	if(g_width != width || g_height != height)
	{
		g_width = width;
		g_height = height;

		//if(g_fullscreen)
			//Reload();
			//ReloadTextures();
		g_GUI.resize();
	}
}

void CalculateFrameRate()
{
	static double frameTime = 0.0f;				// This stores the last frame's time

	// Get the current time in seconds
    g_currentTime = timeGetTime() * 0.001f;

	// We added a small value to the frame interval to account for some video
	// cards (Radeon's) with fast computers falling through the floor without it.

	// Here we store the elapsed time between the current and last frame,
	// then keep the current frame in our static variable for the next frame.
 	g_frameinterval = g_currentTime - frameTime + 0.005f;

	//g_instantFPS = 1.0f / (g_currentTime - frameTime);
	//g_instantFPS = 1.0f / g_frameinterval;

	frameTime = g_currentTime;

	// Increase the frame counter
    ++g_framesPerSecond;

	// Now we want to subtract the current time by the last time that was stored
	// to see if the time elapsed has been over a second, which means we found our FPS.
    if( g_currentTime - g_lastTime > 1.0f )
	{
		g_instantFPS = g_framesPerSecond;

		// Here we set the lastTime to the currentTime
	    g_lastTime = g_currentTime;

		// Reset the frames per second
        g_framesPerSecond = 0;
    }
}


bool AnimateNextFrame(int desiredFrameRate)
{
	static double lastTime = GetTickCount() * 0.001;
	static double elapsedTime = 0.0f;

	double currentTime = GetTickCount() * 0.001; // Get the time (milliseconds = seconds * .001)
	double deltaTime = currentTime - lastTime; // Get the slice of time
	double desiredFPS = 1.0 / (double)desiredFrameRate; // Store 1 / desiredFrameRate

	elapsedTime += deltaTime; // Add to the elapsed time
	lastTime = currentTime; // Update lastTime

	// Check if the time since we last checked is greater than our desiredFPS
	if( elapsedTime > desiredFPS )
	{
		elapsedTime -= desiredFPS; // Adjust the elapsed time

		// Return true, to animate the next frame of animation
		return true;
	}

	// We don't animate right now.
	return false;
	/*
	long long currentTime = GetTickCount();
	float desiredFPMS = 1000.0f/(float)desiredFrameRate;
	int deltaTime = currentTime - g_lasttime;

	if(deltaTime >= desiredFPMS)
	{
		g_lasttime = currentTime;
		return true;
	}

	return false;*/
}

bool InitWindow()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glCullFace(GL_BACK);

#ifdef PLATFORM_WIN32
	glActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)GetGLProcAddr("glActiveTextureARB");
	glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC)GetGLProcAddr("glMultiTexCoord2fARB");
    glClientActiveTextureARB = (PFNGLACTIVETEXTUREARBPROC)GetGLProcAddr("glClientActiveTextureARB");

	if(!glActiveTextureARB || !glMultiTexCoord2fARB || !glClientActiveTextureARB)
	{
		//MessageBox(g_hWnd, TEXT("Your current setup does not support multitexturing"), TEXT("Error"), MB_OK);
        ErrorMessage("Error", "Your current setup does not support multitexturing");
		return false;
	}
#endif

	InitGLSL();
	InitShadows();
	LoadBitmapFonts();

	return true;
}

void DestroyWindow(const char* title)
{
	FreeTextures();
	ReleaseShaders();

#ifdef PLATFORM_WIN32
	if(g_fullscreen)
		ChangeDisplaySettings(NULL, 0);

	if(g_hRC)
	{
		if(!wglMakeCurrent(NULL, NULL))
			MessageBox(NULL, TEXT("Release of DC and RC failed"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);

		if(!wglDeleteContext(g_hRC))
			MessageBox(NULL, TEXT("Release of rendering context failed"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);

		g_hRC = NULL;
	}

	if(g_hDC && !ReleaseDC(g_hWnd, g_hDC))
	{
		MessageBox(NULL, TEXT("Release device context failed"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
		g_hDC = NULL;
	}

	if(g_hWnd && !DestroyWindow(g_hWnd))
	{
		MessageBox(NULL, TEXT("Could not release hWnd"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
		g_hWnd = NULL;
	}

	if(!UnregisterClass(title, g_hInstance))
	{
		MessageBox(NULL, TEXT("Could not unregister class"), TEXT("Error"), MB_OK | MB_ICONINFORMATION);
		g_hInstance = NULL;
	}
#endif
}

#ifdef PLATFORM_WIN32
bool MakeWindow(const char* title, HICON icon, WNDPROC wndproc)
#else
bool MakeWindow(const char* title)
#endif
{
#ifdef PLATFORM_WIN32
	unsigned int PixelFormat;
	WNDCLASS wc;
	DWORD dwExStyle;
	DWORD dwStyle;
	RECT WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)g_selectedRes.width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)g_selectedRes.height;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = (WNDPROC)wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = g_hInstance;
	wc.hIcon = icon; //LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GUN));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = title;

	if(!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Failed to register the window class"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return FALSE;
	}

	if(g_fullscreen)
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= g_selectedRes.width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= g_selectedRes.height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= g_bpp;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			char msg[128];
			wsprintf(msg, TEXT("The requested fullscreen mode %dx%dx%d is not supported by\nyour video card. Use windowed mode instead?"), g_width, g_height, g_bpp);

			if(MessageBox(NULL, msg, title, MB_YESNO | MB_ICONEXCLAMATION) == IDYES)
				g_fullscreen = false;
			else
			{
				MessageBox(NULL, TEXT("Program will now close"), TEXT("Error"), MB_OK | MB_ICONSTOP);
				g_quit = true;
				return FALSE;
			}
		}
	}

	int startx = 0;
	int starty = 0;

	if(g_fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
		//startx = CW_USEDEFAULT;
		//starty = CW_USEDEFAULT
		startx = GetSystemMetrics(SM_CXSCREEN)/2 - g_selectedRes.width/2;
		starty = GetSystemMetrics(SM_CYSCREEN)/2 - g_selectedRes.height/2;
	}

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	if (!(g_hWnd = CreateWindowEx(dwExStyle, title, title, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
								startx, starty,	WindowRect.right-WindowRect.left, WindowRect.bottom-WindowRect.top,
								NULL, NULL,	g_hInstance, NULL)))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Window creation error"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	static PIXELFORMATDESCRIPTOR pfd=
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		g_bpp,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		24,											// 16Bit Z-Buffer (Depth Buffer)
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};

	if (!(g_hDC = GetDC(g_hWnd)))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Can't create a GL device context"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(PixelFormat = ChoosePixelFormat(g_hDC, &pfd)))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Can't find a suitable pixel format"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!SetPixelFormat(g_hDC, PixelFormat, &pfd))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Can't set the pixel format"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if (!(g_hRC = wglCreateContext(g_hDC)))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Can't create a GL rendering context"), TEXT("ERROR"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	if(!wglMakeCurrent(g_hDC, g_hRC))
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Can't activate the GL rendering context"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	ShowWindow(g_hWnd, SW_SHOW);
	SetForegroundWindow(g_hWnd);
	SetFocus(g_hWnd);
	//Resize(g_width, g_height);

	if(!InitWindow())
	{
		DestroyWindow(title);
		MessageBox(NULL, TEXT("Initialization failed"), TEXT("Error"), MB_OK | MB_ICONEXCLAMATION);
		return false;
	}
#endif

	return true;
}
