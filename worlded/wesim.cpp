

#include "wesim.h"
#include "../common/draw/shader.h"
#include "../common/platform.h"
#include "../common/utils.h"
#include "../common/math/3dmath.h"
#include "../common/math/vec4f.h"
#include "weviewport.h"
#include "../common/gui/gui.h"
#include "../common/sim/sim.h"
#include "wegui.h"
#include "../common/sim/door.h"

Brush g_copyB;
int g_edtool = EDTOOL_NONE;

// draw selected brushes filled bg
void DrawFilled(EdMap* map)
{
	//UseS(SHADER_COLOR3D);
	Shader* shader = &g_shader[g_curS];
	glUniform4f(shader->m_slot[SSLOT_COLOR], 0.2f, 0.3f, 0.9f, 0.5f);

	for(int i=0; i<g_selB.size(); i++)
	{
		Brush* b = g_selB[i];

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* side = &b->m_sides[j];

			//glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			//glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			//glDrawArrays(GL_TRIANGLES, 0, va->numverts);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, side->m_drawva.vertices);

			glDrawArrays(GL_TRIANGLES, 0, side->m_drawva.numverts);
		}
	}
}

// draw brush outlines
void DrawOutlines(EdMap* map)
{
	//UseS(SHADER_COLOR3D);
	Shader* shader = &g_shader[g_curS];

	//glUniform4f(shader->m_slot[SSLOT_COLOR], 0.2f, 0.9f, 0.3f, 0.75f);

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		for(int i=0; i<b->m_nsides; i++)
		{
			BrushSide* side = &b->m_sides[i];

			//glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			//glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			//glDrawArrays(GL_TRIANGLES, 0, va->numverts);

			float alpha = 1.0f - Magnitude2(side->m_centroid - g_camera.m_view) / (PROJ_RIGHT*2.0f/g_zoom) / (PROJ_RIGHT*2.0f/g_zoom);

			glUniform4f(shader->m_slot[SSLOT_COLOR], 0.2f, 0.9f, 0.3f, alpha);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, side->m_outline.m_drawoutva);

			glDrawArrays(GL_LINE_STRIP, 0, side->m_outline.m_edv.size());
		}
	}
}

// draw selected brush outlines
void DrawSelOutlines(EdMap* map)
{
	//UseS(SHADER_COLOR3D);
	Shader* shader = &g_shader[g_curS];
	glUniform4f(shader->m_slot[SSLOT_COLOR], 0.2f, 0.9f, 0.3f, 0.75f);

	for(auto bi=g_selB.begin(); bi!=g_selB.end(); bi++)
	{
		Brush* b = *bi;

		for(int i=0; i<b->m_nsides; i++)
		{
			BrushSide* side = &b->m_sides[i];

			//glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			//glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			//glDrawArrays(GL_TRIANGLES, 0, va->numverts);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, side->m_outline.m_drawoutva);

			glDrawArrays(GL_LINE_STRIP, 0, side->m_outline.m_edv.size());
		}
	}
}

