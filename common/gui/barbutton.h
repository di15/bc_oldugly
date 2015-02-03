

#ifndef BARBUTTON_H
#define BARBUTTON_H

#include "widget.h"

class BarButton : public Widget
{
public:
	float m_healthbar;

	BarButton(Widget* parent, unsigned int sprite, float bar, Margin left, Margin top, Margin right, Margin bottom, void (*click)(), void (*overf)(), void (*out)());

	void draw();
	bool lbuttonup(bool moved)
	{
		mousemove();

		if(m_over && m_ldown)
		{
			if(clickfunc != NULL)
				clickfunc();

			m_over = false;
			m_ldown = false;

			return true;	// intercept mouse event
		}

		m_over = false;
		m_ldown = false;

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
	bool mousemove();
};

#endif