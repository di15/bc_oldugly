


#ifndef MAP_H
#define MAP_H

#include "../platform.h"
#include "../math/vec3f.h"
#include "../math/brush.h"
#include "../math/polygon.h"
#include "../math/triangle.h"
#include "../math/brush.h"
#include "../phys/tracework.h"

class Map
{
public:
	int m_nbrush;
	Brush* m_brush;
	list<int> m_transpbrush;
	list<int> m_opaquebrush;
	list<int> m_skybrush;
	Vec3f m_max;
	Vec3f m_min;

	Map();
	~Map();
	void destroy();
	void tracebox(TraceWork* tw, Vec3f start, Vec3f end, Vec3f vmin, Vec3f vmax, float maxstep);
	void trace(TraceWork* tw, TraceJob* tj);
	void trytostep(TraceWork* tw, TraceJob* tj);
	void checkbrush(TraceWork* tw, TraceJob* tj, int brushindex);
};

extern Map g_map;

void DrawMap(Map* map);
void DrawMap2(Map* map);

#endif