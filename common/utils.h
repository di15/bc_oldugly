


#include "platform.h"

#ifdef PLATFORM_WIN32
#define CORRECT_SLASH '\\'
#else
#define CORRECT_SLASH '/'
#endif

extern ofstream g_log;

void FullPath(const char* filename, char* full);
string MakePathRelative(const char* full);
void ExePath(char* exepath);
string StripFile(string filepath);
void StripPathExtension(const char* n, char* o);
void StripExtension(char* filepath);
void StripPath(char* filepath);
void OpenLog(const char* filename, float version);
float StrToFloat(const char *s);
int HexToInt(const char* s);
int StrToInt(const char *s);
void CorrectSlashes(char* corrected);
void BackSlashes(char* corrected);

#ifndef PLATFORM_WIN32
long long timeGetTime();
long long GetTickCount64();
unsigned int GetTickCount();
void Sleep(int ms);
#endif

void ErrorMessage(const char* title, const char* message);
void InfoMessage(const char* title, const char* message);
void WarningMessage(const char* title, const char* message);


