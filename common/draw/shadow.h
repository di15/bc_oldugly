

#ifndef SHADOW_H
#define SHADOW_H

#include "../math/vec3f.h"

extern unsigned int g_depth;
extern Vec3f g_lightPos;
extern Vec3f g_lightEye;
extern Vec3f g_lightUp;

class Matrix;

void InitShadows();
void RenderToShadowMap(Matrix projection, Matrix viewmat, Matrix modelmat, Vec3f focus);
void RenderShadowedScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelview);
void UseShadow(int shader, Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3]);

extern void (*DrawSceneDepthFunc)();
extern void (*DrawSceneFunc)(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3]);

#endif