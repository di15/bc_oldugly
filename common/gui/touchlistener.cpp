

#include "widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "dropdowns.h"
#include "hscroller.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"


TouchListener::TouchListener(Widget* parent, Margin left, Margin top, Margin right, Margin bottom, void (*click2)(int p), void (*overf2)(int p), void (*out)(), int parm) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_TOUCHLISTENER;
	m_over = false;
	m_ldown = false;
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = click2;
	overfunc2 = overf2;
	outfunc = out;
	m_param = parm;
	reframe();
}

bool TouchListener::mousemove()
{
	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y >= m_pos[1].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		if(overfunc != NULL)
			overfunc();
		if(overfunc2 != NULL)
			overfunc2(m_param);
			
		m_over = true;

		return true;
	}
	else
	{
		if(m_over && outfunc != NULL)
			outfunc();

		m_over = false;

		return false;
	}

	return false;
}