void DrawDrag_Door(EdMap* map, Matrix* mvp, int w, int h, bool persp)
{
	Shader* shader = &g_shader[g_curS];

	Brush* b = *g_selB.begin();

	EdDoor* door = b->m_door;

	if(!door)
		return;

	Vec3f startpoint = door->point;
	Vec3f axispoint = door->point + door->axis;

	Vec4f startscreenpos = ScreenPos(mvp, startpoint, w, h, persp);
	Vec4f axisscreenpos = ScreenPos(mvp, axispoint, w, h, persp);

	{
		float verts[] =
		{
			startscreenpos.x, startscreenpos.y, 0,
			axisscreenpos.x, axisscreenpos.y, 0
		};

		float colour2[] = DOOR_POINT_DRAG_FILLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_LINE_STRIP, 0, 2);

	}

	{
		float verts[] =
		{
			startscreenpos.x - DOOR_POINT_DRAG_HSIZE, startscreenpos.y - DOOR_POINT_DRAG_HSIZE, 0,
			startscreenpos.x + DOOR_POINT_DRAG_HSIZE, startscreenpos.y - DOOR_POINT_DRAG_HSIZE, 0,
			startscreenpos.x + DOOR_POINT_DRAG_HSIZE, startscreenpos.y + DOOR_POINT_DRAG_HSIZE, 0,
			startscreenpos.x - DOOR_POINT_DRAG_HSIZE, startscreenpos.y + DOOR_POINT_DRAG_HSIZE, 0,
			startscreenpos.x - DOOR_POINT_DRAG_HSIZE, startscreenpos.y - DOOR_POINT_DRAG_HSIZE, 0
		};

		float colour[] = DOOR_POINT_DRAG_FILLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_QUADS, 0, 4);

		float colour2[] = DOOR_POINT_DRAG_OUTLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}

	{
		float verts[] =
		{
			axisscreenpos.x - DOOR_AXIS_DRAG_HSIZE, axisscreenpos.y - DOOR_AXIS_DRAG_HSIZE, 0,
			axisscreenpos.x + DOOR_AXIS_DRAG_HSIZE, axisscreenpos.y - DOOR_AXIS_DRAG_HSIZE, 0,
			axisscreenpos.x + DOOR_AXIS_DRAG_HSIZE, axisscreenpos.y + DOOR_AXIS_DRAG_HSIZE, 0,
			axisscreenpos.x - DOOR_AXIS_DRAG_HSIZE, axisscreenpos.y + DOOR_AXIS_DRAG_HSIZE, 0,
			axisscreenpos.x - DOOR_AXIS_DRAG_HSIZE, axisscreenpos.y - DOOR_AXIS_DRAG_HSIZE, 0
		};

		float colour[] = DOOR_AXIS_DRAG_FILLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_QUADS, 0, 4);

		float colour2[] = DOOR_AXIS_DRAG_OUTLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}
}

void DrawDrag_VertFaceBrush(EdMap* map, Matrix* mvp, int w, int h, bool persp)
{
	Shader* shader = &g_shader[g_curS];

	for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
	{
		Brush* b = *i;

		for(int j=0; j<b->m_nsharedv; j++)
		{
			Vec3f sharedv = b->m_sharedv[j];
			Vec4f screenpos = ScreenPos(mvp, sharedv, w, h, persp);

			float verts[] =
			{
				screenpos.x - VERT_DRAG_HSIZE, screenpos.y - VERT_DRAG_HSIZE, 0,
				screenpos.x + VERT_DRAG_HSIZE, screenpos.y - VERT_DRAG_HSIZE, 0,
				screenpos.x + VERT_DRAG_HSIZE, screenpos.y + VERT_DRAG_HSIZE, 0,
				screenpos.x - VERT_DRAG_HSIZE, screenpos.y + VERT_DRAG_HSIZE, 0,
				screenpos.x - VERT_DRAG_HSIZE, screenpos.y - VERT_DRAG_HSIZE, 0
			};

			float colour[] = VERT_DRAG_FILLRGBA;
			glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
			glDrawArrays(GL_QUADS, 0, 4);

			float colour2[] = VERT_DRAG_OUTLRGBA;
			glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
			glDrawArrays(GL_LINE_STRIP, 0, 5);
		}
	}

	for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
	{
		Brush* b = *i;

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* side = &b->m_sides[j];
			Vec4f screenpos = ScreenPos(mvp, side->m_centroid, w, h, persp);

			float verts[] =
			{
				screenpos.x - FACE_DRAG_HSIZE, screenpos.y - FACE_DRAG_HSIZE, 0,
				screenpos.x + FACE_DRAG_HSIZE, screenpos.y - FACE_DRAG_HSIZE, 0,
				screenpos.x + FACE_DRAG_HSIZE, screenpos.y + FACE_DRAG_HSIZE, 0,
				screenpos.x - FACE_DRAG_HSIZE, screenpos.y + FACE_DRAG_HSIZE, 0,
				screenpos.x - FACE_DRAG_HSIZE, screenpos.y - FACE_DRAG_HSIZE, 0
			};



			if(g_sel1b == b && g_dragS == j)
			{
				float colour[] = FACE_DRAG_SFILLRGBA;
				glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour);
			}
			else
			{
				float colour[] = FACE_DRAG_FILLRGBA;
				glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour);
			}

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
			glDrawArrays(GL_QUADS, 0, 4);

			if(g_sel1b == b && g_dragS == j)
			{
				float colour2[] = FACE_DRAG_SOUTLRGBA;
				glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);
			}
			else
			{
				float colour2[] = FACE_DRAG_OUTLRGBA;
				glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);
			}

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
			glDrawArrays(GL_LINE_STRIP, 0, 5);
		}
	}


	for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
	{
		Brush* b = *i;
		Vec3f centroid = Vec3f(0,0,0);

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* s = &b->m_sides[j];

			centroid = centroid + s->m_centroid / (float)b->m_nsides;
		}

		Vec4f screenpos = ScreenPos(mvp, centroid, w, h, persp);

		float verts[] =
		{
			screenpos.x - BRUSH_DRAG_HSIZE, screenpos.y - BRUSH_DRAG_HSIZE, 0,
			screenpos.x + BRUSH_DRAG_HSIZE, screenpos.y - BRUSH_DRAG_HSIZE, 0,
			screenpos.x + BRUSH_DRAG_HSIZE, screenpos.y + BRUSH_DRAG_HSIZE, 0,
			screenpos.x - BRUSH_DRAG_HSIZE, screenpos.y + BRUSH_DRAG_HSIZE, 0,
			screenpos.x - BRUSH_DRAG_HSIZE, screenpos.y - BRUSH_DRAG_HSIZE, 0
		};

		float coluor[] = BRUSH_DRAG_FILLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, coluor);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_QUADS, 0, 4);

		float colour2[] = BRUSH_DRAG_OUTLRGBA;
		glUniform4fv(shader->m_slot[SSLOT_COLOR], 1, colour2);

		glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, verts);
		glDrawArrays(GL_LINE_STRIP, 0, 5);
	}
}

