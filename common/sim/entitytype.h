

#include "../platform.h"
#include "../math/vec3f.h"
#include "../sound.h"
#include "../phys/collider.h"
#include "../draw/model.h"

class EntityT
{
public:
	int collider;
	int model;
	Vec3f vmin, vmax;
	float maxStep;
	float speed;
	float jump;
	float crouch;
	float animRate;
	int item;
	Vec3f centerOff;
	//vector<Sound> openSound;
	//vector<Sound> closeSound;
	bool isactor;
	float turnradius;	//degrees

	EntityT();
	EntityT(
	int collider, 
	const char* model, 
	Vec3f modelScale, 
	Vec3f modelOffset, 
	Vec3f vmin, 
	Vec3f vmax, 
	float maxStep, 
	float speed, 
	float jump, 
	float crouch, 
	float animRate, 
	int item, 
	bool isactor, 
	float turnradius);
};

#define ENTITY_TYPES	3
extern EntityT g_entityT[ENTITY_TYPES];
#define ENTITY_BATTLECOMPUTER		0
#define ENTITY_STATESOLDIER			1
#define ENTITY_OFFICETABLE1			2