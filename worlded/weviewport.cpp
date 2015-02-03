//
// weviewport.cpp
//
//

#include "weviewport.h"
#include "../common/draw/shader.h"
#include "../common/gui/gui.h"
#include "../common/math/3dmath.h"
#include "../common/window.h"
#include "../common/platform.h"
#include "../common/gui/font.h"
#include "../common/math/camera.h"
#include "../common/math/matrix.h"
#include "../common/draw/shadow.h"
#include "../common/sim/map.h"
#include "wesim.h"
#include "../common/math/vec4f.h"
#include "wegui.h"
#include "../common/math/brush.h"
#include "../common/draw/sortb.h"
#include "undo.h"
#include "../common/math/frustum.h"
#include "../common/keycodes.h"
#include "../common/sim/sim.h"
#include "../common/sim/explocrater.h"
#include "../common/convex/pruneb.h"

ViewportT g_viewportT[VIEWPORT_TYPES];
Viewport g_viewport[4];
//Vec3f g_focus;
static Vec3f accum(0,0,0);
bool g_changed = false;
UndoH g_beforechange;

ViewportT::ViewportT(Vec3f offset, Vec3f up, const char* label)
{
	m_offset = offset;
	m_up = up;
	strcpy(m_label, label);
}

Viewport::Viewport()
{
	m_drag = false;
	m_ldown = false;
	m_rdown = false;
}

Viewport::Viewport(int type)
{
	m_drag = false;
	m_ldown = false;
	m_rdown = false;
	m_mdown = false;
	m_type = type;
}

Vec3f Viewport::up()
{
    Vec3f upvec = g_camera.m_up;
	ViewportT* t = &g_viewportT[m_type];

	if(m_type != VIEWPORT_ANGLE45O)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::up2()
{
    Vec3f upvec = g_camera.up2();
	ViewportT* t = &g_viewportT[m_type];

	if(m_type != VIEWPORT_ANGLE45O)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::strafe()
{
	Vec3f upvec = up();
	ViewportT* t = &g_viewportT[m_type];
	Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));

	if(m_type == VIEWPORT_ANGLE45O)
		sidevec = g_camera.m_strafe;

	return sidevec;
}

Vec3f Viewport::focus()
{
	Vec3f viewvec = g_camera.m_view;
	return viewvec;
}

Vec3f Viewport::viewdir()
{
	Vec3f focusvec = focus();
	Vec3f posvec = pos();
	//Vec3f viewvec = posvec + Normalize(focusvec-posvec);
	//return viewvec;
	return focusvec-posvec;
}

Vec3f Viewport::pos()
{
    Vec3f posvec = g_camera.m_pos;

	if(g_projtype == PROJ_PERSP && m_type == VIEWPORT_ANGLE45O)
	{
		Vec3f dir = Normalize( g_camera.m_view - g_camera.m_pos );
		posvec = g_camera.m_view - dir * 1000.0f / g_zoom;
	}

	ViewportT* t = &g_viewportT[m_type];

	if(m_type != VIEWPORT_ANGLE45O)
		posvec = g_camera.m_view + t->m_offset;

	return posvec;
}

// draw grid of dots and crosses
// for ease of placement/alignment
void DrawGrid(Vec3f vmin, Vec3f vmax)
{
	Shader* s = &g_shader[g_curS];
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 0.5f, 0.5f, 1.0f, 1.0f);

	// draw x axis
	float xaxisverts[] = {vmin.x, 0.0f, 0.0f, vmax.x, 0.0f, 0.0f};
	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, xaxisverts);
	glDrawArrays(GL_LINES, 0, 2);

	// draw y axis
	float yaxisverts[] = {0.0f, vmin.y, 0.0f, 0.0f, vmax.y, 0.0f};
	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, yaxisverts);
	glDrawArrays(GL_LINES, 0, 2);

	// draw z axis
	float zaxisverts[] = {0.0f, 0.0f, vmin.z, 0.0f, 0.0f, vmax.z};
	glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, zaxisverts);
	glDrawArrays(GL_LINES, 0, 2);

	//float interval = (10.0f/g_zoom);
	//float interval = log10(g_zoom * 100.0f) * 10.0f;
	//float interval = (int)(1.0f/g_zoom/10.0f)*10.0f;

	//float base = PROJ_RIGHT/20*2;
	//float invzoom = base/g_zoom;
	//int power = log(invzoom)/log(base);
	//float interval = pow(base, power);

	//STOREY_HEIGHT=250
	//float baseinterval = STOREY_HEIGHT / 5.0f;
	//float screenfit = PROJ_RIGHT*2/g_zoom;
	//int scale =

	//-  float invzoom = 2.0f/g_zoom;
	//-  int tenpower = log(invzoom)/log(2);
	//-  float interval = pow(2, tenpower);
	//+
	//+  float base = PROJ_RIGHT/15*2;
	//+  float invzoom = base/g_zoom;
	//+  int power = log(invzoom)/log(base);
	//+  float interval = pow(base, power);

	float base = 50.0f;
	//float invzoom = base/g_zoom;
	//int power = log(invzoom)/log(base);
	//float interval = pow(base, power);

	float interval;

	// zoom 1 -> interval 50
	// zoom 0.5 -> interval = 100
	// zoom 0.25 -> interval = 200
	// zoom 0.125 -> interval = 400
	// zoom 0.0625 -> interval 800

	//if(power == 0)
	//if(g_zoom > 1.0f)
	{
	//	interval = invzoom;
		//interval = base / pow(2, (int)g_zoom-1);
		int power2 = log(g_zoom) / log(2);
		interval = base / pow(2, power2);
	}

	// zoom 1 -> interval 50 = 50 / 1 = 50 / 2^0
	// zoom 2 -> interval 25 = 50 / 2 = 50 / 2^1
	// zoom 3 -> interval 12.5 = 50 / 4 = 50 / 2^2
	// zoom 4 -> interval 6.25 = 50 / 8 = 50 / 2^3
	// zoom 5 -> interval 3.125 = 50 / 16 = 50 / 2^4
	// wrong


	Vec3f start = Vec3f( (int)(vmin.x/interval)*interval, (int)(vmin.y/interval)*interval, (int)(vmin.z/interval)*interval );
	Vec3f end = Vec3f( (int)(vmax.x/interval)*interval, (int)(vmax.y/interval)*interval, (int)(vmax.z/interval)*interval );

	//dots
	glUniform4f(g_shader[g_curS].m_slot[SSLOT_COLOR], 0.3f, 0.3f, 0.3f, 1.0f);
	for(float x=start.x; x<=end.x; x+=interval)
		for(float y=start.y; y<=end.y; y+=interval)
			for(float z=start.z; z<=end.z; z+=interval)
			{
				float point[] = {x, y, z};
				glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, point);
				glDrawArrays(GL_POINTS, 0, 1);
			}

	// crosses more spaced out
	float interval2 = interval * 5.0f;

	start = Vec3f( (int)(vmin.x/interval2)*interval2, (int)(vmin.y/interval2)*interval2, (int)(vmin.z/interval2)*interval2 );
	end = Vec3f( (int)(vmax.x/interval2)*interval2, (int)(vmax.y/interval2)*interval2, (int)(vmax.z/interval2)*interval2 );

	for(float x=start.x; x<=end.x; x+=interval2)
		for(float y=start.y; y<=end.y; y+=interval2)
			for(float z=start.z; z<=end.z; z+=interval2)
			{
				float xline[] = {x-interval/2.0f, y, z, x+interval/2.0f, y, z};
				glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, xline);
				glDrawArrays(GL_LINES, 0, 2);

				float yline[] = {x, y-interval/2.0f, z, x, y+interval/2.0f, z};
				glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, yline);
				glDrawArrays(GL_LINES, 0, 2);

				float zline[] = {x, y, z-interval/2.0f, x, y, z+interval/2.0f};
				glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, zline);
				glDrawArrays(GL_LINES, 0, 2);
			}
}