//draw drag handles
void DrawDrag(EdMap* map, Matrix* mvp, int w, int h, bool persp)
{
	Shader* shader = &g_shader[g_curS];

	if(ViewOpen("door edit"))
	{
		if(g_selB.size() <= 0)
			return;

		//return;

		DrawDrag_Door(map, mvp, w, h, persp);
	}
	else
	{
		DrawDrag_VertFaceBrush(map, mvp, w, h, persp);
	}
}

void SelectBrush(EdMap* map, Vec3f line[], Vec3f vmin, Vec3f vmax)
{
	g_dragV = -1;
	g_dragS = -1;
	CloseSideView();
	//CloseView("brush edit");

	//g_log<<"select brush ("<<line[0].x<<","<<line[0].y<<","<<line[0].z<<")->("<<line[1].x<<","<<line[1].y<<","<<line[1].z<<")"<<endl;
	list<Brush*> selB;

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		Vec3f dummy;
		Vec3f trace = b->traceray(line, &dummy);
		if(trace != line[1] && trace.y <= g_maxelev
#if 1
			&&
			trace.x >= vmin.x && trace.x <= vmax.x &&
			trace.y >= vmin.y && trace.y <= vmax.y &&
			trace.z >= vmin.z && trace.z <= vmax.z
#endif
			)
		{
			//g_selB.clear();
			//g_selB.push_back(&*b);
			selB.push_back(&*b);
			OpenAnotherView("brush edit");
			//return;
		}
	}

	//If we already have a selected brush (globally),
	//choose the one after it in our selection array
	//(selB) as the next selected brush.
	if(g_selB.size() == 1)
	{
		bool found = false;
		for(auto i=selB.begin(); i!=selB.end(); i++)
		{
			if(found)
			{
				g_selB.clear();
				g_selB.push_back(*i);
				OpenAnotherView("brush edit");
				return;
			}

			auto j = g_selB.begin();

			if(*i == *j)
				found = true;
		}
	}

	g_selB.clear();

	//If we've reached the end of the selection array
	//(selB), select the one at the front.
	if(selB.size() > 0)
	{
		g_selB.push_back( *selB.begin() );
		OpenAnotherView("brush edit");
	}
}

bool SelectDrag_Door(EdMap* map, Matrix* mvp, int w, int h, int x, int y, Vec3f eye, bool persp, Brush* b, EdDoor* door)
{
	Vec3f startvec = door->point;
	Vec3f axisvec = door->point + door->axis;

	Vec4f startscreenpos = ScreenPos(mvp, startvec, w, h, persp);
	Vec4f axisscreenpos = ScreenPos(mvp, axisvec, w, h, persp);

	if(x >= axisscreenpos.x - DOOR_AXIS_DRAG_HSIZE && x <= axisscreenpos.x + DOOR_AXIS_DRAG_HSIZE && y >= axisscreenpos.y - DOOR_AXIS_DRAG_HSIZE && y <= axisscreenpos.y + DOOR_AXIS_DRAG_HSIZE)
	{
		g_sel1b = b;
		g_dragV = -1;
		g_dragS = -1;
		g_dragB = false;
		g_dragD = DRAG_DOOR_AXIS;
		return true;
	}

	if(x >= startscreenpos.x - DOOR_POINT_DRAG_HSIZE && x <= startscreenpos.x + DOOR_POINT_DRAG_HSIZE && y >= startscreenpos.y - DOOR_POINT_DRAG_HSIZE && y <= startscreenpos.y + DOOR_POINT_DRAG_HSIZE)
	{
		g_sel1b = b;
		g_dragV = -1;
		g_dragS = -1;
		g_dragB = false;
		g_dragD = DRAG_DOOR_POINT;
		return true;
	}

	return false;
}

