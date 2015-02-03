

#include "../sim/map.h"
#include "tracework.h"
#include "../texture.h"
#include "../sim/order.h"
#include "../utils.h"
#include "../save/edmap.h"

void Map::tracebox(TraceWork *tw, Vec3f start, Vec3f end, Vec3f vmin, Vec3f vmax, float maxstep)
{
	tw->clip = end;
	tw->atladder = false;
	tw->onground = false;
	tw->trytostep = false;
	tw->collided = false;

	TraceJob tj;
	tj.type = TRACE_BOX;
	tj.vmin = vmin;
	tj.vmax = vmax;
	tj.start = start;
	tj.end = end;
	tj.absstart = start;
	tj.absend = end;
	tj.extents = Vec3f( 
		-vmin.x > vmax.x ? -vmin.x : vmax.x, 
		-vmin.y > vmax.y ? -vmin.y : vmax.y, 
		-vmin.z > vmax.z ? -vmin.z : vmax.z );
	tj.maxstep = maxstep;

	trace(tw, &tj);

	if(tw->collided && tw->trytostep)
	{
		tj.start = tw->clip;
		trytostep(tw, &tj);
	}

#define ANTIFALL
	
#ifdef ANTIFALL
	tw->stuck = false;

	tj.start = tw->clip;
	trace(tw, &tj);

	if(tw->stuck)
		tw->clip = start;
#endif
}

#ifdef BCOLDEBUG
static bool debugb = false;
#endif

void Map::checkbrush(TraceWork* tw, TraceJob* tj, int brushindex)
{
	float startratio = -1.0f;
	float endratio = 1.0f;
	bool startsout = false;

	Brush* b = &m_brush[brushindex];

	Texture* brushtex = &g_texture[b->m_texture];

	if(brushtex->passthru)
		return;
	
#ifdef BCOLDEBUG
	int relativebrush = b - m_brush;

	//bool debugb = false;

	BrushSide* colbs;
#endif

	for(int i = 0; i < b->m_nsides; i++)
	{
#if 0
		InfoMessage("asd", "co2");
#endif

		BrushSide* s = &b->m_sides[i];
		Plane* p = &s->m_plane;

		float offset = 0;

		if(tj->type == TRACE_SPHERE)
			offset = tj->radius;
		
		float startdistance = Dot(tj->absstart, p->m_normal) + (p->m_d + offset);
		float enddistance = Dot(tj->absend, p->m_normal) + (p->m_d + offset);

		Vec3f voffset(0,0,0);

		if(tj->type == TRACE_BOX)
		{
#if 0
			InfoMessage("asd", "co3");
#endif

			voffset.x = (p->m_normal.x < 0) ? tj->vmax.x : tj->vmin.x;
			voffset.y = (p->m_normal.y < 0) ? tj->vmax.y : tj->vmin.y;
			voffset.z = (p->m_normal.z < 0) ? tj->vmax.z : tj->vmin.z;
			
			startdistance = Dot(tj->start + voffset, p->m_normal) + p->m_d;
			enddistance = Dot(tj->end + voffset, p->m_normal) + p->m_d;
		}

		if(startdistance > 0)	startsout = true;

		if(startdistance > 0 && enddistance > 0)
			return;

		if(startdistance <= 0 && enddistance <= 0)
			continue;

		if(startdistance > enddistance)
		{
			float ratio1 = (startdistance - EPSILON) / (startdistance - enddistance);

			if(ratio1 > startratio)
			{
				startratio = ratio1;

				tw->collisionnormal = p->m_normal;
				
#ifdef BCOLDEBUG
				colbs = s;
#endif
				
				if((tj->start.x != tj->end.x || tj->start.z != tj->end.z) && p->m_normal.y != 1 && p->m_normal.y >= 0.0f)
				//if((tj->start.x != tj->end.x || tj->start.z != tj->end.z))
				{
					tw->trytostep = true;

					//if(debugb)
					//	InfoMessage("asd", "try step");
				}

				if(tw->collisionnormal.y > 0.2f)
					tw->onground = true;
			}
		}
		else
		{
			float ratio = (startdistance + EPSILON) / (startdistance - enddistance);

			if(ratio < endratio)
				endratio = ratio;
		}
	}

	tw->collided = true;
	
#ifdef BCOLDEBUG
	if(relativebrush == 462)
		debugb = true;
#endif

	Texture* ptex = &g_texture[b->m_texture];

	if(ptex->ladder)
		tw->atladder = true;

	if(startsout == false)
	{
		tw->stuck = true;
		return;
	}

	if(startratio < endratio)
	{
		//if(startratio > -1 && startratio < tw->traceratio)
		if(startratio > -1)
		{
			if(startratio < 0)
				startratio = 0;
			
#if 0
			g_order.push_back(OrderMarker(colbs->m_centroid, GetTickCount(), 100));

			InfoMessage("asd", "co");
#endif

			g_selB.push_back(b);

			tw->traceratio = startratio;
		}
	}
}