void DrawVertexDebug(Matrix *projection, Matrix *viewmat, Matrix *modelmat, float width, float height)
{
	Matrix mvp = *projection;
	mvp.postMultiply(*viewmat);
	mvp.postMultiply(*modelmat);

	//ScreenPos(
	
	//for(int i=0; i<1; i++)
	//	for(int j=0; j<1; j++)
	//		g_model[themodel].draw(0, Vec3f(-5*180 + 180*i,0,-2.5f*90 + j*90), 0);

	Model* pmodel = &g_model[themodel];

	int framen = 0;
	
	VertexArray* pva = &pmodel->m_va[framen];

	for(int vertn = 0; vertn < pva->numverts; vertn++)
	{
		Vec3f v = pva->vertices[vertn];
		v = v + Vec3f(-5*180, 0, -2.5f*90);

		char text[32];
		sprintf(text, "%d", vertn);
		//sprintf(text, "%d", pva->orignum[vertn]);

		Vec4f screenpos = ScreenPos(&mvp, v, width, height, true);
		
		//DrawShadowedText(MAINFONT8, screenpos.x, screenpos.y, text);
		DrawShadowedText(MAINFONT16, screenpos.x, screenpos.y, text);
		//384
	}
}

void DrawViewport(int which, int x, int y, int width, int height)
{
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	float aspect = fabsf((float)width / (float)height);
	Matrix projection;

	bool persp = false;

	if(g_projtype == PROJ_PERSP && v->m_type == VIEWPORT_ANGLE45O)
	{
		projection = BuildPerspProjMat(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE);
		persp = true;
	}
	else if(g_projtype == PROJ_ORTHO || v->m_type != VIEWPORT_ANGLE45O)
	{
		projection = setorthographicmat(-PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT/g_zoom, -PROJ_RIGHT/g_zoom, MIN_DISTANCE, MAX_DISTANCE);
	}

	//Vec3f viewvec = g_focus;	//g_camera.m_view;
	//Vec3f viewvec = g_camera.m_view;
	Vec3f focusvec = v->focus();
    //Vec3f posvec = g_focus + t->m_offset;
    //Vec3f posvec = g_camera.m_pos;
	Vec3f posvec = v->pos();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	posvec = g_camera.m_view + t->m_offset;

	//viewvec = posvec + Normalize(viewvec-posvec);
    //Vec3f posvec2 = g_camera.lookpos() + t->m_offset;
    //Vec3f upvec = t->m_up;
    //Vec3f upvec = g_camera.m_up;
	Vec3f upvec = v->up();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	upvec = t->m_up;

    Matrix viewmat = gluLookAt3(posvec.x, posvec.y, posvec.z, focusvec.x, focusvec.y, focusvec.z, upvec.x, upvec.y, upvec.z);

	Matrix modelview;
	Matrix modelmat;
	float translation[] = {0, 0, 0};
	modelview.setTranslation(translation);
	modelmat.setTranslation(translation);
	modelview.postMultiply(viewmat);

	Matrix mvpmat;
	mvpmat.set(projection.m_matrix);
	mvpmat.postMultiply(viewmat);

	float extentx = PROJ_RIGHT*aspect/g_zoom;
	float extenty = PROJ_RIGHT/g_zoom;
	//Vec3f vmin = g_focus - Vec3f(extentx, extenty, extentx);
	//Vec3f vmax = g_focus + Vec3f(extentx, extenty, extentx);
	Vec3f vmin = g_camera.m_view - Vec3f(extentx, extenty, extentx);
	Vec3f vmax = g_camera.m_view + Vec3f(extentx, extenty, extentx);

	//draw only one layer of grid dots ...
	if(v->m_type == VIEWPORT_FRONT)	// ... on the x and y axises
	{
		vmin.z = 0;
		vmax.z = 0;
	}
	else if(v->m_type == VIEWPORT_LEFT)	// ... on the y and z axises
	{
		vmin.x = 0;
		vmax.x = 0;
	}
	else if(v->m_type == VIEWPORT_TOP)	// ... on the x and z axises
	{
		vmin.y = 0;
		vmax.y = 0;
	}

	if(v->m_type == VIEWPORT_ANGLE45O)
	{
		//RenderToShadowMap(projection, viewmat, modelmat, g_focus);
		//RenderToShadowMap(projection, viewmat, modelmat, g_camera.m_view);
		RenderToShadowMap(projection, viewmat, modelmat, Vec3f(0,0,0));
		RenderShadowedScene(projection, viewmat, modelmat, modelview);
	}

	if(v->m_type == VIEWPORT_FRONT || v->m_type == VIEWPORT_LEFT || v->m_type == VIEWPORT_TOP)
	{
		UseS(SHADER_COLOR3D);
		Shader* s = &g_shader[g_curS];
		glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, GL_FALSE, projection.m_matrix);
		glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, GL_FALSE, viewmat.m_matrix);
		glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, GL_FALSE, modelmat.m_matrix);
		glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
		DrawGrid(vmin, vmax);
	}

	//if(v->m_type == VIEWPORT_FRONT || v->m_type == VIEWPORT_LEFT || v->m_type == VIEWPORT_TOP)
	{
		Shader* s = &g_shader[g_curS];

		//if(persp)
		if(v->m_type == VIEWPORT_ANGLE45O)
		{
			UseS(SHADER_COLOR3DPERSP);
			s = &g_shader[g_curS];
			glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, GL_FALSE, projection.m_matrix);
			glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, GL_FALSE, viewmat.m_matrix);
			glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, GL_FALSE, modelmat.m_matrix);
			glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
		}

		glClear(GL_DEPTH_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);
		DrawFilled(&g_edmap);
		if(v->m_type != VIEWPORT_ANGLE45O)
			DrawOutlines(&g_edmap);
		else
			DrawSelOutlines(&g_edmap);
		glEnable(GL_DEPTH_TEST);

		UseS(SHADER_COLOR2D);
		s = &g_shader[g_curS];
		glUniform1f(s->m_slot[SSLOT_WIDTH], (float)width);
		glUniform1f(s->m_slot[SSLOT_HEIGHT], (float)height);
		glUniform4f(s->m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
		//glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
		DrawDrag(&g_edmap, &mvpmat, width, height, persp);

		//if(g_edtool == EDTOOL_CUT && v->m_ldown && !g_keys[VK_CONTROL])
		if(g_edtool == EDTOOL_CUT && v->m_ldown && !g_keys[KEYCODE_CONTROL])
		{
			UseS(SHADER_COLOR2D);
			glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)width);
			glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)height);
			glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], 1, 0, 0, 1);
			glEnableVertexAttribArray(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION]);
			//glEnableVertexAttribArray(g_shader[SHADER_COLOR2D].m_slot[SSLOT_TEXCOORD0]);
			/*
			Vec3f strafe = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, t->m_up));
			float screenratio = (2.0f*PROJ_RIGHT)/(float)height/g_zoom;
			Vec3f last = t->m_up*(float)v->m_lastmouse.y*screenratio + strafe*(float)-v->m_lastmouse.x*screenratio;
			Vec3f cur = t->m_up*(float)v->m_curmouse.y*screenratio + strafe*(float)-v->m_curmouse.x*screenratio;
			*/
			Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));
			//last and current cursor positions relative to viewport top,left corner
			Vec3f last = OnNear(v->m_lastmouse.x, v->m_lastmouse.y, width, height, posvec, sidevec, upvec);
			Vec3f cur = OnNear(v->m_curmouse.x, v->m_curmouse.y, width, height, posvec, sidevec, upvec);
			//snap the last and current cursor positions to the nearest grid point (grid size is variable)
			last.x = SnapNearest(g_snapgrid, last.x);
			last.y = SnapNearest(g_snapgrid, last.y);
			last.z = SnapNearest(g_snapgrid, last.z);
			cur.x = SnapNearest(g_snapgrid, cur.x);
			cur.y = SnapNearest(g_snapgrid, cur.y);
			cur.z = SnapNearest(g_snapgrid, cur.z);
			//Vec4f last4 = Vec4f(last, 1);
			//Vec4f cur4 = Vec4f(cur, 1);
			//last4.transform(mvpmat);
			//cur4.transform(mvpmat);

			// get xyzw vector (vec4f) for pixel coordinates of cursor pos's
			Vec4f last4 = ScreenPos(&mvpmat, last, width, height, persp);
			Vec4f cur4 = ScreenPos(&mvpmat, cur, width, height, persp);

			float line[] = {last4.x, last4.y, 0, cur4.x, cur4.y, 0};
			glVertexAttribPointer(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, line);
			glDrawArrays(GL_LINES, 0, 2);

			//g_log<<"cut draw "<<v->m_lastmouse.x<<","<<v->m_lastmouse.y<<"->"<<v->m_curmouse.x<<","<<v->m_curmouse.y<<endl;
			//g_log<<"cut draw2 "<<last4.x<<","<<last4.y<<"->"<<cur4.x<<","<<cur4.y<<endl;
			//g_log<<"cut draw3 "<<last.x<<","<<last.y<<","<<last.z<<"->"<<cur.x<<","<<cur.y<<","<<cur.z<<endl;
			//g_log.flush();
		}
	}

	Ortho(width, height, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	DrawShadowedText(MAINFONT8, 0, 0, t->m_label, NULL, -1);

	//if(persp)
	//	DrawVertexDebug(&projection, &modelmat, &viewmat, width, height);

	//glFlush();
}

