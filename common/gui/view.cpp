

#include "gui.h"
#include "../texture.h"

void View::draw()
{
	//if(_stricmp(name.c_str(), "brush edit") == 0)
	//	g_log<<"draw brush edit"<<endl;

	for(auto i=widget.begin(); i!=widget.end(); i++)
	{
		(*i)->draw();
	}
}

void View::draw2()
{
	for(auto i=widget.rbegin(); i!=widget.rend(); i++)
		(*i)->draw2();
}