void Map::trace(TraceWork* tw, TraceJob* tj)
{
	tw->traceratio = 1;

	//work goes here
	for(int brushindex = 0; brushindex < m_nbrush; brushindex++)
		checkbrush(tw, tj, brushindex);

	if(tw->traceratio == 1)
	{
		tw->clip = tj->end;
		return;
	}
	else
	{
		Vec3f newpos = tj->start + ((tj->end - tj->start) * tw->traceratio);

		if(tj->type == TRACE_RAY)
		{
			tw->clip = newpos;
			return;
		}

		Vec3f move = tj->end - newpos;

		float distance = Dot(move, tw->collisionnormal);
		
		Vec3f endpos = tj->end - tw->collisionnormal*distance;
		//Vec3f endpos = tj->end;	// - tw->collisionnormal*distance;

		TraceJob subtj = *tj;

		subtj.start = newpos;
		subtj.end = endpos;

		trace(tw, &subtj);

		if(tw->collisionnormal.y > 0.2f || tw->onground)
			tw->onground = true;
		else
			tw->onground = false;
	}
}

void Map::trytostep(TraceWork* tw, TraceJob* tj)
{
	for(float height = 1; height <= tj->maxstep; height += 1)
	//float height = 1;
	{
		tw->collided = false;
		tw->trytostep = false;
		
		Vec3f stepstart = tj->start + Vec3f(0, height, 0);
		Vec3f stepend = tj->end + Vec3f(0, height, 0);

		Vec3f oldclip = tw->clip;
		Vec3f oldstart = tj->start;
		Vec3f oldend = tj->end;

		tj->start = stepstart;
		tj->end = stepend;

		trace(tw, tj);

		tj->start = oldstart;
		tj->end = oldend;
		
#ifdef BCOLDEBUG
		if(debugb)
		{
			g_log<<"height: "<<height<<endl;
			g_log.flush();
		}
#endif

		if(!tw->collided)
		{
			//tw->clip = tj->end;
			
			tj->start = stepstart;
			tj->end = stepend;

#ifdef BCOLDEBUG
		if(debugb)
		{
			g_log<<"no col!: "<<height<<" ("<<stepstart.x<<","<<stepstart.z<<")->("<<stepend.x<<","<<stepend.z<<")"<<endl;
			g_log<<"no col!: "<<height<<" ("<<tj->start.x<<","<<tj->start.z<<")->("<<tj->end.x<<","<<tj->end.z<<")"<<endl;
			g_log<<"no col!: "<<height<<" ("<<tj->absstart.x<<","<<tj->absstart.z<<")->("<<tj->absend.x<<","<<tj->absend.z<<")"<<endl;
			g_log.flush();
		}
	if(debugb)
		debugb = false;
#endif

			return;
		}
		
		tw->clip = oldclip;
	}

	tw->clip = tj->start;
	
#ifdef BCOLDEBUG
		if(debugb)
		{
			g_log<<"still col"<<endl;
			g_log.flush();
		}

	if(debugb)
		debugb = false;
#endif
}