bool ViewportLDown(int which, int relx, int rely, int width, int height)
{
	//return false;

	accum = Vec3f(0,0,0);
	g_changed = false;
	WriteH(&g_beforechange);	//write undo history
	Viewport* v = &g_viewport[which];
	v->m_ldown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	if(g_edtool != EDTOOL_NONE)
	{
		g_dragS = -1;
		g_dragV = -1;
		g_dragB = false;
		g_dragD = -1;
		return true;
	}

	ViewportT* t = &g_viewportT[v->m_type];

	//g_log<<"vp["<<which<<"] l down"<<endl;
	//g_log.flush();

	float aspect = fabsf((float)width / (float)height);
	Matrix projection;

	bool persp = false;

	if(v->m_type == VIEWPORT_ANGLE45O && g_projtype == PROJ_PERSP)
	{
		projection = BuildPerspProjMat(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE);
		persp = true;
	}
	else
	{
		projection = setorthographicmat(-PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT/g_zoom, -PROJ_RIGHT/g_zoom, MIN_DISTANCE, MAX_DISTANCE);
	}

	//Vec3f viewvec = g_focus; //g_camera.m_view;
	//Vec3f viewvec = g_camera.m_view;
	Vec3f focusvec = v->focus();
    //Vec3f posvec = g_focus + t->m_offset;
	//Vec3f posvec = g_camera.m_pos;
	Vec3f posvec = v->pos();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	{
	//	posvec = g_camera.m_view + t->m_offset;
		//viewvec = posvec + Normalize(g_camera.m_view-posvec);
	}

	//viewvec = posvec + Normalize(viewvec-posvec);
    //Vec3f posvec2 = g_camera.lookpos() + t->m_offset;
    //Vec3f upvec = t->m_up;
    //Vec3f upvec = g_camera.m_up;
	Vec3f upvec = v->up();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	upvec = t->m_up;

    Matrix viewmat = gluLookAt3(posvec.x, posvec.y, posvec.z, focusvec.x, focusvec.y, focusvec.z, upvec.x, upvec.y, upvec.z);
	Matrix mvpmat;
	mvpmat.set(projection.m_matrix);
	mvpmat.postMultiply(viewmat);

	SelectDrag(&g_edmap, &mvpmat, width, height, relx, rely, posvec, persp);

	return true;
}


