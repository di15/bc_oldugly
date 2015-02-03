
#ifndef PRUNEB_H
#define PRUNEB_H

#define MERGEV_D		1.5f

#include "../save/edmap.h"
#include "../math/brush.h"
#include "../math/plane.h"
#include "../phys/physics.h"

bool PruneB(EdMap* map, Brush* b);
bool PruneB2(Brush* b, Plane* p, float epsilon=-CLOSE_EPSILON*2);

#endif