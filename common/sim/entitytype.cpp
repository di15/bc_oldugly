

#include "entitytype.h"

EntityT g_entityT[ENTITY_TYPES];

EntityT::EntityT()
{
	model = -1;
	collider = -1;
	item = -1;
}

EntityT::EntityT(
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
	float turnradius)
{
	this->collider = collider;
	QueueModel(&this->model, model, modelScale, modelOffset);
	this->vmin = vmin;
	this->vmax = vmax;
	this->maxStep = maxStep;
	this->speed = speed;
	this->jump = jump;
	this->crouch = crouch;
	this->animRate = animRate;
	this->item = item;
	this->isactor = isactor;
	this->turnradius = turnradius;
}