bool ViewportRDown(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	v->m_rdown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	return true;
}

void CutBrushes(Plane cuttingp)
{
	EdMap* map = &g_edmap;

	Vec3f pop = PointOnPlane(cuttingp);
	BrushSide news0(cuttingp.m_normal, pop);
	BrushSide news1(Vec3f(0,0,0)-cuttingp.m_normal, pop);

	vector<Brush*> newsel;

	for(auto i=g_selB.begin(); i!=g_selB.end(); )
	{
		Brush* b = *i;

		Brush newb0 = *b;
		Brush newb1 = *b;

		newb0.add(news0);
		newb1.add(news1);

		newb0.collapse();
		newb1.collapse();

		newb0.remaptex();
		newb1.remaptex();

		g_edmap.m_brush.push_back(newb0);
		auto j = map->m_brush.rbegin();
		//PruneB(m, &*j);
		if(!PruneB(map, &*j))
		{
			newsel.clear();
			//g_selB.push_back( &*j );
			newsel.push_back( &*j );
		}

		g_edmap.m_brush.push_back(newb1);
		j = map->m_brush.rbegin();
		//PruneB(m, &*j);
		if(!PruneB(map, &*j))
		{
			newsel.clear();
			//g_selB.push_back( &*j );
			newsel.push_back( &*j );
		}

		for(auto k=map->m_brush.begin(); k!=map->m_brush.end(); k++)
		{
			if(&*k == b)
			{
				map->m_brush.erase(k);
				break;
			}
		}

		i = g_selB.erase( i );
	}

	g_selB.clear();
	g_selB = newsel;
	g_sel1b = NULL;

	//g_selB.push_back( &*map->m_brush.rbegin() );
}


void ViewportLUp_Explosion(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//Vec3f viewvec = g_focus;	//g_camera.m_view;
	//Vec3f viewvec = g_camera.m_view - g_camera.m_pos;
	//Vec3f viewdir = v->viewdir();
	//Vec3f posvec = g_focus + t->m_offset;
	//Vec3f posvec = g_camera.m_pos;
	Vec3f posvec = v->pos();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	{
		//posvec = g_camera.m_view + t->m_offset;
		//viewvec = Normalize(Vec3f(0,0,0)-t->m_offset);
	}

	//viewvec = posvec + Normalize(viewvec-posvec);
	//Vec3f posvec2 = g_camera.lookpos() + t->m_offset;
	//Vec3f upvec = t->m_up;
	//Vec3f upvec = g_camera.up2();
	Vec3f up2vec = v->up2();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	upvec = t->m_up;

	//Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));
	//Vec3f sidevec = Normalize(Cross(viewdir, upv2ec));
	Vec3f sidevec = v->strafe();

	float aspect = fabsf((float)width / (float)height);

	float extentx = PROJ_RIGHT*aspect/g_zoom; 
	float extenty = PROJ_RIGHT/g_zoom; 
	
	Vec3f vmin = g_camera.m_view - Vec3f(extentx, extenty, extentx);
	Vec3f vmax = g_camera.m_view + Vec3f(extentx, extenty, extentx);

	Vec3f line[2];
	//g_log<<"==========="<<endl;
	//g_log<<"t->m_offset = "<<t->m_offset.x<<","<<t->m_offset.y<<","<<t->m_offset.z<<endl;
	line[0] = OnNear(relx, rely, width, height, posvec, sidevec, up2vec);
	line[1] = line[0] - t->m_offset*2.0f;

	if(v->m_type == VIEWPORT_ANGLE45O)
	{
		line[1] = line[0] + Normalize(v->viewdir()) * (MAX_DISTANCE / 2.0f);
	}

	if(v->m_type == VIEWPORT_ANGLE45O && g_projtype == PROJ_PERSP)
	{
		line[0] = posvec;
		line[1] = posvec + ScreenPerspRay(relx, rely, width, height, posvec, sidevec, up2vec, v->viewdir(), FIELD_OF_VIEW) * (MAX_DISTANCE / 2.0f);
	}

	ExplodeCrater(&g_edmap, line, vmin, vmax);

	g_edtool = EDTOOL_NONE;
}


