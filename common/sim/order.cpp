

#include "order.h"
#include "../common/draw/shader.h"
#include "../common/sim/selection.h"
#include "../common/sim/entity.h"
#include "../common/sim/entitytype.h"
#include "../common/sim/map.h"
#include "../common/sim/sim.h"
#include "../common/window.h"
#include "../common/texture.h"
#include "../common/utils.h"

list<OrderMarker> g_order;

void DrawOrders(Matrix* projection, Matrix* modelmat, Matrix* viewmat)
{
	UseS(SHADER_BILLBOARDPERSP);
	//glBegin(GL_QUADS);
	
	OrderMarker* o;
	Vec3f p;
	float r;
	float a;

	Shader* s = &g_shader[g_curS];
	
	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, 0, projection->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, 0, viewmat->m_matrix);
	glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, 1);
	glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	glEnableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);

	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_circle ].texname);
	glUniform1iARB(s->m_slot[SSLOT_TEXTURE0], 0);

	auto oitr = g_order.begin();

	while(oitr != g_order.end())
	{
		o = &*oitr;
		p = o->pos;
		r = o->radius;
		a = 1.0f - (float)(GetTickCount() - o->tick)/(float)ORDER_EXPIRE;

		/*
		glColor4f(1, 1, 1, a);
	
		glTexCoord2f(0, 0);		glVertex3f(p.x - r, p.y + 1, p.z - r);
		glTexCoord2f(0, 1);		glVertex3f(p.x - r, p.y + 1, p.z + r);
		glTexCoord2f(1, 1);		glVertex3f(p.x + r, p.y + 1, p.z + r);
		glTexCoord2f(1, 0);		glVertex3f(p.x + r, p.y + 1, p.z - r);
		*/
		
		glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, a);

        float vertices[] =
        {
            //posx, posy posz   texx, texy
            p.x + r, p.y + 1, p.z - r,          1, 0,
            p.x + r, p.y + 1, p.z + r,          1, 1,
            p.x - r, p.y + 1, p.z + r,          0, 1,
            
            p.x - r, p.y + 1, p.z + r,          0, 1,
            p.x - r, p.y + 1, p.z - r,          0, 0,
            p.x + r, p.y + 1, p.z - r,          1, 0
        };
		
		//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);
		
		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[0]);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[3]);
		//glVertexAttribPointer(s->m_slot[SLOT::NORMAL], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, va->normals);

        glDrawArrays(GL_TRIANGLES, 0, 6);

		if(GetTickCount() - o->tick > ORDER_EXPIRE)
		{
			oitr = g_order.erase(oitr);
			continue;
		}

		oitr++;
	}

	//glEnd();
	//glColor4f(1, 1, 1, 1);
	//glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void Order(int mousex, int mousey, int viewwidth, int viewheight, Vec3f campos, Vec3f camfocus, Vec3f camviewdir, Vec3f camside, Vec3f camup2)
{
#if 0
	InfoMessage("asd", "ord");
#endif

	Vec3f line[2];

	Vec3f ray = ScreenPerspRay(mousex, mousey, viewwidth, viewheight, campos, camside, camup2, camviewdir, FIELD_OF_VIEW);
	
	line[0] = campos;
	line[1] = campos + (ray * 10000.0f);

	bool foundsurface = false;

	for(int brushindex = 0; brushindex < g_map.m_nbrush; brushindex++)
	{
		Brush* pbrush = &g_map.m_brush[brushindex];

		unsigned int texindex = pbrush->m_texture;
		Texture* ptex = &g_texture[texindex];

		if(ptex->sky)
			continue;

		Vec3f internormal;
		Vec3f trace = pbrush->traceray(line, &internormal);

		if(trace != line[1] && trace.y <= g_maxelev && internormal.y >= 0.2f)
		{
			line[1] = trace;
			foundsurface = true;
		}
	}

	if(!foundsurface)
		return;

#if 0
	InfoMessage("asd", "found surf");
#endif

	//vector<int> selection = g_selection;
	//int selecttype = g_selectType;

	//g_selection.clear();

	//SelectOne();

	//vector<int> targets = g_selection;
	//int targettype = g_selectType;

	//g_selection = selection;
	//g_selectType = selecttype;

	//char msg[128];
	//sprintf(msg, "s.size()=%d, stype=%d", (int)g_selection.size(), g_selectType);
	//Chat(msg);

	Vec3f vmin = g_map.m_max;
	Vec3f vmax = g_map.m_min;
	Vec3f center(0,0,0);

#if 0
		char msg[256];
		sprintf(msg, "mapmaxmin:(%f,%f)->(%f,%f)", vmin.x, vmin.z, vmax.x, vmax.z);
		InfoMessage("asd", msg);
#endif

	//if(targets.size() <= 0 || (targettype != SELECT_UNIT && targettype != SELECT_BUILDING))
	{
		int i;
		Entity* e;
		EntityT* t;

		Vec3f order = line[1];
	
	#if 0
		g_order.push_back(OrderMarker(order, GetTickCount(), 100));
	#endif
		//order.x = Clip(order.x, 0, g_hmap.m_widthX*TILE_SIZE);
		//order.z = Clip(order.z, 0, g_hmap.m_widthZ*TILE_SIZE);
		Vec3f p;

		auto selitr = g_selection.begin();
		int counted = 0;

		while(selitr != g_selection.end())
		{
			e = g_entity[*selitr];

#if 0
			if(e->hp <= 0.0f)
			{
				g_selection.erase( selitr );
				continue;
			}
#endif

			p = e->camera.m_pos;
			//e->target = -1;
			//e->underOrder = true;

			if(p.x < vmin.x)
				vmin.x = p.x;
			if(p.z < vmin.z)
				vmin.z = p.z;
			if(p.x > vmax.x)
				vmax.x = p.x;
			if(p.z > vmax.z)
				vmax.z = p.z;
		
			//center = center + p;
			center = (center * counted + p) / (float)(counted+1);

			counted ++;
			selitr++;
		}

		//center = center / (float)g_selection.size();
		//Vec3f half = (min + max) / 2.0f;

#if 0
		char msg[256];
		sprintf(msg, "minmax:(%f,%f)->(%f,%f),order:(%f,%f)", vmin.x, vmin.z, vmax.x, vmax.z, order.x, order.z);
		InfoMessage("asd", msg);
#endif

		// All units to one goal
		//if(fabs(center.x - order.x) < half.x && fabs(center.z - order.z) < half.z)
		if(order.x <= vmax.x && order.x >= vmin.x && order.z <= vmax.z && order.z >= vmin.z)
		{	
#if 0
			InfoMessage("asd", "typ 1");
#endif
			
			float radius = 0;
			selitr = g_selection.begin();

			// Get the biggest unit width/radius
			selitr = g_selection.begin();
			while(selitr != g_selection.end())
			{
				e = g_entity[*selitr];
				e->goal = order;
				//e->underorder = true;
				t = &g_entityT[e->type];
				if(t->vmax.x > radius)
					radius = t->vmax.x;
				//e->nopath = false;
				selitr++;
			}

			g_order.push_back(OrderMarker(order, GetTickCount(), radius*3));
		}
		// Formation goal
		else
		{
#if 0
			InfoMessage("asd", "typ 2");
#endif

			Vec3f offset;
			
			selitr = g_selection.begin();

			while(selitr != g_selection.end())
			{
				e = g_entity[*selitr];
				p = e->camera.m_pos;
				offset = p - center;
				e->goal = order + offset;
				t = &g_entityT[e->type];
				float radius = t->vmax.x;
				//u->goal.x = Clip(u->goal.x, 0 + radius, g_hmap.m_widthX*TILE_SIZE - radius);
				//u->goal.z = Clip(u->goal.z, 0 + radius, g_hmap.m_widthZ*TILE_SIZE - radius);
				//u->nopath = false;
				g_order.push_back(OrderMarker(e->goal, GetTickCount(), radius*3));
				selitr++;
			}
		}
	
		//AckSnd();
	}
#if 0
	else if(targets.size() > 0 && targettype == SELECT_UNIT)
	{
		int targi = targets[0];
		CUnit* targu = &g_unit[targi];
		Vec3f p = targu->camera.Position();

		if(targu->owner == g_localP)
			return;

		MakeWar(g_localP, targu->owner);

		for(int j=0; j<g_selection.size(); j++)
		{
			int i = g_selection[j];
			CUnit* u = &g_unit[i];
			u->goal = p;
			u->underOrder = true;
			u->targetU = true;
			u->target = targi;
		}
		
		AckSnd();
	}
	else if(targets.size() > 0 && targettype == SELECT_BUILDING)
	{
		int targi = targets[0];
		CBuilding* targb = &g_building[targi];
		Vec3f p = targb->pos;

		if(targb->owner == g_localP)
			return;

		MakeWar(g_localP, targb->owner);

		for(int j=0; j<g_selection.size(); j++)
		{
			int i = g_selection[j];
			CUnit* u = &g_unit[i];
			u->goal = p;
			u->underOrder = true;
			u->targetU = false;
			u->target = targi;
		}
		
		AckSnd();
	}
#endif
}