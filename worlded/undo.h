

#include "../common/math/brush.h"
#include "../common/sim/map.h"
#include "../common/platform.h"

class UndoH	//undo history
{
public:
	list<Brush> brushes;

	UndoH(){}
	~UndoH();
};

#define MAX_UNDO		16

void LinkPrevUndo(UndoH* tosave=NULL);	//call this BEFORE the change is made
void LinkLatestUndo();	//called by undo itself
void WriteH(UndoH* writeto);
void Undo();
void Redo();
void ClearUndo();