void ViewportLUp_CutBrush(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//Vec3f viewvec = g_focus;	//g_camera.m_view;
	//Vec3f viewvec = g_camera.m_view - g_camera.m_pos;
	//Vec3f viewdir = v->viewdir();
	//Vec3f posvec = g_focus + t->m_offset;
	//Vec3f posvec = g_camera.m_pos;
	Vec3f posvec = v->pos();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	{
		//posvec = g_camera.m_view + t->m_offset;
		//viewvec = Normalize(Vec3f(0,0,0)-t->m_offset);
	}

	//viewvec = posvec + Normalize(viewvec-posvec);
	//Vec3f posvec2 = g_camera.lookpos() + t->m_offset;
	//Vec3f upvec = t->m_up;
	//Vec3f upvec = g_camera.up2();
	Vec3f up2vec = v->up2();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	upvec = t->m_up;

	//Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));
	//Vec3f sidevec = Normalize(Cross(viewdir, upv2ec));
	Vec3f sidevec = v->strafe();

	Vec3f last = OnNear(v->m_lastmouse.x, v->m_lastmouse.y, width, height, posvec, sidevec, up2vec);
	Vec3f cur = OnNear(v->m_curmouse.x, v->m_curmouse.y, width, height, posvec, sidevec, up2vec);
	last.x = SnapNearest(g_snapgrid, last.x);
	last.y = SnapNearest(g_snapgrid, last.y);
	last.z = SnapNearest(g_snapgrid, last.z);
	cur.x = SnapNearest(g_snapgrid, cur.x);
	cur.y = SnapNearest(g_snapgrid, cur.y);
	cur.z = SnapNearest(g_snapgrid, cur.z);

	if(last != cur)
	{
		LinkPrevUndo();
		Vec3f crossaxis[2];
		crossaxis[0] = Normalize(cur - last);
		crossaxis[1] = Normalize(t->m_offset);
		Plane cuttingp;
		cuttingp.m_normal = Normalize( Cross(crossaxis[0], crossaxis[1]) );
		cuttingp.m_d = PlaneDistance(cuttingp.m_normal, last);
		CutBrushes(cuttingp);
		//LinkLatestUndo();
	}

	g_edtool = EDTOOL_NONE;
}

void ViewportLUp_SelectBrush(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	float aspect = fabsf((float)width / (float)height);

	float extentx = PROJ_RIGHT*aspect/g_zoom;
	float extenty = PROJ_RIGHT/g_zoom;
	//Vec3f vmin = g_focus - Vec3f(extentx, extenty, extentx);
	//Vec3f vmax = g_focus + Vec3f(extentx, extenty, extentx);
	Vec3f vmin = g_camera.m_view - Vec3f(extentx, extenty, extentx);
	Vec3f vmax = g_camera.m_view + Vec3f(extentx, extenty, extentx);

	//Vec3f viewvec = g_focus;	//g_camera.m_view;
	//Vec3f viewvec = g_camera.m_view - g_camera.m_pos;

	//Vec3f posvec = g_focus + t->m_offset;
	//Vec3f posvec = g_camera.m_pos;
	Vec3f posvec = v->pos();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	{
		//	posvec = g_camera.m_view + t->m_offset;
		//	viewvec = Normalize(Vec3f(0,0,0)-t->m_offset);
	}

	//viewvec = posvec + Normalize(viewvec-posvec);
	//Vec3f posvec2 = g_camera.lookpos() + t->m_offset;
	//Vec3f upvec = t->m_up;
	//Vec3f upvec = g_camera.up2();
	Vec3f up2vec = v->up2();

	//if(v->m_type != VIEWPORT_ANGLE45O)
	//	upvec = t->m_up;

	//Vec3f vCross = Cross(m_view - m_pos, m_up);
	//Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));
	//Vec3f sidevec = Normalize(Cross(viewvec, upvec));
	Vec3f sidevec = v->strafe();

	//g_log<<"viewvec "<<viewvec.x<<","<<viewvec.y<<","<<viewvec.z<<endl;
	//g_log<<"upvec "<<upvec.x<<","<<upvec.y<<","<<upvec.z<<endl;
	//g_log<<"sidevec "<<sidevec.x<<","<<sidevec.y<<","<<sidevec.z<<endl;
	//g_log.flush();

#if 0
	// pass frustum to SelectBrush to cull possible selection?
	if(v->m_type == VIEWPORT_ANGLE45O && g_projtype == PROJ_PERSEP)
	{
		Frustum frust;
		frust.CalculateFrustum(proj, modl);
	}
#endif

	Vec3f line[2];
	//g_log<<"==========="<<endl;
	//g_log<<"t->m_offset = "<<t->m_offset.x<<","<<t->m_offset.y<<","<<t->m_offset.z<<endl;
	line[0] = OnNear(relx, rely, width, height, posvec, sidevec, up2vec);
	line[1] = line[0] - t->m_offset*2.0f;

	if(v->m_type == VIEWPORT_ANGLE45O)
	{
		line[1] = line[0] + Normalize(v->viewdir()) * (MAX_DISTANCE / 2.0f);
	}

	if(v->m_type == VIEWPORT_ANGLE45O && g_projtype == PROJ_PERSP)
	{
		line[0] = posvec;
		line[1] = posvec + ScreenPerspRay(relx, rely, width, height, posvec, sidevec, up2vec, v->viewdir(), FIELD_OF_VIEW) * (MAX_DISTANCE / 2.0f);
	}

	SelectBrush(&g_edmap, line, vmin, vmax);
	//g_log<<"============"<<endl;
}

