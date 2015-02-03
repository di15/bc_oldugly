

#ifndef WEVIEWPORT_H
#define WEVIEWPORT_H

#include "../common/math/3dmath.h"
#include "../common/math/camera.h"
#include "../common/math/vec2i.h"

#define VIEWPORT_MAIN3D		0
#define VIEWPORT_TYPES		1

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
	bool m_mdown;
	bool m_rdown;
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
bool ViewportMDown(int which, int relx, int rely, int width, int height);
bool ViewportMUp(int which, int relx, int rely, int width, int height);
bool ViewportMousewheel(int which, int delta);

#endif