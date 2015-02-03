
#ifndef VIEW_H
#define VIEW_H

#include "widget.h"

class View
{
public:
	string name;
	int page;
	bool opened;
	list<Widget*> widget;

	View(const char* n, int p=0)
	{
		name = n;
		opened = false;
		page = p;
	}

	~View()
	{
		for(auto i=widget.begin(); i!=widget.end(); i++)
			delete *i;

		widget.clear();
	}

	void frameupd()
	{
		for(auto i=widget.begin(); i!=widget.end(); i++)
			(*i)->frameupd();
	}

	bool mousemove()
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->mousemove())
				return true;

		return false;
	}
	
	void premousemove()
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			(*i)->premousemove();
	}
	
	bool keydown(int k)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->keydown(k))
				return true;
		
		return false;
	}
	
	bool keyup(int k)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->keyup(k))
				return true;
		
		return false;
	}

	bool charin(int k)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->charin(k))
				return true;
		
		return false;
	}
	
	bool prelbuttonup(bool moved)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->prelbuttonup(moved))
				return true;	// intercept mouse event

		return false;
	}

	bool lbuttonup(bool moved)
	{
		//for(int i=widget.size()-1; i>=0; i--)
		//	if(widget[i]->m_type == DROPDOWN && widget[i]->m_opened && widget[i]->lbuttonup(moved))
		//		return true;	// intercept mouse event
		
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->lbuttonup(moved))
				return true;	// intercept mouse event

		return false;
	}

	bool prelbuttondown()
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->prelbuttondown())
				return true;	// intercept mouse event

		return false;
	}

	bool lbuttondown()
	{
		//for(int i=0; i<widget.size(); i++)
		//	if(widget[i]->m_type == WIDGET_EDITBOX)
		//		widget[i]->m_opened = false;

		//for(int i=widget.size()-1; i>=0; i--)
		//	if(widget[i]->m_type == DROPDOWN && widget[i].opened && widget[i].lbuttondown())
		//		return true;	// intercept mouse event
		
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->lbuttondown())
				return true;	// intercept mouse event

		return false;
	}

	bool prerbuttonup(bool moved)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->prerbuttonup(moved))
				return true;	// intercept mouse event

		return false;
	}

	bool rbuttonup(bool moved)
	{
		//for(int i=widget.size()-1; i>=0; i--)
		//	if(widget[i]->m_type == DROPDOWN && widget[i]->m_opened && widget[i]->lbuttonup(moved))
		//		return true;	// intercept mouse event
		
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->rbuttonup(moved))
				return true;	// intercept mouse event

		return false;
	}

	
	bool prerbuttondown()
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->prerbuttondown())
				return true;	// intercept mouse event

		return false;
	}

	bool rbuttondown()
	{
	//g_log<<"r down view"<<endl;

		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->rbuttondown())
				return true;	// intercept mouse event

		return false;
	}

	bool mbuttonup(bool moved)
	{
		//for(int i=widget.size()-1; i>=0; i--)
		//	if(widget[i]->m_type == DROPDOWN && widget[i]->m_opened && widget[i]->lbuttonup(moved))
		//		return true;	// intercept mouse event
		
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->mbuttonup(moved))
				return true;	// intercept mouse event

		return false;
	}

	bool mbuttondown()
	{
		//for(int i=0; i<widget.size(); i++)
		//	if(widget[i]->m_type == WIDGET_EDITBOX)
		//		widget[i]->m_opened = false;

		//for(int i=widget.size()-1; i>=0; i--)
		//	if(widget[i]->m_type == DROPDOWN && widget[i].opened && widget[i].lbuttondown())
		//		return true;	// intercept mouse event
		
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->mbuttondown())
				return true;	// intercept mouse event

		return false;
	}
	
	bool mousewheel(int delta)
	{
		for(auto i=widget.rbegin(); i!=widget.rend(); i++)
			if((*i)->mousewheel(delta))
				return true;	// intercept mouse event

		return false;
	}

	Widget* getwidget(const char* name, int type)
	{
		for(auto i=widget.begin(); i!=widget.end(); i++)
			if((*i)->m_type == type && _stricmp((*i)->m_name.c_str(), name) == 0)
				return *i;

		return NULL;
	}

	void resize()
	{
		for(auto i=widget.begin(); i!=widget.end(); i++)
			(*i)->reframe();
	}
	
	void draw();
	void draw2();
};

#endif