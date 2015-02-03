



#ifndef GUI_H
#define GUI_H

#include "../platform.h"
#include "../math/3dmath.h"
#include "view.h"
#include "image.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "dropdowns.h"
#include "editbox.h"
#include "hscroller.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"
#include "frame.h"
#include "viewportw.h"

class GUI
{
public:
	list<View> view;
	void (*keyupfunc[256])();
	void (*keydownfunc[256])();
	void (*anykeyupfunc)(int k);
	void (*anykeydownfunc)(int k);
	void (*mousemovefunc)();
	void (*lbuttondownfunc)();
	void (*lbuttonupfunc)();
	void (*rbuttondownfunc)();
	void (*rbuttonupfunc)();
	void (*mbuttondownfunc)();
	void (*mbuttonupfunc)();
	void (*mousewheelfunc)(int delta);

	GUI()
	{
		for(int i=0; i<256; i++)
		{
			keyupfunc[i] = NULL;
			keydownfunc[i] = NULL;
		}

		mousemovefunc = NULL;
	}

	void frameupd()
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened)
				i->frameupd();
	}

	View* getview(const char* name)
	{
		for(auto i=view.begin(); i!=view.end(); i++)
			if(_stricmp(i->name.c_str(), name) == 0)
				return &*i;

		return NULL;
	}

	void assignMouseWheel(void (*wheel)(int delta))
	{
		mousewheelfunc = wheel;
	}

	void assignLButton(void (*down)(), void (*up)())
	{
		lbuttondownfunc = down;
		lbuttonupfunc = up;
	}

	void assignRButton(void (*down)(), void (*up)())
	{
		rbuttondownfunc = down;
		rbuttonupfunc = up;
	}

	void assignMButton(void (*down)(), void (*up)())
	{
		mbuttondownfunc = down;
		mbuttonupfunc = up;
	}

	void assignMouseMove(void (*mouse)())
	{
		mousemovefunc = mouse;
	}

	void assignKey(int i, void (*down)(), void (*up)())
	{
		keydownfunc[i] = down;
		keyupfunc[i] = up;
	}

	void assignAnyKey(void (*down)(int k), void (*up)(int k))
	{
		anykeydownfunc = down;
		anykeyupfunc = up;
	}

	void mousewheel(int delta)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->mousewheel(delta))
				return;	// intercept mouse event

		if(mousewheelfunc != NULL)
			mousewheelfunc(delta);
	}

	void lbuttondown()
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->prelbuttondown())
				return;	// intercept mouse event

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->lbuttondown())
				return;	// intercept mouse event

		if(lbuttondownfunc != NULL)
			lbuttondownfunc();
	}
	void lbuttonup(bool moved)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->prelbuttonup(moved))
				return;	// intercept mouse event

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->lbuttonup(moved))
				return;	// intercept mouse event

		if(lbuttonupfunc != NULL)
			lbuttonupfunc();
	}

	void rbuttondown()
	{

	//g_log<<"r down gui"<<endl;

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->prerbuttondown())
				return;	// intercept mouse event

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->rbuttondown())
				return;	// intercept mouse event

		if(rbuttondownfunc != NULL)
			rbuttondownfunc();
	}
	void rbuttonup(bool moved)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->prerbuttonup(moved))
				return;	// intercept mouse event

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->rbuttonup(moved))
				return;	// intercept mouse event

		if(rbuttonupfunc != NULL)
			rbuttonupfunc();
	}

	void mbuttondown()
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->mbuttondown())
				return;	// intercept mouse event

		if(mbuttondownfunc != NULL)
			mbuttondownfunc();
	}

	void mbuttonup(bool moved)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened && i->mbuttonup(moved))
				return;	// intercept mouse event

		if(mbuttonupfunc != NULL)
			mbuttonupfunc();
	}

	void mousemove()
	{
		for(auto i=view.begin(); i!=view.end(); i++)
			if(i->opened)
				i->premousemove();

		for(auto i=view.begin(); i!=view.end(); i++)
			if(i->opened)
				if(i->mousemove())
					return;

		if(mousemovefunc != NULL)
			mousemovefunc();
	}

	bool keyup(int k)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened)
				if(i->keyup(k))
					return true;

		if(anykeyupfunc != NULL)
			anykeyupfunc(k);

		if(keyupfunc[k] != NULL)
			keyupfunc[k]();

		return false;
	}

	bool keydown(int k)
	{
		g_keyintercepted = false;

		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened)
				if(i->keydown(k))
				{
					g_keyintercepted = true;
					return true;
				}

		if(anykeydownfunc != NULL)
			anykeydownfunc(k);

		if(keydownfunc[k] != NULL)
			keydownfunc[k]();

		return false;
	}

	bool charin(int k)
	{
		for(auto i=view.rbegin(); i!=view.rend(); i++)
			if(i->opened)
				if(i->charin(k))
					return true;

		return false;
	}

	void resize()
	{
		for(auto i=view.begin(); i!=view.end(); i++)
			i->resize();
	}

	void draw();
};

extern GUI g_GUI;
extern int g_currw;
extern int g_currh;

void AssignKey(int i, void (*down)(), void (*up)());
void AssignAnyKey(void (*down)(int k), void (*up)(int k));
void AssignMouseWheel(void (*wheel)(int delta));
void AssignMouseMove(void (*mouse)());
void AssignLButton(void (*down)(), void (*up)());
void AssignRButton(void (*down)(), void (*up)());
void AssignMButton(void (*down)(), void (*up)());
View* AddView(const char* name, int page=0);
bool ViewOpen(const char* name, int page=0);
void CloseView(const char* name);
void OpenSoleView(const char* name, int page=0);
bool OpenAnotherView(const char* name, int page=0);
void NextPage(const char* name);
bool MousePosition();
void CenterMouse();
void UpdateStatus(const char* status, bool logthis=false);
void Ortho(int width, int height, float r, float g, float b, float a);

#endif
