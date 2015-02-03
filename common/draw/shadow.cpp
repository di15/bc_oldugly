

#include "../utils.h"
#include "shadow.h"
#include "shader.h"
#include "../math/3dmath.h"
#include "model.h"
#include "particle.h"
#include "projectile.h"
#include "../debug.h"
#include "billboard.h"
#include "../platform.h"
#include "../window.h"
#include "../math/vec3f.h"
#include "../math/matrix.h"
#include "model.h"
#include "../sim/sim.h"
#include "model.h"
#include "../sim/map.h"
#include "../math/camera.h"

unsigned int g_depth;
const int g_depthSizeX = 2048;	//512;	//4096;
const int g_depthSizeY = 2048;	//512;	//4096;
unsigned int g_rbDepth;
unsigned int g_fbDepth;

//1000000.0f/300 = 3333.3333333333333333333333333333 cm =

Vec3f g_lightOff(-MAX_DISTANCE/500*2, MAX_DISTANCE/300*2, MAX_DISTANCE/400*2);
Vec3f g_lightPos;	//(-MAX_DISTANCE/2, MAX_DISTANCE/5, MAX_DISTANCE/3);
Vec3f g_lightEye;	//(-MAX_DISTANCE/2+1.0f/2.0f, MAX_DISTANCE/3-1.0f/3.0f, MAX_DISTANCE/3-1.0f/3.0f);
Vec3f g_lightUp(0,1,0);

Matrix g_lightProjectionMatrix;
Matrix g_lightModelViewMatrix;
Matrix g_cameraInverseModelViewMatrix;
Matrix g_lightMatrix;
Matrix g_cameraModelViewMatrix;
Matrix g_cameraProjectionMatrix;

void (*DrawSceneDepthFunc)() = NULL;
void (*DrawSceneFunc)(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3]) = NULL;

Vec3f g_viewInter;

void InitShadows()
{
	glGenTextures(1, &g_depth);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, g_depthSizeX, g_depthSizeY, 0, GL_RGBA, GL_UNSIGNED_SHORT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenRenderbuffersEXT(1, &g_rbDepth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_rbDepth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT24, g_depthSizeX, g_depthSizeY);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, 0);

	glGenFramebuffersEXT(1, &g_fbDepth);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_fbDepth);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_depth, 0);
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_rbDepth);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}

void InverseMatrix(Matrix* dstm, Matrix srcm)
{
	float dst[16];
	const float* src = srcm.m_matrix;

	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0;

	dstm->set(dst);
}

void InverseMatrix(float dst[16], float src[16])
{
	dst[0] = src[0];
	dst[1] = src[4];
	dst[2] = src[8];
	dst[3] = 0.0;
	dst[4] = src[1];
	dst[5] = src[5];
	dst[6]  = src[9];
	dst[7] = 0.0;
	dst[8] = src[2];
	dst[9] = src[6];
	dst[10] = src[10];
	dst[11] = 0.0;
	dst[12] = -(src[12] * src[0]) - (src[13] * src[1]) - (src[14] * src[2]);
	dst[13] = -(src[12] * src[4]) - (src[13] * src[5]) - (src[14] * src[6]);
	dst[14] = -(src[12] * src[8]) - (src[13] * src[9]) - (src[14] * src[10]);
	dst[15] = 1.0;
}

bool gluInvertMatrix(const float m[16], float invOut[16])
{
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    return true;
}

