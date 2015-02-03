

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
#include "gui.h"
#include "../keycodes.h"

EditBox::EditBox(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, bool pw, int maxl, void (*change2)(int p), int parm) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_EDITBOX;
	m_name = n;
	m_font = f;
	m_value = t;
	m_caret = strlen(t);
	m_opened = false;
	m_passw = pw;
	m_maxlen = maxl;
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_scroll[0] = 0;
	m_highl[0] = 0;
	m_highl[1] = 0;
	CreateTexture(m_frametex, "gui\\frame.jpg", true);
	m_param = parm;
	changefunc2 = change2;
	reframe();
}

string EditBox::drawvalue()
{
	string val = m_value;

	if(m_passw)
	{
		val = "";
		for(int i=0; i<m_value.length(); i++)
			val.append("*");
	}

	return val;
}

void EditBox::draw()
{
	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	DrawImage(g_texture[m_frametex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	if(m_over)
		//glColor4f(1, 1, 1, 1);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	else
		//glColor4f(0.8f, 0.8f, 0.8f, 1);
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.8f, 0.8f, 0.8f, 1);

	string val = drawvalue();

	DrawShadowedTextF(m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached, val.c_str(), NULL, m_opened ? m_caret : -1);

	//glColor4f(1, 1, 1, 1);
	//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	HighlightF(m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached, val.c_str(), m_highl[0], m_highl[1]);
}

bool EditBox::mousemove()
{
//#ifdef MOUSESC_DEBUG
	//g_log<<"editbox mousemove"<<endl;
	//g_log.flush();
//#endif

	if(m_ldown)
	{
		string val = drawvalue();
		int newcaret = MatchGlyphF(val.c_str(), m_font, g_mouse.x, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

		if(newcaret > m_caret)
		{
			m_highl[0] = m_caret;
			m_highl[1] = newcaret;
			//g_log<<"hihgl "<<m_highl[0]<<"->"<<m_highl[1]<<endl;
			//g_log.flush();
		}
		else
		{
			m_highl[0] = newcaret;
			m_highl[1] = m_caret;
			//g_log<<"hihgl "<<m_highl[0]<<"->"<<m_highl[1]<<endl;
			//g_log.flush();
		}

		return true;
	}

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

//#define MOUSESC_DEBUG

void EditBox::frameupd()
{
#ifdef MOUSESC_DEBUG
	g_log<<"editbox frameup"<<endl;
	g_log.flush();
#endif

	if(m_ldown)
	{
		bool movedcar = false;

#ifdef MOUSESC_DEBUG
		g_log<<"ldown frameup"<<endl;
		g_log.flush();
#endif

		if(g_mouse.x >= m_pos[2].m_cached-5)
		{
			m_scroll[0] -= max(1, g_font[m_font].gheight/4.0f);

			string val = drawvalue();
			int vallen = val.length();

			int endx = EndX(val.c_str(), vallen, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

			if(endx < m_pos[2].m_cached)
				m_scroll[0] += m_pos[2].m_cached - endx;

			if(m_scroll[0] > 0.0f)
				m_scroll[0] = 0.0f;

			movedcar = true;
		}
		else if(g_mouse.x <= m_pos[0].m_cached+5)
		{
			m_scroll[0] += max(1, g_font[m_font].gheight/4.0f);

			if(m_scroll[0] > 0.0f)
				m_scroll[0] = 0.0f;

			movedcar = true;
		}

		if(movedcar)
		{
			string val = drawvalue();
			int newcaret = MatchGlyphF(val.c_str(), m_font, g_mouse.x, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

			if(newcaret > m_caret)
			{
				m_highl[0] = m_caret;
				m_highl[1] = newcaret;
			}
			else
			{
				m_highl[0] = newcaret;
				m_highl[1] = m_caret;
			}
		}
	}
}

bool EditBox::prelbuttondown()
{
	if(m_opened)
	{
		m_opened = false;
		m_highl[0] = m_highl[1] = 0;
	}

	return false;
}

bool EditBox::lbuttondown()
{
	if(m_over)
	{
		m_ldown = true;

		string val = drawvalue();

		//m_highl[1] = MatchGlyphF(m_value.c_str(), m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);
		//m_highl[0] = m_highl[1];
		//m_caret = m_highl[1];
		m_caret = MatchGlyphF(val.c_str(), m_font, g_mouse.x, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);
		m_highl[0] = 0;
		m_highl[1] = 0;

		return true;
	}

	return false;
}

bool EditBox::lbuttonup(bool moved)
{
	//if(m_over && m_ldown)
	if(m_ldown)
	{
		m_opened = true;
		m_ldown = false;

		if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
		{
			m_caret = -1;
		}

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

bool EditBox::keydown(int k)
{
	if(!m_opened)
		return false;

	int len = m_value.length();

	if(m_caret > len)
		m_caret = len;
/*
	if(k == VK_BACK)
	{
		int len = value.length();

		if(caret <= 0 || len <= 0)
			return true;

		string before = value.substr(0, caret-1);
		string after = value.substr(caret, len-caret);
		value = before;
		value.append(after);

		caret--;
	}
	else if(k == VK_DELETE)
	{
		int len = value.length();

		if(caret >= len || len <= 0)
			return true;

		string before = value.substr(0, caret);
		string after = value.substr(caret+1, len-caret);
		value = before;
		value.append(after);
	}
	else*/
	#if FINISHED_KEYCODES
	if(k == VK_LEFT)
	{
		if(m_highl[0] > 0 && m_highl[0] != m_highl[1])
		{
			m_caret = m_highl[0];
			m_highl[0] = m_highl[1] = 0;
		}
		else if(m_caret <= 0)
			return true;
		else
			m_caret --;

		string val = drawvalue();
		int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

		//g_log<<"left endx = "<<endx<<"/"<<m_pos[0].m_cached<<endl;
		//g_log.flush();

		if(endx <= m_pos[0].m_cached)
			m_scroll[0] += m_pos[0].m_cached - endx + 1;
	}
	else if(k == VK_RIGHT)
	{
		int len = m_value.length();

		if(m_highl[0] > 0 && m_highl[0] != m_highl[1])
		{
			m_caret = m_highl[1];
			m_highl[0] = m_highl[1] = 0;
		}
		else if(m_caret >= len)
			return true;
		else
			m_caret ++;

		string val = drawvalue();
		int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

		if(endx >= m_pos[2].m_cached)
			m_scroll[0] -= endx - m_pos[2].m_cached + 1;
	}
	else if(k == 190 && !g_keys[VK_SHIFT])
		placechar('.');
    #endif
	/*
	else if(k == VK_SHIFT)
		return true;
	else if(k == VK_CAPITAL)
		return true;
	else if(k == VK_SPACE)
	{
		int len = value.length();

		if(len >= maxlen)
			return true;

		char addchar = ' ';
		string before = value.substr(0, caret);
		string after = value.substr(caret, len-caret);
		value = before + addchar + after;
		caret ++;
	}/*
	else if(k >= 'A' && k <= 'Z')
	{
		int len = value.length();

		if(len >= maxlen)
			return true;

		char addchar = k;

		if(!g_keys[VK_SHIFT] && !g_keys[VK_CAPITAL])
			addchar += 32;

		string before = value.substr(0, caret);
		string after = value.substr(caret, len-caret);
		value = before + addchar + after;
		caret ++;
	}*//*
	else if(k == 190 || k == 188)
	{
		int len = value.length();

		if(len >= maxlen)
			return true;

		char addchar = k;

		if(g_keys[VK_SHIFT])
		{
			if(k == 190)
				addchar = '>';
			else if(k == 188)
				addchar = '<';
		}
		else
		{
			if(k == 190)
				addchar = '.';
			else if(k == 188)
				addchar = ',';
		}

		string before = value.substr(0, caret);
		string after = value.substr(caret, len-caret);
		value = before + addchar + after;
		caret ++;
	}
	else if((k >= '!' && k <= '@') || (k >= '[' && k <= '`') || (k >= '{' && k <= '~'))
	{
		int len = value.length();

		if(len >= maxlen)
			return true;

		char addchar = k;

		if(g_keys[VK_SHIFT])
		{
			if(k == '0')
				addchar = ')';
			else if(k == '1')
				addchar = '!';
			else if(k == '2')
				addchar = '@';
			else if(k == '3')
				addchar = '#';
			else if(k == '4')
				addchar = '$';
			else if(k == '5')
				addchar = '%';
			else if(k == '6')
				addchar = '^';
			else if(k == '7')
				addchar = '&';
			else if(k == '8')
				addchar = '*';
			else if(k == '9')
				addchar = '(';
			else if(k == '`')
				addchar = '~';
		}

		string before = value.substr(0, caret);
		string after = value.substr(caret, len-caret);
		value = before + addchar + after;
		caret ++;
	}*/
	/*
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

					m_opened = true;
					return true;
				}
			}
		}
	}*/

	if(changefunc2 != NULL)
		changefunc2(m_param);

	return true;
}

bool EditBox::keyup(int k)
{
	if(!m_opened)
		return false;

	return true;
}

void EditBox::placechar(int k)
{
	int len = m_value.length();

	if(len >= m_maxlen)
		return;

	char addchar = k;

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		string before = m_value.substr(0, m_highl[0]);
		string after = m_value.substr(m_highl[1], len-m_highl[1]);
		m_value = before + after;

		m_caret = m_highl[0];
		m_highl[0] = m_highl[1] = 0;

		before = m_value.substr(0, m_caret);
		after = m_value.substr(m_caret, len-m_caret);
		m_value = before + addchar + after;
		m_caret ++;
	}
	else
	{
		string before = m_value.substr(0, m_caret);
		string after = m_value.substr(m_caret, len-m_caret);
		m_value = before + addchar + after;
		m_caret ++;
	}

	string val = drawvalue();
	int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

	if(endx >= m_pos[2].m_cached)
		m_scroll[0] -= endx - m_pos[2].m_cached + 1;
}

void EditBox::placestr(const char* str)
{
	int len = m_value.length();

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		len -= m_highl[1] - m_highl[0];
	}

	int addlen = strlen(str);
	if(addlen + len >= m_maxlen)
		addlen = m_maxlen - len;

	char* addstr = new char[addlen+1];

	if(addlen > 0)
	{
		for(int i=0; i<addlen; i++)
			addstr[i] = str[i];
		addstr[addlen] = '\0';
	}
	else
	{
		delete [] addstr;
		return;
	}

	if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
	{
		string before = m_value.substr(0, m_highl[0]);
		string after = m_value.substr(m_highl[1]-1, m_value.length()-m_highl[1]);
		m_value = before + addstr + after;

		m_caret = m_highl[0] + addlen;
		m_highl[0] = m_highl[1] = 0;
	}
	else
	{
		if(len >= m_maxlen)
		{
			delete [] addstr;
			return;
		}

		string before = m_value.substr(0, m_caret);
		string after = m_value.substr(m_caret, m_value.length()-m_caret);
		m_value = before + addstr + after;
		m_caret += addlen;
	}

	string val = drawvalue();
	int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

	if(endx >= m_pos[2].m_cached)
		m_scroll[0] -= endx - m_pos[2].m_cached + 1;

	delete [] addstr;
}

void EditBox::changevalue(const char* str)
{
	int len = m_value.length();

	if(len >= m_maxlen)
		return;

	int setlen = strlen(str);
	if(setlen >= m_maxlen)
		setlen = m_maxlen;

	char* setstr = new char[setlen+1];

	for(int i=0; i<setlen; i++)
		setstr[i] = str[i];
	setstr[setlen] = '\0';

	m_value = setstr;
	m_highl[0] = m_highl[1] = 0;
	m_caret = 0;

	delete [] setstr;
}

//#define PASTE_DEBUG

bool EditBox::charin(int k)
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

		if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
		{
			string before = m_value.substr(0, m_highl[0]);
			string after = m_value.substr(m_highl[1], len-m_highl[1]);
			m_value = before + after;

			m_caret = m_highl[0];
			m_highl[0] = m_highl[1] = 0;
		}
		else if(m_caret <= 0 || len <= 0)
			return true;
		else
		{
			string before = m_value.substr(0, m_caret-1);
			string after = m_value.substr(m_caret, len-m_caret);
			m_value = before;
			m_value.append(after);

			m_caret--;
		}

		string val = drawvalue();
		int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

		if(endx <= m_pos[0].m_cached)
			m_scroll[0] += m_pos[0].m_cached - endx + 1;
		else if(endx >= m_pos[2].m_cached)
			m_scroll[0] -= endx - m_pos[2].m_cached + 1;
	}
	else if(k == VK_DELETE)
	{
		int len = m_value.length();

		if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
		{
			string before = m_value.substr(0, m_highl[0]);
			string after = m_value.substr(m_highl[1], len-m_highl[1]);
			m_value = before + after;

			m_caret = m_highl[0];
			m_highl[0] = m_highl[1] = 0;
		}
		else if(m_caret >= len || len <= 0)
			return true;
		else
		{
			string before = m_value.substr(0, m_caret);
			string after = m_value.substr(m_caret+1, len-m_caret);
			m_value = before;
			m_value.append(after);
		}

		string val = drawvalue();
		int endx = EndX(val.c_str(), m_caret, m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

		if(endx <= m_pos[0].m_cached)
			m_scroll[0] += m_pos[0].m_cached - endx + 1;
		else if(endx >= m_pos[2].m_cached)
			m_scroll[0] -= endx - m_pos[2].m_cached + 1;
	}
	else if(k == VK_SHIFT)
		return true;
	else if(k == VK_CAPITAL)
		return true;
	else if(k == VK_RETURN)
		return true;
	else if(k == VK_SPACE)
	{
		placechar(' ');
	}
	else
	{

#ifdef PASTE_DEBUG
		g_log<<"charin "<<(char)k<<" ("<<k<<")"<<endl;
		g_log.flush();
#endif

		//if(k == 'C' && g_keys[VK_CONTROL])
		if(k == 3)	//copy
		{
#ifdef PASTE_DEBUG
			g_log<<"copy vkc"<<endl;
			g_log.flush();
#endif

			if(m_highl[1] > 0 && m_highl[0] != m_highl[1])
			{
				string highl = m_value.substr(m_highl[0], m_highl[1]-m_highl[0]);
				const size_t len = strlen(highl.c_str())+1;
				HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
				memcpy(GlobalLock(hMem), highl.c_str(), len);
				GlobalUnlock(hMem);
				OpenClipboard(0);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hMem);
				CloseClipboard();
			}
			else
			{
				const char* output = "";
				const size_t len = strlen(output)+1;
				HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
				memcpy(GlobalLock(hMem), output, len);
				GlobalUnlock(hMem);
				OpenClipboard(0);
				EmptyClipboard();
				SetClipboardData(CF_TEXT, hMem);
				CloseClipboard();
			}
		}
		//else if(k == 'V' && g_keys[VK_CONTROL])
		else if(k == 22)	//paste
		{
#ifdef PASTE_DEBUG
			g_log<<"paste"<<endl;
#endif
			OpenClipboard(NULL);

#ifdef PASTE_DEBUG
			g_log<<"paste1"<<endl;
#endif
			HANDLE clip0 = GetClipboardData(CF_TEXT);

#ifdef PASTE_DEBUG
			g_log<<"paste2"<<endl;
#endif
			//HANDLE h = GlobalLock(clip0);
			//placestr((char*)clip0);
			char* str = (char*)GlobalLock(clip0);
#ifdef PASTE_DEBUG
			g_log<<"paste3"<<endl;
			g_log<<str<<endl;
#endif
			placestr(str);
#ifdef PASTE_DEBUG
			g_log<<"place str ";
			g_log<<str<<endl;
			g_log.flush();
#endif
			g_log.flush();
			GlobalUnlock(clip0);
			CloseClipboard();
		}
		//else if(k == 'A' && g_keys[VK_CONTROL])
		else if(k == 1)	//select all
		{
			m_highl[0] = 0;
			m_highl[1] = m_value.length()+1;
			m_caret = -1;

			string val = drawvalue();
			int endx = EndX(val.c_str(), m_value.length(), m_font, m_pos[0].m_cached+m_scroll[0], m_pos[1].m_cached);

			if(endx <= m_pos[2].m_cached)
				m_scroll[0] += m_pos[2].m_cached - endx - 1;

			if(m_scroll[0] >= 0)
				m_scroll[0] = 0;
		}
		else
			placechar(k);
	}
#endif

	if(changefunc != NULL)
		changefunc();

	if(changefunc2 != NULL)
		changefunc2(m_param);

	return true;
}

