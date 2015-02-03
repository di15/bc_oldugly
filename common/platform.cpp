
#include "platform.h"

#ifdef PLATFORM_WIN32
HDC g_hDC = NULL;
HGLRC g_hRC = NULL;
HWND g_hWnd = NULL;
HINSTANCE g_hInstance = NULL;
#endif

#ifdef PLATFORM_LINUX
//Display *g_display;
//Window g_window;
SDL_Window *g_window;
#endif
