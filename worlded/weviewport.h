

#ifndef WEVIEWPORT_H
#define WEVIEWPORT_H

#include "../common/math/3dmath.h"
#include "../common/math/camera.h"
#include "../common/math/vec2i.h"

#define VERT_DRAG_HSIZE		3
#define VERT_DRAG_OUTLRGBA	{0.5f, 0.5f, 0.5f, 0.5f}
#define VERT_DRAG_FILLRGBA	{0.7f, 0.7f, 0.7f, 0.5f}
#define FACE_DRAG_HSIZE		4
#define FACE_DRAG_OUTLRGBA	{0.7f, 0.1f, 0.1f, 0.5f}
#define FACE_DRAG_FILLRGBA	{0.9f, 0.3f, 0.3f, 0.5f}
#define FACE_DRAG_SOUTLRGBA	{0.7f, 0.1f, 0.1f, 1.0f}
#define FACE_DRAG_SFILLRGBA	{0.9f, 0.3f, 0.3f, 1.0f}
#define BRUSH_DRAG_HSIZE	5
#define BRUSH_DRAG_OUTLRGBA {0.0f, 0.0f, 0.0f, 0.75f}
#define BRUSH_DRAG_FILLRGBA {0.4f, 0.4f, 0.4f, 0.75f}
#define DOOR_POINT_DRAG_HSIZE		7
#define DOOR_POINT_DRAG_OUTLRGBA	{0.7f, 0.7f, 0.0f, 0.5f}
#define DOOR_POINT_DRAG_FILLRGBA	{0.5f, 0.5f, 0.0f, 0.5f}
#define DOOR_AXIS_DRAG_HSIZE		4
#define DOOR_AXIS_DRAG_OUTLRGBA		{0.9f, 0.9f, 0.0f, 0.5f}
#define DOOR_AXIS_DRAG_FILLRGBA		{0.7f, 0.7f, 0.0f, 0.5f}

#define VIEWPORT_FRONT		0
#define VIEWPORT_LEFT		1
#define VIEWPORT_TOP		2
//#define VIEWPORT_BACK		3
//#define VIEWPORT_RIGHT	4
//#define VIEWPORT_BOTTOM	5
#define VIEWPORT_ANGLE45O	6
#define VIEWPORT_TYPES		7

class ViewportT
{
public:
	Vec3f m_offset;
	Vec3f m_up;
	char m_label[32];

	ViewportT(){}
	ViewportT(Vec3f offset, Vec3f up, const char* label);
};

extern ViewportT g_viewportT[VIEWPORT_TYPES];

class Viewport
{
public:
	int m_type;
	bool m_ldown;
	bool m_rdown;
	bool m_mdown;
	Vec2i m_lastmouse;
	Vec2i m_curmouse;
	bool m_drag;

	Viewport();
	Viewport(int type);
	Vec3f up();
	Vec3f up2();
	Vec3f strafe();
	Vec3f focus();
	Vec3f viewdir();
	Vec3f pos();
};

extern Viewport g_viewport[4];
//extern Vec3f g_focus;

void DrawViewport(int which, int x, int y, int width, int height);
bool ViewportLDown(int which, int relx, int rely, int width, int height);
bool ViewportLUp(int which, int relx, int rely, int width, int height);
bool ViewportMousemove(int which, int relx, int rely, int width, int height);
bool ViewportRDown(int which, int relx, int rely, int width, int height);
bool ViewportRUp(int which, int relx, int rely, int width, int height);
bool ViewportMousewheel(int which, int delta);

#endif