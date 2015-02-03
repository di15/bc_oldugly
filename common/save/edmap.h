

#ifndef EDMAP_H
#define EDMAP_H

#include "../platform.h"
#include "../math/vec3f.h"
#include "../math/brush.h"
#include "../math/polygon.h"
#include "../math/triangle.h"
#include "../sim/door.h"

class EdMap
{
public:
	list<Brush> m_brush;
};

extern EdMap g_edmap;
extern vector<Brush*> g_selB;
extern Brush* g_sel1b;	//drag selected brush
extern int g_dragV;	//drag vertex of selected brush
extern int g_dragS;	//drag side of selected brush
extern bool g_dragB;
extern int g_dragD;

#define DRAG_DOOR_POINT		1
#define DRAG_DOOR_AXIS		2

void DrawEdMap(EdMap* map, bool showsky);

#endif