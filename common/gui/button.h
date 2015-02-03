

#ifndef BUTTON_H
#define BUTTON_H

#include "widget.h"

class Button : public Widget
{
public:
	Button(Widget* parent, const char* n, const char* filepath, const char* label, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, void (*click)(), void (*overf)(), void (*out)());
	Button(Widget* parent, const char* filepath, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, void (*click)(), void (*overf)(), void (*out)());
	Button(Widget* parent, const char* filepath, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, void (*click2)(int p), int parm);
	Button(Widget* parent, const char* filepath, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, void (*click2)(int p), void (*overf)(int p), void (*out)(), int parm);

	void draw();
	void draw2();
	bool lbuttonup(bool moved)
	{
		mousemove();

		if(m_over && m_ldown)
		{
			if(clickfunc != NULL)
				clickfunc();

			if(clickfunc2 != NULL)
				clickfunc2(m_param);

			m_over = false;
			m_ldown = false;

			return true;	// intercept mouse event
		}
		
		if(m_ldown)
		{
			m_ldown = false;
			return true;
		}

		m_over = false;

		return false;
	}
	bool lbuttondown()
	{
		mousemove();

		if(m_over)
		{
			m_ldown = true;
			return true;	// intercept mouse event
		}

		return false;
	}
	void premousemove();
	bool mousemove();
	void reframe();
};

#endif