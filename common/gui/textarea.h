

#ifndef TEXTAREA_H
#define TEXTAREA_H

#include "widget.h"

class TextArea : public Widget
{
public:
	TextArea(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, float r=1, float g=1, float b=1, float a=1, void (*change)()=NULL);

	void draw();
	bool lbuttonup(bool moved);
	bool lbuttondown();
	bool mousemove();
	int rowsshown();
	int square();

	float topratio()
	{
		return m_scroll[1] / (float)m_lines;
	}

	float bottomratio()
	{
		return (m_scroll[1]+rowsshown()) / (float)m_lines;
	}

	float scrollspace();
	bool keydown(int k);
	bool keyup(int k);
	bool charin(int k);
	void changevalue(const char* newv);
};

#endif