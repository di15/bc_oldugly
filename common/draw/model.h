

#ifndef MODEL_H
#define MODEL_H

#include "../platform.h"
#include "ms3d.h"
#include "../math/vec3f.h"
#include "../math/vec2f.h"

class VertexArray;
class MS3DModel;
class Shader;

class Model
{
public:
	bool m_on;
	MS3DModel m_ms3d;
	VertexArray* m_va;
	unsigned int m_diffusem;
	unsigned int m_specularm;
	unsigned int m_normalm;
	unsigned int m_ownerm;

	Model()
	{
		m_va = NULL;
		m_on = false;
	}

	~Model();

	void load(const char* filepath, Vec3f scale, Vec3f translate);
	void usetex();
	void draw(int frame, Vec3f pos, float yaw);
};

#define MODELS	512
extern Model g_model[MODELS];

struct ModelToLoad
{
	int* id;
	char filepath[MAX_PATH];
	Vec3f scale;
	Vec3f translate;
};

int NewModel();
void QueueModel(int* id, const char* filepath, Vec3f scale, Vec3f translate);
bool Load1Model();
void DrawVA(VertexArray* va, Vec3f pos);
void BeginVertexArrays();
void EndVertexArrays();
bool PlayAnimation(float& frame, int first, int last, bool loop, float rate);
bool PlayAnimationB(float& frame, int first, int last, bool loop, float rate);	//Play animation backwards

#endif