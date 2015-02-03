

#include "undo.h"
#include "../common/draw/sortb.h"
#include "../common/math/camera.h"
#include "../common/utils.h"
#include "../common/save/edmap.h"

list<UndoH> g_undoh;
int g_currundo = -1;	//the index which we will revert to when pressing undo next time
bool g_savedlatest = false;
//bool g_doubleredo = false;
//bool g_doubleundo = false;
//bool g_oncurrh = false;

//#define UNDO_DEBUG

UndoH::~UndoH()
{
#if 0
	g_log<<"~UndoH()"<<endl;
#endif
}

//write undo history
void WriteH(UndoH* towrite)
{
	//Important. For some reason there's a memory leak here
	//if we don't call clear(). Bug in list<>?
	towrite->brushes.clear();
	towrite->brushes = g_edmap.m_brush;

#ifdef UNDO_DEBUG
	g_log<<"save h "<<towrite->brushes.size()<<" brushes"<<endl;
	g_log.flush();
#endif
}

void LinkPrevUndo(UndoH* tosave)
{
	UndoH h;

	if(tosave != NULL)
		h = *tosave;
	else
	{
		WriteH(&h);
	}

#ifdef UNDO_DEBUG
	g_log<<"linkpr gcurrun="<<g_currundo<<endl;
	g_log.flush();
#endif

	g_savedlatest = false;
	//g_doubleredo = false;
	//g_doubleundo = true;

	int j=0;
	auto i=g_undoh.begin();
	while(i!=g_undoh.end())
	{
#ifdef UNDO_DEBUG
		g_log<<"erase? "<<j<<endl;
		g_log.flush();
#endif

		if(j > g_currundo)
		{
#ifdef UNDO_DEBUG
			g_log<<"erase. "<<j<<endl;
			g_log.flush();
#endif
			i = g_undoh.erase(i);
			j++;
			continue;
		}

		i++;
		j++;
	}

	g_currundo++;
	//if(g_currundo >= MAX_UNDO)
	//	g_currundo = MAX_UNDO-1;
	if(g_currundo > MAX_UNDO)
		g_currundo = MAX_UNDO;

#ifdef UNDO_DEBUG
	g_log<<"linkpr gcurrun="<<g_currundo<<endl;
	g_log.flush();
#endif

	g_undoh.push_back(h);

#ifdef UNDO_DEBUG
	g_log<<"linkpr undoh.size="<<g_undoh.size()<<endl;
	g_log.flush();
#endif

	int overl = (int)g_undoh.size() - MAX_UNDO;
	if(overl > 0)
	{
		j=0;
		i=g_undoh.begin();
		while(i!=g_undoh.end() && overl > j)
		{
			i = g_undoh.erase(i);
			j++;
		}
	}

#ifdef UNDO_DEBUG
	g_log<<"linkpr undoh.size2="<<g_undoh.size()<<endl;
	g_log.flush();
#endif
}

void LinkLatestUndo()
{
#ifdef UNDO_DEBUG
	g_log<<"linklate "<<g_currundo<<" == "<<((int)g_undoh.size()-1)<<endl;
	g_log.flush();
#endif

	if(g_currundo >= (int)g_undoh.size()-1 && !g_savedlatest)	//only save if we're at the latest undo
	{
		LinkPrevUndo();
		g_currundo--;
		g_savedlatest = true;
		//g_doubleredo = true;
		//g_doubleundo = false;
	}
}

void Undo()
{
	//g_doubleredo = true;

#ifdef UNDO_DEBUG
	g_log<<"undo? g_curu="<<g_currundo<<endl;
	g_log.flush();
#endif

	if(g_currundo <= -1)
	{
		//g_doubleredo = false;
		return;
	}

#ifdef UNDO_DEBUG
	g_log<<"undo from1 "<<g_currundo<<" of "<<g_undoh.size()<<endl;
	g_log.flush();
#endif

	LinkLatestUndo();
	/*
	if(g_doubleundo)
	{
		//g_doubleundo = false;
		g_currundo --;
		
		if(g_currundo <= -1)
		{
			//g_doubleredo = false;
			return;
		}
	}*/

#ifdef UNDO_DEBUG
	g_log<<"undoh.soze="<<g_undoh.size()<<endl;
	g_log.flush();
#endif

	int j=0;
	for(auto i=g_undoh.begin(); i!=g_undoh.end(); i++, j++)
	{
#ifdef UNDO_DEBUG
		g_log<<"undoh #"<<j<<endl;
		g_log.flush();
#endif

		if(j == g_currundo)
		{
			UndoH* h = &*i;
			g_edmap.m_brush = h->brushes;

#ifdef UNDO_DEBUG
			g_log<<"undid now "<<g_edmap.m_brush.size()<<" brushes"<<endl;
			g_log.flush();
#endif

			break;
		}
	}

#ifdef UNDO_DEBUG
	g_log<<"undo from2 "<<g_currundo<<" of "<<g_undoh.size()<<endl;
	g_log.flush();
#endif

	g_currundo--;
	if(g_currundo < 0)
	{
		g_currundo = 0;
		//return;
	}

	//if(g_currundo <= 0)
	//	g_doubleredo = false;

	g_sel1b = NULL;
	g_selB.clear();
	g_dragB = -1;
	g_dragS = -1;
	g_dragV = -1;
	SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
	
#ifdef UNDO_DEBUG
	g_log<<"undo to "<<g_currundo<<" of "<<g_undoh.size()<<endl;
	g_log.flush();
#endif
	
	//g_oncurrh = true;
	//g_doubleredo = true;
	//g_doubleundo = false;
}

void Redo()
{	
	//g_doubleundo = true;
	g_currundo++;	//moves to current state
	/*
	if(g_doubleredo)
	{
		g_currundo++;	//moves to next state
	}*/
	
	if(g_currundo >= g_undoh.size())
	{
		g_currundo = g_undoh.size()-1;
	//if(g_currundo > g_undoh.size())
	//{
	//	g_currundo = g_undoh.size();
	//if(g_currundo > g_undoh.size()-2)
	//{
	//	g_currundo = g_undoh.size()-2;
		return;
	}

	int j=0;
	for(auto i=g_undoh.begin(); i!=g_undoh.end(); i++, j++)
	{
		if(j == g_currundo)
		{
			UndoH* h = &*i;
			g_edmap.m_brush = h->brushes;

#ifdef UNDO_DEBUG
			g_log<<"redid to "<<g_edmap.m_brush.size()<<" brushes"<<endl;
			g_log.flush();
#endif
			break;
		}
	}
	/*
	if(g_doubleredo)
	{
		//g_doubleredo = false;
		g_currundo--;	//move back to new current state
	}*/

	g_sel1b = NULL;
	g_selB.clear();
	g_dragB = -1;
	g_dragS = -1;
	g_dragV = -1;
	SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);

#ifdef UNDO_DEBUG
	g_log<<"redo to "<<g_currundo<<" of "<<g_undoh.size()<<endl;
	g_log.flush();
#endif

	if(g_currundo > g_undoh.size()-2)
	{
		g_currundo = g_undoh.size()-2;
	}
	
	//g_doubleredo = false;
	//g_doubleundo = true;
}

void ClearUndo()
{
#ifdef UNDO_DEBUG
	g_log<<"clear undo"<<endl;
	g_log.flush();
#endif

	g_currundo = -1;
	g_undoh.clear();
	g_savedlatest = false;
	//g_doubleundo = true;
}