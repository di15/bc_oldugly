



#include "widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "dropdownm.h"
#include "hscroller.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"

DropDownM::DropDownM(Widget* parent, const char* n, int f, Margin left, Margin top, Margin right, Margin bottom, void (*change)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_DROPDOWNSELECTOR;
	m_name = n;
	m_font = f;
	m_opened = false;
	m_selected = 0;
	m_scroll[1] = 0;
	m_mousescroll = false;
	m_ldown = false;
	changefunc = change;
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	CreateTexture(m_frametex, "gui\\frame.jpg", true);
	CreateTexture(m_filledtex, "gui\\filled.jpg", true);
	CreateTexture(m_uptex, "gui\\up.jpg", true);
	CreateTexture(m_downtex, "gui\\down.jpg", true);
	reframe();
}

void DropDownM::erase(int which)
{
	m_options.erase( m_options.begin() + which );
	if(m_selected == which)
		m_selected = -1;

	if(m_scroll[1] + rowsshown() > m_options.size())
		m_scroll[1] = m_options.size() - rowsshown();

	if(m_scroll[1] < 0)
		m_scroll[1] = 0;
}

int DropDownM::rowsshown()
{
	int rows = MAX_OPTIONS_SHOWN;
		
	if(rows > m_options.size())
		rows = m_options.size();

	return rows;
}

int DropDownM::square()
{
	return g_font[m_font].gheight;
}

float DropDownM::scrollspace()
{
	return g_font[m_font].gheight*(rowsshown())-square();
}

void DropDownM::draw()
{
	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(g_texture[m_frametex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	if(!m_opened)
		DrawImage(g_texture[m_downtex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[1].m_cached+square());

	if(m_options.size() <= 0)
		return;

	if(m_selected < 0)
	{
		//if(m_options.size() <= 0)
		//	return;

		//DrawShadowedText(m_font, m_pos[0].m_cached+3, m_pos[1].m_cached, m_options[0].c_str());

		return;
	}

	if(m_selected >= m_options.size())
		return;

	DrawShadowedText(m_font, m_pos[0].m_cached+3, m_pos[1].m_cached, m_options[m_selected].c_str());
}

void DropDownM::draw2()
{
	if(!m_opened)
		return;

	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(g_texture[m_frametex].texname, m_pos[0].m_cached, m_pos[1].m_cached+g_font[m_font].gheight, m_pos[2].m_cached, m_pos[3].m_cached+g_font[m_font].gheight*rowsshown());
	DrawImage(g_texture[m_frametex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached+g_font[m_font].gheight*rowsshown());
	DrawImage(g_texture[m_uptex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[1].m_cached+square());
	DrawImage(g_texture[m_downtex].texname, m_pos[2].m_cached-square(), m_pos[3].m_cached+g_font[m_font].gheight*rowsshown()-square(), m_pos[2].m_cached, m_pos[3].m_cached+g_font[m_font].gheight*rowsshown());
	DrawImage(g_texture[m_filledtex].texname, m_pos[2].m_cached-square(), m_pos[3].m_cached+scrollspace()*topratio(), m_pos[2].m_cached, m_pos[3].m_cached+scrollspace()*bottomratio());

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
		DrawShadowedText(m_font, m_pos[0].m_cached+3, m_pos[3].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1]), m_options[i].c_str());
}

bool DropDownM::mousemove()
{
	if(!m_mousescroll)
		return false;

	int dy = g_mouse.y - m_mousedown[1];/*
	int topspace = topratio() * scrollspace();
	int bottomspace = scrollspace() - bottomratio() * scrollspace();

	if(dy < 0 && abs(dy) > topspace)
	{
		scroll = 0;
		return;
	}
	else if(dy > 0 && dy > bottomspace)
	{
		scroll = options.size()-rowsshown();
		if(scroll < 0)
			scroll = 0;

		//char msg[128];
		//sprintf(msg, "bottomspace=%d", bottomspace);
		//MessageBox(NULL, msg, " ", NULL);

		return;
	}
	*/
	float topy = m_pos[3].m_cached+square()+scrollspace()*topratio();
	float newtopy = topy + dy;

	//topratio = (float)scroll / (float)(options.size());
	//topy = pos[3]+square+scrollspace*topratio
	//topy = pos[3]+square+scrollspace*((float)scroll / (float)(options.size()))
	//topy - pos[3] - square = scrollspace*(float)scroll / (float)(options.size())
	//(topy - pos[3] - square)*(float)(options.size())/scrollspace = scroll

	m_scroll[1] = (newtopy - m_pos[3].m_cached - square())*(float)(m_options.size())/scrollspace();

	if(m_scroll[1] < 0)
	{
		m_scroll[1] = 0;
		return true;
	}
	else if(m_scroll[1] + rowsshown() > m_options.size())
	{
		m_scroll[1] = m_options.size() - rowsshown();
		return true;
	}

	m_mousedown[1] = g_mouse.y;
	return true;
}

bool DropDownM::prelbuttondown()
{
	if(!m_opened)
		return false;

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
	{
		// list item?
		if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1])
			&& g_mouse.y <= m_pos[3].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1]+1))
		{
			m_ldown = true;
			return true;	// intercept mouse event
		}
	}

	// scroll bar?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached+scrollspace()*topratio() && g_mouse.x <= m_pos[2].m_cached && 
			g_mouse.y <= m_pos[3].m_cached+scrollspace()*bottomratio())
	{
		m_ldown = true;
		m_mousescroll = true;
		m_mousedown[1] = g_mouse.y;
		return true;	// intercept mouse event
	}

	// up button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_ldown = true;
		return true;
	}

	// down button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached+scrollspace() && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached+scrollspace()+g_font[m_font].gheight)
	{
		m_ldown = true;
		return true;
	}

	return true;
}

bool DropDownM::lbuttondown()
{
	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_ldown = true;
		return true;
	}

	return false;
}

bool DropDownM::prelbuttonup(bool moved)
{
	if(!m_opened)
		return false;

	if(!m_ldown)
	{
		m_opened = false;
		return false;
	}
	
	m_ldown = false;

	if(m_mousescroll)
	{
		m_mousescroll = false;
		return true;	// intercept mouse event
	}

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
	{
		// list item?
		if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1])
			&& g_mouse.y <= m_pos[3].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1]+1))
		{
			m_selected = i;
			m_opened = false;
			if(changefunc != NULL)
				changefunc();
			return true;	// intercept mouse event
		}
	}

	// up button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_scroll[1]--;
		if(m_scroll[1] < 0)
			m_scroll[1] = 0;

		return true;
	}

	// down button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached+scrollspace() && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached+scrollspace()+g_font[m_font].gheight)
	{
		m_scroll[1]++;
		if(m_scroll[1]+rowsshown() > m_options.size())
			m_scroll[1] = m_options.size() - rowsshown();

		return true;
	}
	
	m_opened = false;

	return true;	// intercept mouse event
}

bool DropDownM::lbuttonup(bool moved)
{
	if(!m_ldown)
		return false;

	m_ldown = false;

	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[1].m_cached+square())
	{
		m_opened = true;
		return true;
	}

	return false;
}

