

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



HScroller::HScroller(Widget* parent, Margin top, Margin bottom, void (*click2)(int p), void (*out)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_HSCROLLER;
	m_over = false;
	m_ldown = false;
	m_vel[0] = 0;
	m_vel[1] = 0;
	m_pos[0] = Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, HSCROLLER_SIDESP);
	m_pos[1] = top;
	m_pos[2] = Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_SUBTPIXELS, HSCROLLER_SIDESP);
	m_pos[3] = bottom;
	clickfunc = NULL;
	overfunc = NULL;
	clickfunc2 = click2;
	outfunc = out;
	reframe();
}

void HScroller::draw()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
	{
		(*i)->draw();
	}
}

bool HScroller::checkover()
{
	m_over = false;

	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y >= m_pos[1].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_over = true;
		return true;
	}

	return false;
}

bool HScroller::mousemove()
{
	checkover();

	if(m_ldown)
	{
		m_vel[0] = g_mouse.x - m_mousedown[0];
		//vel[1] = g_mouse.y - mousedown[1];
		m_mousedown[0] = g_mouse.x;
		//mousedown[1] = g_mouse.y;
		movesub();

		return true;
	}
	else
	{
		for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
			if((*i)->mousemove())
				return true;
	}

	return false;
}

void HScroller::movesub()
{
	float changex = 0;
	if(m_vel[0] < 0.0f)
	{
		changex = max(m_vel[0], g_width-HSCROLLER_SIDESP-m_pos[2].m_cached);
		if(changex > m_vel[0])
			m_vel[0] = 0;
	}
	else if(m_vel[0] > 0.0f)
	{
		changex = min(m_vel[0], HSCROLLER_SIDESP-m_pos[0].m_cached);
		if(changex < m_vel[0])
			m_vel[0] = 0;
	}
	else
		return;

	m_vel[0] *= 0.9f;

	m_pos[0].m_cached += changex;
	m_pos[2].m_cached += changex;

	Widget* sw;
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
	{
		sw = *i;
		sw->m_pos[0].m_cached += changex;
		sw->m_pos[2].m_cached += changex;
		sw->m_tpos[0].m_cached += changex;
	}
}

void HScroller::frameupd()
{
	if(m_ldown)
	{
	}
	else
	{
		movesub();
	}
}

bool HScroller::lbuttondown()
{
	if(m_over)
	{
		/*
		for(int i=subwidg.size()-1; i>=0; i--)
		{
			if(subwidg[i].lbuttondown())
				return true;
		}*/
			
		m_ldown = true;
		m_mousedown[0] = g_mouse.x;
		m_mousedown[1] = g_mouse.y;
		
		return true;	// intercept mouse event
	}
	else
	{
		m_ldown = false;
	}
	
	return false;
}

bool HScroller::lbuttonup(bool moved)
{
	if(m_ldown)
	{
		m_ldown = false;
		
		if(!moved)
		{
			for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
			{
				if((*i)->lbuttondown())
					break;
			}
			
			for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
			{
				if((*i)->lbuttonup(moved))
				{
					m_vel[0] = 0;
					m_vel[1] = 0;
					return true;
				}
			}
			
			goto outf;
		}

		return true;
	}

outf:
	if(outfunc != NULL)
	{
		m_vel[0] = 0;
		m_vel[1] = 0;
		outfunc();
		return true;	// intercept mouse event
	}

	return false;
}

