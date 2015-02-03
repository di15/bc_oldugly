


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



ListBox::ListBox(Widget* parent, const char* n, int f, Margin left, Margin top, Margin right, Margin bottom, void (*change)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_LISTBOX;
	m_name = n;
	m_font = f;
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_opened = false;
	m_selected = -1;
	m_scroll[1] = 0;
	m_mousescroll = false;
	m_ldown = false;
	changefunc = change;
	CreateTexture(m_frametex, "gui\\frame.jpg", true);
	CreateTexture(m_filledtex, "gui\\filled.jpg", true);
	CreateTexture(m_uptex, "gui\\up.jpg", true);
	CreateTexture(m_downtex, "gui\\down.jpg", true);
	reframe();
}

void ListBox::erase(int which)
{
	m_options.erase( m_options.begin() + which );
	if(m_selected == which)
		m_selected = -1;

	if(m_scroll[1] + rowsshown() > m_options.size())
		m_scroll[1] = m_options.size() - rowsshown();

	if(m_scroll[1] < 0)
		m_scroll[1] = 0;
}

int ListBox::rowsshown()
{
	int rows = (m_pos[3].m_cached-m_pos[1].m_cached)/g_font[m_font].gheight;

	if(rows > m_options.size())
		rows = m_options.size();

	return rows;
}

int ListBox::square()
{
	return g_font[m_font].gheight;
}

float ListBox::scrollspace()
{
	return (m_pos[3].m_cached-m_pos[1].m_cached-square()*2);
}

void ListBox::draw()
{
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	BitmapFont* f = &g_font[m_font];
	int rows = rowsshown();

	DrawImage(g_texture[m_frametex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	DrawImage(g_texture[m_frametex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);
	DrawImage(g_texture[m_uptex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[1].m_cached+square());
	DrawImage(g_texture[m_downtex].texname, m_pos[2].m_cached-square(), m_pos[3].m_cached-square(), m_pos[2].m_cached, m_pos[3].m_cached);
	DrawImage(g_texture[m_filledtex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached+square()+scrollspace()*topratio(), m_pos[2].m_cached, m_pos[1].m_cached+square()+scrollspace()*bottomratio());

	if(m_selected >= 0 && m_selected >= (int)m_scroll[1] && m_selected < (int)m_scroll[1]+rowsshown())
	{
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);
		DrawImage(g_texture[m_filledtex].texname, m_pos[0].m_cached, m_pos[1].m_cached+(m_selected-(int)m_scroll[1])*f->gheight, m_pos[2].m_cached-square(), m_pos[1].m_cached+(m_selected-(int)m_scroll[1]+1)*f->gheight);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	}

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
		DrawShadowedText(m_font, m_pos[0].m_cached+3, m_pos[1].m_cached+g_font[m_font].gheight*(i-(int)m_scroll[1]), m_options[i].c_str());
}

bool ListBox::mousemove()
{
	if(!m_mousescroll)
		return false;

	int dy = g_mouse.y - m_mousedown[1];

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

bool ListBox::lbuttondown()
{
	BitmapFont* f = &g_font[m_font];

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
	{
		int row = i-(int)m_scroll[1];
		// list item?
		if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached+f->gheight*row
			&& g_mouse.y <= m_pos[1].m_cached+f->gheight*(row+1))
		{
			m_ldown = true;
			return true;	// intercept mouse event
		}
	}

	// scroll bar?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached+square()+scrollspace()*topratio() && g_mouse.x <= m_pos[2].m_cached &&
			g_mouse.y <= m_pos[1].m_cached+square()+scrollspace()*bottomratio())
	{
		m_ldown = true;
		m_mousescroll = true;
		m_mousedown[1] = g_mouse.y;
		return true;	// intercept mouse event
	}

	// up button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[1].m_cached+square())
	{
		m_ldown = true;
		return true;
	}

	// down button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached-square() && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_ldown = true;
		return true;
	}

	return false;
}

bool ListBox::lbuttonup(bool moved)
{
	if(!m_ldown)
		return false;

	m_ldown = false;

	if(m_mousescroll)
	{
		m_mousescroll = false;
		return true;	// intercept mouse event
	}

	BitmapFont* f = &g_font[m_font];

	for(int i=(int)m_scroll[1]; i<(int)m_scroll[1]+rowsshown(); i++)
	{
		int row = i-(int)m_scroll[1];

		// list item?
		if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached+f->gheight*row
			&& g_mouse.y <= m_pos[1].m_cached+f->gheight*(row+1))
		{
			m_selected = i;
			if(changefunc != NULL)
				changefunc();

			return true;	// intercept mouse event
		}
	}

	// up button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[1].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[1].m_cached+square())
	{
		if(rowsshown() < (int)((m_pos[3].m_cached-m_pos[1].m_cached)/f->gheight))
		{
			return true;
		}

		m_scroll[1]--;
		if(m_scroll[1] < 0)
			m_scroll[1] = 0;

		return true;
	}

	// down button?
	if(g_mouse.x >= m_pos[2].m_cached-square() && g_mouse.y >= m_pos[3].m_cached-square() && g_mouse.x <= m_pos[2].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_scroll[1]++;
		if(m_scroll[1]+rowsshown() > m_options.size())
			m_scroll[1] = m_options.size() - rowsshown();

		return true;
	}

	return true;	// intercept mouse event
}