bool ViewportLUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	if(v->m_ldown)
	{
		//return true;
		v->m_ldown = false;

		if(g_changed)
		{
			LinkPrevUndo(&g_beforechange);
			g_changed = false;
			//LinkLatestUndo();
		}

		//g_log<<"vp["<<which<<"] l up = false"<<endl;
		//g_log.flush();

		//if(!g_keys[VK_CONTROL])
		if(!g_keys[KEYCODE_CONTROL])
		{
			if(g_edtool == EDTOOL_CUT)
			{
				ViewportLUp_CutBrush(which, relx, rely, width, height);
			}
			else if(g_edtool == EDTOOL_EXPLOSION)
			{
				ViewportLUp_Explosion(which, relx, rely, width, height);
			}
			else if(g_sel1b == NULL && g_dragV < 0 && g_dragS < 0)
			{
				ViewportLUp_SelectBrush(which, relx, rely, width, height);
			}
		}
	}

	//g_sel1b = NULL;
	//g_dragV = -1;
	//g_dragS = -1;

	return false;
}

bool ViewportRUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	v->m_rdown = false;

	return false;
}

bool ViewportMousewheel(int which, int delta)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//if(v->m_type == VIEWPORT_ANGLE45O)
	{
		g_zoom *= 1.0f + (float)delta / 10.0f;
		return true;
	}

	return false;
}

void ViewportTranslate(int which, int dx, int dy, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//Vec3f strafe = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, t->m_up));

	float screenratio = (2.0f*PROJ_RIGHT)/(float)height/g_zoom;

	//Vec3f move = t->m_up*(float)dy*screenratio + strafe*(float)-dx*screenratio;

	//Vec3f up = Normalize(Cross(strafe, Vec3f(0,0,0)-t->m_offset));
	Vec3f up2 = v->up2();

	//if(v->m_type == VIEWPORT_ANGLE45O)
	//{
	//	strafe = g_camera.m_strafe;
	//	up = g_camera.up2();
	//}

	Vec3f strafe = v->strafe();

	Vec3f move = up2*(float)dy*screenratio + strafe*(float)-dx*screenratio;
	//g_camera.move(move);
	//g_focus = g_focus + move;
	g_camera.move(move);
}


void ViewportRotate(int which, int dx, int dy)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];
	/*
	Vec3f view = Normalize( Vec3f(0,0,0)-t->m_offset );

	Vec3f strafe = Normalize(Cross(view, t->m_up));

	if(Magnitude(view - t->m_up) <= EPSILON || Magnitude(Vec3f(0,0,0) - view - t->m_up) <= EPSILON)
	{
		strafe = Vec3f(1,0,0);
		t->m_offset = Vec3f(1000.0f/3, 1000.0f/3, 1000.0f/3);
	}*/

	//t->m_offset = RotateAround(t->m_offset, g_focus, dy / 100.0f, strafe.x, strafe.y, strafe.z);
	//t->m_offset = RotateAround(t->m_offset, g_focus, dx / 100.0f, t->m_up.x, t->m_up.y, t->m_up.z);

	g_camera.rotateabout(g_camera.m_view, dy / 100.0f, g_camera.m_strafe.x, g_camera.m_strafe.y, g_camera.m_strafe.z);
	g_camera.rotateabout(g_camera.m_view, dx / 100.0f, g_camera.m_up.x, g_camera.m_up.y, g_camera.m_up.z);

	//g_log<<"rotate "<<dx/10.0f<<","<<dy/10.0f<<endl;
	//g_log.flush();
	//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
	SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
}

//#define DRAGV_DEBUG

