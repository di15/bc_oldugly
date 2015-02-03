

#ifndef TOUCHLISTENER_H
#define TOUCHLISTENER_H

#include "widget.h"

class TouchListener : public Widget
{
public:
	TouchListener(Widget* parent, Margin left, Margin top, Margin right, Margin bottom, void (*click2)(int p), void (*overf)(int p), void (*out)(), int parm);

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
	bool mousemove();
};

#endif