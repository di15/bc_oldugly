
#ifndef WINDOW_H
#define WINDOW_H

#include "platform.h"
#include "math/3dmath.h"
#include "math/vec2i.h"

#define INI_WIDTH			640
#define INI_HEIGHT			480
#define INI_BPP				32
#define FRAME_RATE			30
#define MIN_DISTANCE		1.0f
#define MAX_DISTANCE		1000000.0f
#define FIELD_OF_VIEW		45.0f
#define PROJ_RIGHT			(500)	//(30*12.5f)

extern double g_frameinterval;
extern int g_width;
extern int g_height;
extern int g_bpp;
extern bool g_quit;
extern bool g_active;
extern bool g_fullscreen;
extern Vec2i g_mouse;
extern Vec2i g_mousestart;
extern double g_currentTime;
extern double g_lastTime;
extern double g_framesPerSecond;
extern double g_instantFPS;
extern float g_zoom;

struct Resolution
{
	int width;
	int height;
};

extern Resolution g_selectedRes;
extern vector<Resolution> g_resolution;
extern vector<int> g_bpps;

extern bool g_keyintercepted;
extern bool g_keys[256];
extern bool g_mousekeys[3];

void AddRes(int w, int h);
void CalculateFrameRate();
bool AnimateNextFrame(int desiredFrameRate);
void EnumerateDisplay();
void Resize(int width, int height);
void DestroyWindow(const char* title);

#ifdef PLATFORM_WIN32
bool MakeWindow(const char* title, HICON icon, WNDPROC wndproc);
#endif

#endif
