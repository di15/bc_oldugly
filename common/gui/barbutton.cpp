

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

BarButton::BarButton(Widget* parent, unsigned int sprite, float bar, Margin left, Margin top, Margin right, Margin bottom, void (*click)(), void (*overf)(), void (*out)()) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_BARBUTTON;
	m_over = false;
	m_ldown = false;
	m_tex = sprite;
	CreateTexture(m_bgtex, "gui\\buttonbg.png", true);
	CreateTexture(m_bgovertex, "gui\\buttonbgover.png", true);
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_healthbar = bar;
	clickfunc = click;
	overfunc = overf;
	outfunc = out;
	reframe();
}

void BarButton::draw()
{
	if(m_over)
		DrawImage(g_texture[m_bgovertex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);
	else
		DrawImage(g_texture[m_bgtex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	DrawImage(g_texture[m_tex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached);

	UseS(SHADER_COLOR2D);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)g_currw);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)g_currh);
	DrawSquare(1, 0, 0, 1, m_pos[0].m_cached, m_pos[3].m_cached-5, m_pos[2].m_cached, m_pos[3].m_cached);
	float bar = (m_pos[2].m_cached - m_pos[0].m_cached) * m_healthbar;
	DrawSquare(0, 1, 0, 1, m_pos[0].m_cached, m_pos[3].m_cached-5, m_pos[0].m_cached+bar, m_pos[3].m_cached);
	
	Ortho(g_currw, g_currh, 1, 1, 1, 1);
}

bool BarButton::mousemove()
{
	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.x <= m_pos[2].m_cached && g_mouse.y >= m_pos[1].m_cached && g_mouse.y <= m_pos[3].m_cached)
	{
		if(overfunc != NULL)
			overfunc();
			
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