void DragV(Brush* b, BrushSide* s, int j, Vec3f& newv, bool& mergedv, bool* invalidv, bool& remove)
{
	remove = false;
	Vec3f movev = b->m_sharedv[ s->m_vindices[j] ];

	if(!mergedv)
	{
		for(int i=0; i<b->m_nsharedv; i++)
		{
			if(i == s->m_vindices[j])
				continue;

			Vec3f thisv = b->m_sharedv[i];
			float mag = Magnitude( newv - thisv );

			if(mag <= MERGEV_D)
			{
				newv = thisv;
				break;
			}
		}

		mergedv = true;
	}

	//if(s->m_outline.m_edv.size() % 2 == 0)
	//if(true)
	//if(!g_keys[VK_SHIFT])
	if(!g_keys[KEYCODE_SHIFT])
	{
		Vec3f farthestv = movev;
		float farthestD = 0;

		for(int i=0; i<s->m_ntris+2; i++)
		{
			Vec3f thisv = b->m_sharedv[ s->m_vindices[i] ];
			float mag = Magnitude( thisv - movev );

			if(mag > farthestD)
			{
				farthestD = mag;
				farthestv = thisv;
			}
		}

		//return Normalize( Cross( m_strafe, m_view - m_pos ) );

		Vec3f newline = Normalize(newv - farthestv);
		Vec3f crossaxis = Normalize(Cross(newline, s->m_plane.m_normal));
		s->m_plane.m_normal = Normalize(Cross(crossaxis, newline));
		s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, newv);

#ifdef DRAGV_DEBUG
		g_log<<"crossaxis = "<<crossaxis.x<<","<<crossaxis.y<<","<<crossaxis.z<<endl;
		//g_log<<"midv = "<<midv.x<<","<<midv.y<<","<<midv.z<<endl;
		//g_log<<"crossaxis2 = "<<crossaxis2.x<<","<<crossaxis2.y<<","<<crossaxis2.z<<endl;
		g_log.flush();
#endif
	}
	else
	{
		float farthestd[] = {0, 0};
		Vec3f farthestv[] = {movev, movev};

#ifdef DRAGV_DEBUG
		g_log<<"--------------move v side"<<(s-b->m_sides)<<"------------------"<<endl;
		g_log.flush();
#endif

		for(int i=0; i<s->m_ntris+2; i++)
		{
			//if(i == j)
			//	continue;

			//if(invalidv[i])
			//	continue;

			Vec3f thisv = b->m_sharedv[ s->m_vindices[i] ];

			if(thisv == movev)
				continue;

			//if(thisv == newv)
			//	continue;

			float mag = Magnitude( thisv - movev );

			if(mag <= EPSILON)
				continue;

#ifdef DRAGV_DEBUG
			g_log<<"thisv="<<thisv.x<<","<<thisv.y<<","<<thisv.z<<endl;
			g_log<<"nearestd[0]="<<nearestd[0]<<endl;
			g_log.flush();
#endif

			if(mag >= farthestd[0] || farthestd[0] <= 0)
			{
#ifdef DRAGV_DEBUG
				g_log<<"closer vert0="<<thisv.x<<","<<thisv.y<<","<<thisv.z<<endl;
				g_log.flush();
#endif

				farthestd[0] = mag;
				farthestv[0] = thisv;
			}
		}

		for(int i=0; i<s->m_ntris+2; i++)
		{
			//if(i == j)
			//	continue;

			//if(invalidv[i])
			//	continue;

			Vec3f thisv = b->m_sharedv[ s->m_vindices[i] ];

			if(thisv == movev)
				continue;

			//if(thisv == newv)
			//	continue;

			if(thisv == farthestv[0])
				continue;

			float mag = Magnitude( thisv - movev );

			if(mag <= EPSILON)
				continue;

#ifdef DRAGV_DEBUG
			g_log<<"thisv="<<thisv.x<<","<<thisv.y<<","<<thisv.z<<endl;
			g_log<<"nearestd[]="<<nearestd[1]<<endl;
			g_log.flush();
#endif

			if(mag >= farthestd[1] || farthestd[1] <= 0)
			{
#ifdef DRAGV_DEBUG
				g_log<<"closer vert1="<<thisv.x<<","<<thisv.y<<","<<thisv.z<<endl;
				g_log.flush();
#endif

				farthestd[1] = mag;
				farthestv[1] = thisv;
			}
		}

		Vec3f tri[3];
		tri[0] = movev;
		//tri[0] = newv;
		tri[1] = farthestv[0];
		tri[2] = farthestv[1];

		if(Close(tri[0], tri[1]) || Close(tri[0], tri[2]) || Close(tri[1], tri[2]))
		{
			remove = true;
			return;	//invalid side should be discarded
		}

		Vec3f norm = Normal(tri);

		//g_log<<"tri = ("<<tri[0].x<<","<<tri[0].y<<","<<tri[0].z<<"),("<<tri[1].x<<","<<tri[1].y<<","<<tri[1].z<<"),("<<tri[2].x<<","<<tri[2].y<<","<<tri[2].z<<")"<<endl;
		//g_log<<"tri norm="<<norm.x<<","<<norm.y<<","<<norm.z<<"    plane norm="<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<endl;

		if(Magnitude( norm - s->m_plane.m_normal ) > Magnitude( Vec3f(0,0,0) - norm - s->m_plane.m_normal ))
		{
			//g_log<<"flip vertex order YES"<<endl;
			//g_log.flush();
			Vec3f tempv = farthestv[0];
			farthestv[0] = farthestv[1];
			farthestv[1] = tempv;
		}
		else
		{
#ifdef DRAGV_DEBUG
			g_log<<"flip vertex order NO"<<endl;
			g_log.flush();
#endif
		}

		//return Normalize( Cross( m_strafe, m_view - m_pos ) );

		Vec3f crossaxis = Normalize(farthestv[0] - farthestv[1]);
		Vec3f midv = (farthestv[0] + farthestv[1])/2.0f;
		Vec3f crossaxis2 = Normalize(newv - midv);

		//Vec3f crossaxis2 = Normalize(newv - s->m_centroid);
		//Vec3f crossaxis = Normalize(Cross( crossaxis2, s->m_plane.m_normal ));

		s->m_plane.m_normal = Normalize(Cross(crossaxis, crossaxis2));
		s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, newv);

#ifdef DRAGV_DEBUG
		g_log<<"crossaxis = "<<crossaxis.x<<","<<crossaxis.y<<","<<crossaxis.z<<endl;
		g_log<<"midv = "<<midv.x<<","<<midv.y<<","<<midv.z<<endl;
		g_log<<"crossaxis2 = "<<crossaxis2.x<<","<<crossaxis2.y<<","<<crossaxis2.z<<endl;
		g_log.flush();
#endif
	}
}

