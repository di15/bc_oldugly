

#ifndef DROPDOWNS_H
#define DROPDOWNS_H

#include "widget.h"

//drop down selector
class DropDownS : public Widget
{
public:
	DropDownS(Widget* parent, const char* n, int f, Margin left, Margin top, Margin right, Margin bottom, void (*change)());

	void draw();
	void draw2();
	bool prelbuttonup(bool moved);
	bool lbuttonup(bool moved);
	bool prelbuttondown();
	bool lbuttondown();
	bool mousemove();
	bool mousewheel(int delta)
	{
		if(m_opened)
		{


			return true;	// intercept mouse event
		}

		return false;
	}

	int rowsshown();
	int square();
	void erase(int which);

	float topratio()
	{
		return m_scroll[1] / (float)(m_options.size());
	}

	float bottomratio()
	{
		return (m_scroll[1]+rowsshown()) / (float)(m_options.size());
	}

	float scrollspace();
};

#endif