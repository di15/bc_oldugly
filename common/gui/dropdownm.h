

#ifndef DROPDOWNM_H
#define DROPDOWNM_H

#include "widget.h"

// drop down menu
class DropDownM : public Widget
{
public:
	vector<void (*)()> optionfunc;

	DropDownM(Widget* parent, const char* n, int f, Margin left, Margin top, Margin right, Margin bottom, void (*change)());

	void addoption(const char* name, void (*optf)());
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