

#include "../sim/entity.h"
#include "../sim/entitytype.h"
#include "movement.h"
#include "../sim/map.h"
#include "../phys/physics.h"
#include "../phys/tracework.h"

void MoveEntity(Entity* pent, int entindex)
{
	Camera* cam = &pent->camera;
	EntityT* t = &g_entityT[pent->type];

	Vec3f bodymin = pent->bodymin();
	Vec3f bodymax = t->vmax;
	Vec3f old = cam->m_pos;
	float speed = t->speed;
	//float speed = 0;

	if(pent->forward)
		cam->accelerate(speed);
	if(pent->backward)
		cam->accelerate(-speed);
	if(pent->left)
		cam->accelstrafe(-speed);
	if(pent->right)
		cam->accelstrafe(speed);
	if(pent->jump && cam->m_grounded)
		cam->accelrise(t->jump);

#if 0
	if(pent->crouching && !pent->crouched)
	{
		if(cam->m_grounded)
		{
			old.y -= t->crouch;
			cam->moveto(old);
		}

		pent->crouched = true;
		pent->run = false;
	}
	if(pent->crouched)
		speed /= 2.0f;
	else if(pent->run && pent->stamina > 0.0f)
		speed *= 2.0f;
#endif

	cam->limithvel(speed);
	cam->frameupd();

	cam->grounded(false);

	TraceWork tw;

	g_map.tracebox(&tw, old, cam->m_pos, bodymin, bodymax, t->maxStep);

#if 0
	if(tw.ladder)
	{
		Vec3f v = cam->m_vel;
		v.y += (cam->m_view.y - cam->m_pos.y) * 50.0f;
		v.y = Clipf(v.y, -speed/3.0f, speed/3.0f);
		cam->m_vel = v;
	}
#endif

	cam->moveto(tw.clip);

#if 0
	int collided = -1;
	if(collided = CollisionResponse(pent, entindex, cam, old))
		continue;
#endif

	if(tw.onground)
		cam->grounded(true);

	if(!cam->m_grounded && !tw.atladder)
		cam->accelrise(-GRAVITY);

	cam->friction();
}