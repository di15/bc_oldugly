

#include "edmap.h"
#include "../draw/shader.h"
#include "../texture.h"
#include "../utils.h"

EdMap g_edmap;
vector<Brush*> g_selB;
Brush* g_sel1b = NULL;	//drag selected brush (brush being dragged or manipulated currently)
int g_dragV = -1;	//drag vertex of selected brush?
int g_dragS = -1;	//drag side of selected brush?
bool g_dragB = false;	//drag brush?
int g_dragD = -1;	// dragging DRAG_DOOR_POINT or DRAG_DOOR_AXIS ?

void DrawEdMap(EdMap* map, bool showsky)
{
	//return;
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	Shader* shader = &g_shader[g_curS];

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		Texture* t = &g_texture[b->m_texture];

		if(t->sky && !showsky)
			continue;

		for(int i=0; i<b->m_nsides; i++)
		{
			BrushSide* side = &b->m_sides[i];
			/*
			CreateTexture(side->m_diffusem, "gui/frame.jpg", false);
			side->m_diffusem = g_texindex;*/
			side->usetex();
			/*
			unsigned int atex;
			CreateTexture(atex, "gui/dmd.jpg", false);
	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, atex);
	glUniform1iARB(g_shader[g_curS].m_slot[SSLOT_TEXTURE0], 0);
	*/
			VertexArray* va = &side->m_drawva;

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->tangents);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TANGENT], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			glDrawArrays(GL_TRIANGLES, 0, va->numverts);
		}
	}
}
