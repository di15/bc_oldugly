



#include "map.h"
#include "../draw/shader.h"
#include "../texture.h"
#include "../utils.h"

Map g_map;

Map::Map()
{
	m_brush = NULL;
	m_nbrush = 0;
}

Map::~Map()
{
	destroy();
}

void Map::destroy()
{
	for(int brushidx = 0; brushidx < m_nbrush; brushidx++)
	{
		Brush* pbrush = &m_brush[brushidx];

		for(int sideidx=0; sideidx < pbrush->m_nsides; sideidx++)
		{
			BrushSide* s = &pbrush->m_sides[sideidx];

			if(s->m_diffusem != 0)
				FreeTexture(s->m_diffusem);
			if(s->m_specularm != 0)
				FreeTexture(s->m_specularm);
			if(s->m_normalm != 0)
				FreeTexture(s->m_normalm);
		}
	}

	if(m_brush)
	{
		delete [] m_brush;
		m_brush = NULL;
	}

	m_nbrush = 0;
	m_transpbrush.clear();
	m_opaquebrush.clear();
	m_skybrush.clear();
}

/*
void LogBrush()
{
	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];
		Vec3f* un = &s->m_tceq[0].m_normal;
		Vec3f* vn = &s->m_tceq[1].m_normal;
		float ud = s->m_tceq[0].m_d;
		float vd = s->m_tceq[1].m_d;

		for(int j=0; j<s->m_ntris; j++)
		{
			Triangle2* t = &s->m_tris[j];

			//for(int j=0; j<va->numverts; j++)
			for(int j=0; j<3; j++)
			{
		Vec2f* tc = &t->m_texcoord[j];
				//g_log<<"u "<<va->texcoords[j].x<<"\t	v "<<va->texcoords[j].y<<endl;
				g_log<<"u "<<tc->x<<"\t	v "<<tc->y<<endl;
				g_log.flush();
			}
		}

		s->makeva();
	}
	}
}*/

//Draw opaque brushes first
void DrawMap(Map* map)
{

	//return;
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	Shader* shader = &g_shader[g_curS];

	for(auto brushiterator = map->m_opaquebrush.begin(); brushiterator != map->m_opaquebrush.end(); brushiterator++)
	{
		int brushindex = *brushiterator;
#if 0
		g_log<<"draw brush "<<brushidx<<endl;
#endif

		Brush* b = &map->m_brush[brushindex];
		Texture* t = &g_texture[b->m_texture];

		//TO DO: Replace with index table look-ups
		if(t->sky)
			continue;

		for(int sideindex = 0; sideindex < b->m_nsides; sideindex++)
		{
#if 0
			g_log<<"\tdraw side "<<sideindex<<endl;
#endif

			BrushSide* pside = &b->m_sides[sideindex];
			/*
			CreateTexture(side->m_diffusem, "gui/frame.jpg", false);
			side->m_diffusem = g_texindex;*/
			pside->usetex();
			/*
			unsigned int atex;
			CreateTexture(atex, "gui/dmd.jpg", false);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atex);
	glUniform1iARB(g_shader[g_curS].m_slot[SSLOT_TEXTURE0], 0);
	*/
			VertexArray* va = &pside->m_drawva;

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->tangents);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			glDrawArrays(GL_TRIANGLES, 0, va->numverts);

#if 0
			for(int vertindex = 0; vertindex < va->numverts; vertindex++)
			{
				g_log<<"\t\tvert: ("<<va->vertices[vertindex].x<<","<<va->vertices[vertindex].y<<","<<va->vertices[vertindex].z<<")"<<endl;
			}

			g_log<<"\tdrew triangles: "<<(va->numverts/3)<<endl;
#endif
		}
	}
}


// Draw transparent faces on top of all models and opaque brushes
void DrawMap2(Map* map)
{
	//return;
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	//Reset the model matrix from the model rotations and translations

	Shader* pshader = &g_shader[g_curS];
	Matrix modelmat;
	modelmat.loadIdentity();
    glUniformMatrix4fv(pshader->m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);

	Shader* shader = &g_shader[g_curS];

	for(auto brushiterator = map->m_transpbrush.begin(); brushiterator != map->m_transpbrush.end(); brushiterator++)
	{
		int brushindex = *brushiterator;
#if 0
		g_log<<"draw brush "<<brushidx<<endl;
#endif

		Brush* b = &map->m_brush[brushindex];
		Texture* t = &g_texture[b->m_texture];

		//TO DO: Replace with index table look-ups
		if(t->sky)
			continue;

		for(int sideindex = 0; sideindex < b->m_nsides; sideindex++)
		{
#if 0
			g_log<<"\tdraw side "<<sideindex<<endl;
#endif

			BrushSide* pside = &b->m_sides[sideindex];
			/*
			CreateTexture(side->m_diffusem, "gui/frame.jpg", false);
			side->m_diffusem = g_texindex;*/
			pside->usetex();
			/*
			unsigned int atex;
			CreateTexture(atex, "gui/dmd.jpg", false);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atex);
	glUniform1iARB(g_shader[g_curS].m_slot[SSLOT_TEXTURE0], 0);
	*/
			VertexArray* va = &pside->m_drawva;

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->tangents);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			glDrawArrays(GL_TRIANGLES, 0, va->numverts);

#if 0
			for(int vertindex = 0; vertindex < va->numverts; vertindex++)
			{
				g_log<<"\t\tvert: ("<<va->vertices[vertindex].x<<","<<va->vertices[vertindex].y<<","<<va->vertices[vertindex].z<<")"<<endl;
			}

			g_log<<"\tdrew triangles: "<<(va->numverts/3)<<endl;
#endif
		}
	}
}