void SelectDrag_VertFaceBrush(EdMap* map, Matrix* mvp, int w, int h, int x, int y, Vec3f eye, bool persp)
{
	float nearest = -1;

	for(int i=0; i<g_selB.size(); i++)
	{
		Brush* b = g_selB[i];

		for(int j=0; j<b->m_nsharedv; j++)
		{
			Vec3f sharedv = b->m_sharedv[j];
			Vec4f screenpos = ScreenPos(mvp, sharedv, w, h, persp);

			if(x >= screenpos.x - VERT_DRAG_HSIZE && x <= screenpos.x + VERT_DRAG_HSIZE && y >= screenpos.y - VERT_DRAG_HSIZE && y <= screenpos.y + VERT_DRAG_HSIZE)
			{
				float thisD = Magnitude(b->m_sharedv[j] - eye);

				if(thisD < nearest || nearest < 0)
				{
					g_sel1b = b;
					g_dragV = j;
					g_dragS = -1;
					g_dragB = false;
					nearest = thisD;
				}
			}
		}

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* side = &b->m_sides[j];
			//g_log<<"centroid "<<side->m_centroid.x<<","<<side->m_centroid.y<<","<<side->m_centroid.z<<endl;
			//g_log.flush();
			Vec4f screenpos = ScreenPos(mvp, side->m_centroid, w, h, persp);

			if(x >= screenpos.x - FACE_DRAG_HSIZE && x <= screenpos.x + FACE_DRAG_HSIZE && y >= screenpos.y - FACE_DRAG_HSIZE && y <= screenpos.y + FACE_DRAG_HSIZE)
			{
				float thisD = Magnitude(side->m_centroid - eye);

				if(thisD <= nearest || nearest < 0 || g_dragS < 0)
				{
					g_sel1b = b;
					g_dragV = -1;
					g_dragS = j;
					g_dragB = false;
					nearest = thisD;
				}
			}
		}

		Vec3f centroid(0,0,0);

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* side = &b->m_sides[j];
			centroid = centroid + side->m_centroid;
		}

		centroid = centroid / (float)b->m_nsides;
		Vec4f screenpos = ScreenPos(mvp, centroid, w, h, persp);

		if(x >= screenpos.x - BRUSH_DRAG_HSIZE && x <= screenpos.x + BRUSH_DRAG_HSIZE && y >= screenpos.y - BRUSH_DRAG_HSIZE && y <= screenpos.y + BRUSH_DRAG_HSIZE)
		{
			float thisD = Magnitude(centroid - eye);

			g_sel1b = b;
			g_dragV = -1;
			g_dragS = -1;
			g_dragB = true;
			nearest = thisD;
		}
	}
}

bool SelectDrag(EdMap* map, Matrix* mvp, int w, int h, int x, int y, Vec3f eye, bool persp)
{
	g_sel1b = NULL;
	g_dragV = -1;
	g_dragS = -1;
	g_dragB = false;
	g_dragD = -1;

	if(ViewOpen("door edit"))
	{
		if(g_selB.size() <= 0)
			goto nodoor;

		Brush* b = *g_selB.begin();
		EdDoor* door = b->m_door;

		if(door == NULL)
			goto nodoor;

		if(SelectDrag_Door(map, mvp, w, h, x, y, eye, persp, b, door))
			return true;
	}

	nodoor:

	CloseSideView();
	//CloseView("brush side edit");

	SelectDrag_VertFaceBrush(map, mvp, w, h, x, y, eye, persp);

	if(g_sel1b != NULL)
	{
		if(g_dragS >= 0)
		{
			OpenAnotherView("brush edit");
			OpenAnotherView("brush side edit");
			RedoBSideGUI();
		}

		return true;
	}

	return false;
}