bool InverseMatrix2(Matrix mat, Matrix invMat)
{
    double inv[16], det;
    int i;

	const float* m = mat.m_matrix;

    inv[0] = m[5]  * m[10] * m[15] -
             m[5]  * m[11] * m[14] -
             m[9]  * m[6]  * m[15] +
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] -
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] +
              m[4]  * m[11] * m[14] +
              m[8]  * m[6]  * m[15] -
              m[8]  * m[7]  * m[14] -
              m[12] * m[6]  * m[11] +
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] -
             m[4]  * m[11] * m[13] -
             m[8]  * m[5] * m[15] +
             m[8]  * m[7] * m[13] +
             m[12] * m[5] * m[11] -
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] +
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] -
               m[8]  * m[6] * m[13] -
               m[12] * m[5] * m[10] +
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] +
              m[1]  * m[11] * m[14] +
              m[9]  * m[2] * m[15] -
              m[9]  * m[3] * m[14] -
              m[13] * m[2] * m[11] +
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] -
             m[0]  * m[11] * m[14] -
             m[8]  * m[2] * m[15] +
             m[8]  * m[3] * m[14] +
             m[12] * m[2] * m[11] -
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] +
              m[0]  * m[11] * m[13] +
              m[8]  * m[1] * m[15] -
              m[8]  * m[3] * m[13] -
              m[12] * m[1] * m[11] +
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] -
              m[0]  * m[10] * m[13] -
              m[8]  * m[1] * m[14] +
              m[8]  * m[2] * m[13] +
              m[12] * m[1] * m[10] -
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] -
             m[1]  * m[7] * m[14] -
             m[5]  * m[2] * m[15] +
             m[5]  * m[3] * m[14] +
             m[13] * m[2] * m[7] -
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] +
              m[0]  * m[7] * m[14] +
              m[4]  * m[2] * m[15] -
              m[4]  * m[3] * m[14] -
              m[12] * m[2] * m[7] +
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] -
              m[0]  * m[7] * m[13] -
              m[4]  * m[1] * m[15] +
              m[4]  * m[3] * m[13] +
              m[12] * m[1] * m[7] -
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] +
               m[0]  * m[6] * m[13] +
               m[4]  * m[1] * m[14] -
               m[4]  * m[2] * m[13] -
               m[12] * m[1] * m[6] +
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] +
              m[1] * m[7] * m[10] +
              m[5] * m[2] * m[11] -
              m[5] * m[3] * m[10] -
              m[9] * m[2] * m[7] +
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] -
             m[0] * m[7] * m[10] -
             m[4] * m[2] * m[11] +
             m[4] * m[3] * m[10] +
             m[8] * m[2] * m[7] -
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] +
               m[0] * m[7] * m[9] +
               m[4] * m[1] * m[11] -
               m[4] * m[3] * m[9] -
               m[8] * m[1] * m[7] +
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] -
              m[0] * m[6] * m[9] -
              m[4] * m[1] * m[10] +
              m[4] * m[2] * m[9] +
              m[8] * m[1] * m[6] -
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        return false;

    det = 1.0 / det;

	float invOut[16];
    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

	invMat.set(invOut);

    return true;
}

void Transpose(Matrix mat, Matrix transpMat)
{
	const float* m = mat.m_matrix;
	float transp[16];

	int j;
	int index_1;
	int index_2;

	for (int i = 0; i < 4; i++)
	{
		for (j = 0; j < 4;  j++)
		{
			index_1 = i * 4 + j;
			index_2 = j * 4 + i;
			transp[index_1] = m[index_2];
		}
	}

	transpMat.set(transp);
}

void RenderToShadowMap(Matrix projection, Matrix viewmat, Matrix modelmat, Vec3f focus)
{
	glDisable(GL_CULL_FACE);

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_fbDepth);
	glViewport(0, 0, g_depthSizeX, g_depthSizeY);

	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_POLYGON_OFFSET_FILL);
	glPolygonOffset(2.0, 500.0);

	g_lightPos = focus + g_lightOff;
	g_lightEye = focus;

	//g_lightEye = Vec3f(0,0,0);
	//g_lightPos = g_lightEye + g_lightOff;

	float zoom = 0.4f;
	//g_lightProjectionMatrix = BuildPerspProjMat(90.0, 1.0, 30.0, 10000.0);
	//g_lightProjectionMatrix = setorthographicmat(-PROJ_RIGHT*2/g_zoom, PROJ_RIGHT*2/g_zoom, PROJ_RIGHT*2/g_zoom, -PROJ_RIGHT*2/g_zoom, MIN_DISTANCE, MAX_DISTANCE/300);
	g_lightProjectionMatrix = setorthographicmat(-PROJ_RIGHT*2/zoom, PROJ_RIGHT*2/zoom, PROJ_RIGHT*2/zoom, -PROJ_RIGHT*2/zoom, MIN_DISTANCE, MAX_DISTANCE/300);
	g_lightModelViewMatrix = gluLookAt2(g_lightPos.x, g_lightPos.y, g_lightPos.z,
		g_lightEye.x, g_lightEye.y, g_lightEye.z,
		g_lightUp.x, g_lightUp.y, g_lightUp.z);

	UseS(SHADER_DEPTH);
	glUniformMatrix4fv(g_shader[SHADER_DEPTH].m_slot[SSLOT_PROJECTION], 1, 0, g_lightProjectionMatrix.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_DEPTH].m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_DEPTH].m_slot[SSLOT_VIEWMAT], 1, 0, g_lightModelViewMatrix.m_matrix);
	glUniform4f(g_shader[SHADER_MODEL].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	glEnableVertexAttribArray(g_shader[SHADER_DEPTH].m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(g_shader[SHADER_DEPTH].m_slot[SSLOT_TEXCOORD0]);

	if(DrawSceneDepthFunc != NULL)
		DrawSceneDepthFunc();

	TurnOffShader();

	glDisable(GL_POLYGON_OFFSET_FILL);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	//glViewport(0, 0, g_width, g_height);
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
	glEnable(GL_CULL_FACE);
}

void UseShadow(int shader, Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3])
{
	UseS(shader);
	Shader* s = &g_shader[g_curS];
	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, 0, projection.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, 0, viewmat.m_matrix);
	//glUniformMatrix4fv(s->m_slot[SSLOT_NORMALMAT], 1, 0, modelviewinv.m_matrix);
	//glUniformMatrix4fv(s->m_slot[SSLOT_INVMODLVIEWMAT], 1, 0, modelviewinv.m_matrix);
	glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD1]);
	//glEnableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);

	//glUniformMatrix4fvARB(s->m_slot[SSLOT_LIGHTMATRIX], 1, false, g_lightMatrix);
	glUniformMatrix4fvARB(s->m_slot[SSLOT_LIGHTMATRIX], 1, false, g_lightMatrix.m_matrix);

	glUniform3fARB(s->m_slot[SSLOT_LIGHTPOS], mvLightPos[0], mvLightPos[1], mvLightPos[2]);
	glUniform3fARB(s->m_slot[SSLOT_SUNDIRECTION], lightDir[0], lightDir[1], lightDir[2]);
	glUniform1fARB(s->m_slot[SSLOT_MAXELEV], g_maxelev);
}

void RenderShadowedScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelview)
{
	//glViewport(0, 0, g_width, g_height);
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(FIELD_OF_VIEW, double(g_width) / double(g_height), MIN_DISTANCE, MAX_DISTANCE);
	//g_cameraProjectionMatrix = BuildPerspProjMat(FIELD_OF_VIEW, double(g_width) / double(g_height), MIN_DISTANCE, MAX_DISTANCE);
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//g_camera.Look();

	//glGetFloatv(GL_PROJECTION_MATRIX, g_cameraProjectionMatrix);
	g_cameraProjectionMatrix = projection;
	//glGetFloatv(GL_MODELVIEW_MATRIX, g_cameraModelViewMatrix);
	g_cameraModelViewMatrix = modelview;

	// Do non-shadowed drawing here
	//DrawSkyBox(g_camera.LookPos());

	InverseMatrix(&g_cameraInverseModelViewMatrix, modelview);

	float trans[] = { 0.5f, 0.5f, 0.5f };
	g_lightMatrix.loadIdentity();
	g_lightMatrix.setTranslation(trans);
	float scalef[] = { 0.5f, 0.5f, 0.5f };
	Matrix scalem;
	scalem.setScale(scalef);
	g_lightMatrix.postMultiply(scalem);
	g_lightMatrix.postMultiply(g_lightProjectionMatrix);
	g_lightMatrix.postMultiply(g_lightModelViewMatrix);
	//g_lightMatrix.postMultiply(g_cameraInverseModelViewMatrix);

	Matrix modelviewinv;
	InverseMatrix2(modelview, modelviewinv);
	Transpose(modelviewinv, modelviewinv);

	const float* mv = g_cameraModelViewMatrix.m_matrix;
	float mvLightPos[3];
	mvLightPos[0] = mv[0] * g_lightPos.x + mv[4] * g_lightPos.y + mv[8] * g_lightPos.z + mv[12];
	mvLightPos[1] = mv[1] * g_lightPos.x + mv[5] * g_lightPos.y + mv[9] * g_lightPos.z + mv[13];
	mvLightPos[2] = mv[2] * g_lightPos.x + mv[6] * g_lightPos.y + mv[10] * g_lightPos.z + mv[14];

	float lightDir[3];
	//lightDir[0] = g_lightEye.x - g_lightPos.x;
	//lightDir[1] = g_lightEye.y - g_lightPos.y;
	//lightDir[2] = g_lightEye.z - g_lightPos.z;
	lightDir[0] = g_lightPos.x - g_lightEye.x;
	lightDir[1] = g_lightPos.y - g_lightEye.y;
	lightDir[2] = g_lightPos.z - g_lightEye.z;

	if(DrawSceneFunc != NULL)
		DrawSceneFunc(projection, viewmat, modelmat, modelviewinv, mvLightPos, lightDir);
		//DrawSceneFunc(projection, viewmat, modelmat, modelviewinv, (float*)&g_lightPos, lightDir);

	TurnOffShader();
	glBindTexture(GL_TEXTURE_2D, 0);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	/*
	if(g_mode == EDITOR)
	{
		UseS(COLOR3D);
		glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_PROJECTION], 1, 0, projection.m_matrix);
		glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
		glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_VIEWMAT], 1, 0, viewmat.m_matrix);
		glUniform4f(g_shader[SHADER_COLOR3D].m_slot[SSLOT_COLOR], 0, 1, 0, 1);
		glEnableVertexAttribArray(g_shader[SHADER_COLOR3D].m_slot[SSLOT_POSITION]);
		glEnableVertexAttribArray(g_shader[SHADER_COLOR3D].m_slot[SSLOT_NORMAL]);
		DrawTileSq();
	}*/

	TurnOffShader();
	//g_camera.Look();
}


