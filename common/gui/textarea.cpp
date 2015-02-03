

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
#include "../keycodes.h"

TextArea::TextArea(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, float r, float g, float b, float a, void (*change)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_TEXTAREA;
	m_name = n;
	m_value = t;
	m_font = f;
	m_pos[0] = left;
	m_pos[1] = top;
    m_pos[2] = right;
    m_pos[3] = bottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	changefunc = change;
	m_scroll[1] = 0;
	m_caret = strlen(t);
	CreateTexture(m_frametex, "gui\\frame.jpg", true);
	CreateTexture(m_filledtex, "gui\\filled.jpg", true);
	CreateTexture(m_uptex, "gui\\up.jpg", true);
	CreateTexture(m_downtex, "gui\\down.jpg", true);
	reframe();
	m_lines = CountLines(t, f, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached-m_pos[0].m_cached-square(), m_pos[3].m_cached-m_pos[1].m_cached);
}

void TextArea::draw()
{
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(g_texture[m_frametex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	DrawImage(g_texture[m_frametex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);
	DrawImage(g_texture[m_uptex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached, m_pos[2].m_cached, m_pos[1].m_cached+square());
	DrawImage(g_texture[m_downtex].texname, m_pos[2].m_cached-square(), m_pos[3].m_cached-square(), m_pos[2].m_cached, m_pos[3].m_cached);
	DrawImage(g_texture[m_filledtex].texname, m_pos[2].m_cached-square(), m_pos[1].m_cached+square()+scrollspace()*topratio(), m_pos[2].m_cached, m_pos[1].m_cached+square()+scrollspace()*bottomratio());

    float width = m_pos[2].m_cached - m_pos[0].m_cached - square();
    float height = m_pos[3].m_cached - m_pos[1].m_cached;

    DrawBoxShadText(m_font, m_pos[0].m_cached, m_pos[1].m_cached, width, height, m_value.c_str(), m_rgba, m_scroll[1], m_opened ? m_caret : -1);
}

int TextArea::rowsshown()
{
	int rows = (m_pos[3].m_cached-m_pos[1].m_cached)/g_font[m_font].gheight;

	return rows;
}

int TextArea::square()
{
	return g_font[m_font].gheight;
}

float TextArea::scrollspace()
{
	return (m_pos[3].m_cached-m_pos[1].m_cached-square()*2);
}

bool TextArea::mousemove()
{
	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y >= m_pos[1].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		m_over = true;

		return true;
	}
	else
	{
		m_over = false;

		return false;
	}
}

bool TextArea::lbuttondown()
{
	if(m_over)
	{
		m_ldown = true;
		return true;
	}

	return false;
}

bool TextArea::lbuttonup(bool moved)
{
	if(m_over && m_ldown)
	{
		m_opened = true;
		m_ldown = false;
		return true;
	}

	m_ldown = false;

	if(m_opened)
	{
		m_opened = false;
		return true;
	}

	return false;
}

bool TextArea::keydown(int k)
{
	if(!m_opened)
		return false;

	int len = m_value.length();

	if(m_caret > len)
		m_caret = len;

#if FINISHED_KEYCODES
	if(k == VK_LEFT)
	{
		if(m_caret <= 0)
			return true;

		m_caret --;
	}
	else if(k == VK_RIGHT)
	{
		int len = m_value.length();

		if(m_caret >= len)
			return true;

		m_caret ++;
	}
	else if(k == 190 && !g_keys[VK_SHIFT])
		placechar('.');/*
	else if(k == VK_TAB)
	{
		for(int i=0; i<g_GUI.view.size(); i++)
		{
			View* v = &g_GUI.view[i];
			for(int j=0; j<v->widget.size(); j++)
			{
				Widget* w = &v->widget[j];
				if(w == this)
				{
					opened = false;

					for(int l=j+1; l<v->widget.size();l++)
					{
						w = &v->widget[l];
						if(w->type == WIDGET_EDITBOX)
						{
							w->opened = true;
							return true;
						}
					}
					for(int l=0; l<j; l++)
					{
						w = &v->widget[l];
						if(w->type == WIDGET_EDITBOX)
						{
							w->opened = true;
							return true;
						}
					}

					opened = true;
					return true;
				}
			}
		}
	}*/
#endif

	if(changefunc != NULL)
		changefunc();

	if(changefunc2 != NULL)
		changefunc2(m_param);

	return true;
}

void TextArea::changevalue(const char* newv)
{
	m_value = newv;
	if(m_caret > strlen(newv))
		m_caret = strlen(newv);
	m_lines = CountLines(m_value.c_str(), MAINFONT8, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached-m_pos[0].m_cached-square(), m_pos[3].m_cached-m_pos[1].m_cached);
}

bool TextArea::keyup(int k)
{
	if(!m_opened)
		return false;

	return true;
}

bool TextArea::charin(int k)
{
	if(!m_opened)
		return false;

	int len = m_value.length();

	if(m_caret > len)
		m_caret = len;

#if FINISHED_KEYCODES
	if(k == VK_BACK)
	{
		int len = m_value.length();

		if(m_caret <= 0 || len <= 0)
			return true;

		string before = m_value.substr(0, m_caret-1);
		string after = m_value.substr(m_caret, len-m_caret);
		m_value = before;
		m_value.append(after);

		m_caret--;
	}
	else if(k == VK_DELETE)
	{
		int len = m_value.length();

		if(m_caret >= len || len <= 0)
			return true;

		string before = m_value.substr(0, m_caret);
		string after = m_value.substr(m_caret+1, len-m_caret);
		m_value = before;
		m_value.append(after);
	}
	else if(k == VK_SHIFT)
		return true;
	else if(k == VK_CAPITAL)
		return true;
	else if(k == VK_SPACE)
	{
		placechar(' ');
	}
	else if(k == VK_RETURN)
	{
		placechar('\n');
	}
	//else if(k >= 'A' && k <= 'Z' || k >= 'a' && k <= 'z')
	//else if(k == 190 || k == 188)
	//else if((k >= '!' && k <= '@') || (k >= '[' && k <= '`') || (k >= '{' && k <= '~') || (k >= '0' || k <= '9'))
	else
	{
		placechar(k);
	}
#endif

	if(changefunc != NULL)
		changefunc();

	if(changefunc2 != NULL)
		changefunc2(m_param);

	return true;
}

