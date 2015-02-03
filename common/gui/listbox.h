

#ifndef LISTBOX_H
#define LISTBOX_H

#include "widget.h"

class ListBox : public Widget
{
public:
	ListBox(Widget* parent, const char* n, int f, Margin left, Margin top, Margin right, Margin bottom, void (*change)());

	void draw();
	bool lbuttonup(bool moved);
	bool lbuttondown();
	bool mousemove();
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