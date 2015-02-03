
#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #define PLATFORM_WIN32
#elif __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_MAC
        #define PLATFORM_MAC
    #elif TARGET_OS_IPHONE
        #define PLATFORM_IOS
        #define PLATFORM_IPHONE
    #elif TARGET_OS_IPAD
        #define PLATFORM_IOS
        #define PLATFORM_IPAD
    #endif
#elif defined( __GNUC__ )
    #define PLATFORM_LINUX
#elif defined( __linux__ )
    #define PLATFORM_LINUX
#elif defined ( __linux )
    #define PLATFORM_LINUX
#endif

#ifdef PLATFORM_WIN32
#pragma warning(disable: 4996)
#pragma warning(disable: 4018)
#pragma warning(disable: 4160)
#pragma warning(disable: 4244)
#pragma warning(disable: 4800)
#pragma warning(disable: 4305)
#endif

#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#ifdef PLATFORM_WIN32
#include <winsock2.h>	// winsock2 needs to be included before windows.h
#include <windows.h>
#include <mmsystem.h>
#endif

#ifdef PLATFORM_LINUX
#include <GL/glx.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

#ifdef PLATFORM_WIN32
#include <gl/glaux.h>
#endif

#ifdef PLATFORM_WIN32
#include <commdlg.h>
#endif

#ifdef PLATFORM_LINUX
#include <linux/limits.h>
#include <gcrypt.h>
#endif

#if defined(PLATFORM_LINUX) || defined(PLATFORM_MAC)
#include <sys/time.h>
#include <sys/types.h>
#include <dirent.h>
#endif

#include <string>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <set>
#include <fstream>
#include <iostream>
#include <math.h>

#include <jpeglib.h>
#include <png.h>
#include <zip.h>

#ifdef PLATFORM_LINUX
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif

#ifdef PLATFORM_MAC
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#endif

#ifdef PLATFORM_WIN32
//#pragma comment(lib, "SDL.lib")
//#pragma comment(lib, "SDLmain.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#pragma comment(lib, "glaux.lib")
#pragma comment(lib, "jpeg.lib")
#pragma comment(lib, "libpng15.lib")
#pragma comment(lib, "zlibstatic.lib")
#pragma comment(lib, "zipstatic.lib")
#endif

using namespace std;

#ifndef MAX_PATH
#define MAX_PATH 260
//#define MAX_PATH PATH_MAX
#endif

#ifndef PLATFORM_WIN32
#define SOCKET int
typedef unsigned char byte;
typedef unsigned int UINT;
typedef unsigned char BYTE; // 1byte
typedef unsigned short WORD; // 2bytes
typedef unsigned long DWORD; //4bytes
#define _isnan isnan
#define stricmp strcasecmp
#define _stricmp strcasecmp
#endif

/*
#ifndef _isnan
int _isnan(double x) { return x != x; }
#endif
*/

#ifndef min
#define min(X,Y) ((X) < (Y) ? (X) : (Y))
#endif

#ifndef max
#define max(X,Y) ((X) > (Y) ? (X) : (Y))
#endif

#ifdef PLATFORM_WIN32
extern HDC g_hDC;
extern HGLRC g_hRC;
extern HWND g_hWnd;
extern HINSTANCE g_hInstance;
#endif

#ifdef PLATFORM_LINUX
//extern Display *g_display;
//extern Window g_window;
extern SDL_Window *g_window;
#endif

#endif