void Drag(int which, int dx, int dy, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//Vec3f strafe = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, t->m_up));
	Vec3f strafe = v->strafe();
	Vec3f up2 = v->up2();

	float screenratio = (2.0f*PROJ_RIGHT)/(float)height/g_zoom;

	Vec3f move = up2*(float)dy*screenratio + strafe*(float)-dx*screenratio;
	Vec3f newmove;
	newmove.x = Snap(g_snapgrid, move.x + accum.x);
	newmove.y = Snap(g_snapgrid, move.y + accum.y);
	newmove.z = Snap(g_snapgrid, move.z + accum.z);
	accum = accum + move - newmove;

	//g_log<<"move = "<<move.x<<","<<move.y<<","<<move.z<<endl;
	//g_log<<"newmove = "<<newmove.x<<","<<newmove.y<<","<<newmove.z<<endl;
	//g_log<<"accum = "<<accum.x<<","<<accum.y<<","<<accum.z<<endl;
	//g_log.flush();

	if(newmove != Vec3f(0,0,0))
		g_changed = true;

	if(g_sel1b)
	{
		Brush* b = g_sel1b;

		if(g_dragB)
		{
			list<float> oldus;
			list<float> oldvs;

			for(int i=0; i<b->m_nsides; i++)
			{
				BrushSide* s = &b->m_sides[i];

				//Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
				float oldu = s->m_centroid.x*s->m_tceq[0].m_normal.x + s->m_centroid.y*s->m_tceq[0].m_normal.y + s->m_centroid.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
				float oldv = s->m_centroid.x*s->m_tceq[1].m_normal.x + s->m_centroid.y*s->m_tceq[1].m_normal.y + s->m_centroid.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
				//Vec3f axis = s->m_plane.m_normal;
				//float radians = DEGTORAD(degrees);
				//s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
				//s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);

				oldus.push_back(oldu);
				oldvs.push_back(oldv);

				Vec3f pop = PointOnPlane(s->m_plane);
				pop = pop - newmove;
				s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, pop);

			}

			b->collapse();

			auto oldu = oldus.begin();
			auto oldv = oldvs.begin();

			for(int i=0; i<b->m_nsides; i++, oldu++, oldv++)
			{
				BrushSide* s = &b->m_sides[i];

				//Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];
				float newu = s->m_centroid.x*s->m_tceq[0].m_normal.x + s->m_centroid.y*s->m_tceq[0].m_normal.y + s->m_centroid.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
				float newv = s->m_centroid.x*s->m_tceq[1].m_normal.x + s->m_centroid.y*s->m_tceq[1].m_normal.y + s->m_centroid.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
				float changeu = newu - *oldu;
				float changev = newv - *oldv;
				s->m_tceq[0].m_d -= changeu;
				s->m_tceq[1].m_d -= changev;
			}

			b->remaptex();
			PruneB(&g_edmap, g_sel1b);

			ViewportT* t = &g_viewportT[VIEWPORT_ANGLE45O];
			//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
			SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
		}
		else if(g_dragV >= 0)
		{
			bool* invalidv = new bool[b->m_nsharedv];
			for(int i=0; i<b->m_nsharedv; i++)
				invalidv[i] = false;

			Vec3f movev = b->m_sharedv[ g_dragV ];
			Vec3f newv = movev - newmove;
			bool mergedv = false;

			for(int i=0; i<b->m_nsides; i++)
			{
				BrushSide* s = &b->m_sides[i];
				for(int j=0; j<s->m_ntris+2; j++)
				{
					if(s->m_vindices[j] == g_dragV)
					{
						bool remove;
						DragV(b, s, j, newv, mergedv, invalidv, remove);

						//s->gentexeq();
						s->remaptex();
						b->prunev(invalidv);

						//if(invalidv[ s->m_vindices[j] ])
						//	g_dragV = -1;

						//if(remove)
						{
						//	b->removeside(i);
						//	i--;
						//	break;
						}
					}
				}
			}
			delete [] invalidv;
			b->collapse();
			b->remaptex();
			PruneB(&g_edmap, g_sel1b);

			ViewportT* t = &g_viewportT[VIEWPORT_ANGLE45O];
			//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
			SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
		}
		else if(g_dragS >= 0)
		{
			BrushSide* s = &b->m_sides[g_dragS];
			Vec3f pop = PointOnPlane(s->m_plane);
			pop = pop - newmove;
			s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, pop);
			b->collapse();
			b->remaptex();
			PruneB(&g_edmap, g_sel1b);

			ViewportT* t = &g_viewportT[VIEWPORT_ANGLE45O];
			//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
			SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
		}
		else if(g_dragD >= 0)
		{
			Brush* b = g_sel1b;
			EdDoor* door = b->m_door;

			if(g_dragD == DRAG_DOOR_POINT)
			{
				door->point = door->point - newmove;
			}
			else if(g_dragD == DRAG_DOOR_AXIS)
			{
				door->axis = door->axis - newmove;
			}
		}
	}
}

bool ViewportMousemove(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];

	if(v->m_ldown)
	{
		//g_log<<"vp["<<which<<"] down mouse move l"<<endl;
		//g_log.flush();

		if(g_sel1b)
		{
			Drag(which, relx - v->m_lastmouse.x, rely - v->m_lastmouse.y, width, height);
		}
		//else if(g_keys[VK_CONTROL])
		else if(g_keys[KEYCODE_CONTROL])
		{
			ViewportTranslate(which, relx - v->m_lastmouse.x, rely - v->m_lastmouse.y, width, height);
		}

		//if(g_keys[VK_CONTROL] || g_edtool != EDTOOL_CUT || !v->m_ldown)
		if(g_keys[KEYCODE_CONTROL] || g_edtool != EDTOOL_CUT || !v->m_ldown)
			v->m_lastmouse = Vec2i(relx, rely);
		v->m_curmouse = Vec2i(relx, rely);
		return true;
	}
	else if(v->m_rdown)
	{
		if(v->m_type == VIEWPORT_ANGLE45O)
		{
			ViewportRotate(which, relx - v->m_lastmouse.x, rely - v->m_lastmouse.y);
		}
		v->m_lastmouse = Vec2i(relx, rely);
		v->m_curmouse = Vec2i(relx, rely);
		return true;
	}

	//if(g_keys[VK_CONTROL] || g_edtool != EDTOOL_CUT || (!v->m_ldown && !v->m_rdown))
	if(g_keys[KEYCODE_CONTROL] || g_edtool != EDTOOL_CUT || (!v->m_ldown && !v->m_rdown))
		v->m_lastmouse = Vec2i(relx, rely);

	v->m_curmouse = Vec2i(relx, rely);

	return false;
}
