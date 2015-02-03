

#ifndef BEMAIN_H
#define BEMAIN_H

#include "../common/platform.h"

#define VERSION				1.0f
#define TITLE				"WorldEd"
#define CONFIGFILE			"worlded.ini"

extern bool g_mouseout;
extern bool g_gameover;

enum APPMODE{LOADING, LOGO, EDITOR};
extern APPMODE g_mode;
extern int g_reStage;

class Matrix;

void WriteConfig();
void DrawScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3]);
void DrawSceneDepth();

#endif
