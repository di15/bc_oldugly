

#include "ai.h"
#include "../sim/entity.h"
#include "../sim/entitytype.h"
#include "../utils.h"

bool IsActor(Entity* pent)
{
	EntityT* t = &g_entityT[pent->type];
	return t->isactor;
}

#define GOAL_PROXY	200

void UpdateAI(Entity* pent)
{
	Camera* cam = &pent->camera;
	EntityT* t = &g_entityT[pent->type];
	
	pent->forward = false;
	
	//if(cam->m_pos != pent->goal)
	//if(Magnitude3(cam->m_pos - pent->goal) > GOAL_PROXY*GOAL_PROXY)
	if(Magnitude2(cam->m_pos - pent->goal) > GOAL_PROXY*GOAL_PROXY)
	{
		//Vec3f goaldir = Normalize( pent->goal - cam->m_pos );
		float dyaw = DYaw(cam, pent->goal);

		//float turn = Clipf(dyaw, -DEGTORAD(t->turnradius), DEGTORAD(t->turnradius));

		//cam->rotateview(turn, 0, 1, 0);
		//cam->rotateview(dyaw, 0, 1, 0);

		cam->m_view = pent->goal;
		cam->calcstrafe();
		cam->calcyaw();
		cam->calcpitch();

		//if(RADTODEG(abs(dyaw)) < 30)
			//cam->accelerate(t->speed);
			pent->forward = true;
	}
}