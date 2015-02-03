//
// weviewport.cpp
// 
// 

#include "gviewport.h"
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
#include "../common/math/vec4f.h"
#include "ggui.h"
#include "../common/math/brush.h"
#include "../common/draw/sortb.h"
#include "../common/math/frustum.h"
#include "../common/sim/selection.h"
#include "../common/sim/order.h"

ViewportT g_viewportT[VIEWPORT_TYPES];
Viewport g_viewport[4];
//Vec3f g_focus;
static Vec3f accum(0,0,0);

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

	if(m_type != VIEWPORT_MAIN3D)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::up2()
{
    Vec3f upvec = g_camera.up2();
	ViewportT* t = &g_viewportT[m_type];

	if(m_type != VIEWPORT_MAIN3D)
		upvec = t->m_up;

	return upvec;
}

Vec3f Viewport::strafe()
{
	Vec3f upvec = up();
	ViewportT* t = &g_viewportT[m_type];
	Vec3f sidevec = Normalize(Cross(Vec3f(0,0,0)-t->m_offset, upvec));

	if(m_type == VIEWPORT_MAIN3D)
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

	if(g_projtype == PROJ_PERSP && m_type == VIEWPORT_MAIN3D)
	{
		Vec3f dir = Normalize( g_camera.m_view - g_camera.m_pos );
		posvec = g_camera.m_view - dir * 1000.0f / g_zoom;
	}

	ViewportT* t = &g_viewportT[m_type];

	if(m_type != VIEWPORT_MAIN3D)
		posvec = g_camera.m_view + t->m_offset;

	return posvec;
}

void DrawViewport(int which, int x, int y, int width, int height)
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_DEPTH_BUFFER_BIT);
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];
	
	float aspect = fabsf((float)width / (float)height);
	Matrix projection;

	bool persp = false;

	if(g_projtype == PROJ_PERSP && v->m_type == VIEWPORT_MAIN3D)
	{
		projection = BuildPerspProjMat(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE);
		persp = true;
	}
	else if(g_projtype == PROJ_ORTHO || v->m_type != VIEWPORT_MAIN3D)
	{
		projection = setorthographicmat(-PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT*aspect/g_zoom, PROJ_RIGHT/g_zoom, -PROJ_RIGHT/g_zoom, MIN_DISTANCE, MAX_DISTANCE);
	}

	Vec3f focusvec = v->focus();
	Vec3f posvec = v->pos();
	Vec3f upvec = v->up();
        
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
	
	//if(v->m_type == VIEWPORT_MAIN3D)
	{
		//RenderToShadowMap(projection, viewmat, modelmat, g_camera.m_view);
		RenderToShadowMap(projection, viewmat, modelmat, Vec3f(0,0,0));
		RenderShadowedScene(projection, viewmat, modelmat, modelview);
	}
	
	Ortho(width, height, 1, 0, 0, 1);
	glDisable(GL_DEPTH_TEST);
	//DrawShadowedText(MAINFONT8, 0, 0, t->m_label, NULL, -1);

	//glFlush();
}

bool ViewportLDown(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];

	v->m_ldown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	g_mousestart = g_mouse;

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

bool ViewportMDown(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];

	v->m_mdown = true;
	v->m_lastmouse = Vec2i(relx, rely);
	v->m_curmouse = Vec2i(relx, rely);

	return true;
}

bool ViewportLUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	v->m_ldown = false;
	v->m_lastmouse = v->m_curmouse;
	v->m_curmouse = Vec2i(relx, rely);

	DoSelection(v->pos(), v->strafe(), v->up2(), v->viewdir());

	return false;
}

bool ViewportRUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];
	
	v->m_rdown = false;

	Order(relx, rely, width, height, v->pos(), v->focus(), v->viewdir(), v->strafe(), v->up2());

	return false;
}

bool ViewportMUp(int which, int relx, int rely, int width, int height)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];
	
	v->m_mdown = false;

	return false;
}

bool ViewportMousewheel(int which, int delta)
{	
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	//if(v->m_type == VIEWPORT_MAIN3D)
	{
		g_zoom *= 1.0f + (float)delta / 10.0f;
		return true;
	}

	return false;
}

void ViewportRotate(int which, int dx, int dy)
{
	Viewport* v = &g_viewport[which];
	ViewportT* t = &g_viewportT[v->m_type];

	g_camera.rotateabout(g_camera.m_view, dy / 100.0f, g_camera.m_strafe.x, g_camera.m_strafe.y, g_camera.m_strafe.z);
	g_camera.rotateabout(g_camera.m_view, dx / 100.0f, g_camera.m_up.x, g_camera.m_up.y, g_camera.m_up.z);

	//SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
}

bool ViewportMousemove(int which, int relx, int rely, int width, int height)
{	
	Viewport* v = &g_viewport[which];
	
	if(v->m_mdown)
	{
		//if(v->m_type == VIEWPORT_ANGLE45O)
		{
			ViewportRotate(which, relx - v->m_lastmouse.x, rely - v->m_lastmouse.y);
		}
		v->m_lastmouse = Vec2i(relx, rely);
		v->m_curmouse = Vec2i(relx, rely);
		return true;
	}
	
	v->m_lastmouse = v->m_curmouse;
	v->m_curmouse = Vec2i(relx, rely);

	return false;
}