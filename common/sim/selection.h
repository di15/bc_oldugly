

#include "../platform.h"
#include "../math/vec3f.h"

class Matrix;

extern unsigned int g_circle;
extern list<int> g_selection;

void DrawSelectionCircles(Matrix* projection, Matrix* modelmat, Matrix* viewmat);
void DoSelection(Vec3f campos, Vec3f camside, Vec3f camup2, Vec3f viewdir);