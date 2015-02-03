

#include "../draw/model.h"
#include "../math/brush.h"
#include "entitytype.h"
#include "entity.h"
#include "../texture.h"
#include "selection.h"

int themodel;
float g_maxelev = 10000;

#if 0
class Kilobyte
{
public:
	char bytes[1024];

	Kilobyte(){}
	~Kilobyte(){}
};
#endif

void InitOnce()
{
	for(int i=0; i<ENTITIES; i++)
	{
		g_entity[i] = NULL;
	}
		
	themodel = 0;
	
	float fcharscale = 2.57f;
	//float fcharscale = 10.57f;
	Vec3f vcharscale = Vec3f(fcharscale, fcharscale, fcharscale);
	Vec3f charMax = Vec3f(10.0f, 3.4f, 10.0f) * fcharscale;
	Vec3f charMin = Vec3f(-10.0f, -69.9f, -10.0f) * fcharscale;
	float charMaxStep = 50.0f;
	//float charMaxStep = 500.0f;
	float charSpeed = 10.0f;
	//float charSpeed = 50.0f;
	float charJump = 200.0f;
	float charCrouch = (charMax.y-charMin.y)/3.0f;
	float charAnimRate = 1.0f;
	float charTurnRadius = 5;

	//g_entityT[ENTITY_BATTLECOMPUTER] = EntityT(-1, "models/battlecomp/battlecomp.ms3d", vcharscale, Vec3f(0,0,0), charMin, charMax, charMaxStep, charSpeed, charJump, charCrouch, charAnimRate, -1, true, charTurnRadius);
	//g_entityT[ENTITY_BATTLECOMPUTER] = EntityT(-1, "models/domhoff/domhoff.ms3d", vcharscale, Vec3f(0,0,0), charMin, charMax, charMaxStep, charSpeed, charJump, charCrouch, charAnimRate, -1, true, charTurnRadius);
	g_entityT[ENTITY_BATTLECOMPUTER] = EntityT(-1, "models/battlecompb/battlecomp.ms3d", vcharscale, Vec3f(0,0,0), charMin, charMax, charMaxStep, charSpeed, charJump, charCrouch, charAnimRate, -1, true, charTurnRadius);

}

void Queue()
{
	// 73 units to 188 cm (2.57 ratio)
	//QueueModel(&themodel, "models/battlecomp/battlecomp.ms3d", Vec3f(2.57f,2.57f,2.57f), Vec3f(0,188,0));

	QueueTexture(&g_circle, "gui/circle.png", true);
}