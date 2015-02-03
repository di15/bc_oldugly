

#ifndef TEXT_H
#define TEXT_H

#include "widget.h"

class Text : public Widget
{
public:
	Text(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, bool shdw=true, float r=0.8f, float g=0.8f, float b=0.8f, float a=1) : Widget()
	{
		m_parent = parent;
		m_type = WIDGET_TEXT;
		m_name = n;
		m_text = t;
		m_font = f;
		m_pos[0] = left;
		m_pos[1] = top;
		m_ldown = false;
		//rgba[0] = rgba[1] = rgba[2] = 0.8f;
		//rgba[3] = 1.0f;
		m_rgba[0] = r;
		m_rgba[1] = g;
		m_rgba[2] = b;
		m_rgba[3] = a;
		m_shadow = shdw;
		reframe();
	}

	void draw();
};

#endif