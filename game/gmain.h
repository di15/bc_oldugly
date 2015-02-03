

#ifndef GAME_MAIN_H
#define GAME_MAIN_H

#include "../common/platform.h"

#define VERSION				1.0f
#define TITLE				"Battle Computer"
#define CONFIGFILE			"game.ini"

extern bool g_active;
extern bool g_fullscreen;
extern bool g_mouseout;
extern bool g_gameover;

enum APPMODE{LOADING, LOGO, INTRO, RELOADING, MENU, PLAY, PAUSE};
extern APPMODE g_mode;
extern int g_reStage;
extern string g_startmap;

class Matrix;

void DrawScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3]);
void DrawSceneDepth();
void SkipLogo();
void WriteConfig();

#endif
