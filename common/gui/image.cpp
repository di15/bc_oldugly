

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



Image::Image(Widget* parent, const char* nm, const char* filepath, Margin left, Margin top, Margin right, Margin bottom, int align, float r, float g, float b, float a, float texleft, float textop, float texright, float texbottom) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_IMAGE;
	m_name = nm;
	//CreateTexture(tex, filepath, true);
	CreateTexture(m_tex, filepath, false);
	//CreateTexture(tex, filepath, clamp);
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_texc[0] = texleft;
	m_texc[1] = textop;
	m_texc[2] = texright;
	m_texc[3] = texbottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_alignment = align;
	reframe();
}

Image::Image(Widget* parent, const char* filepath, Margin left, Margin top, Margin right, Margin bottom, int align, float r, float g, float b, float a, float texleft, float textop, float texright, float texbottom) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_IMAGE;
	//CreateTexture(tex, filepath, true);
	CreateTexture(m_tex, filepath, false);
	//CreateTexture(tex, filepath, clamp);
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_texc[0] = texleft;
	m_texc[1] = textop;
	m_texc[2] = texright;
	m_texc[3] = texbottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_alignment = align;
	reframe();
}

Image::Image(Widget* parent, unsigned int t, Margin left, Margin top, Margin right, Margin bottom, int align, float r, float g, float b, float a, float texleft, float textop, float texright, float texbottom) : Widget()
{
	m_parent = parent;
	m_type = WIDGET_IMAGE;
	m_tex = t;
	m_pos[0] = left;
	m_pos[1] = top;
	m_pos[2] = right;
	m_pos[3] = bottom;
	m_texc[0] = texleft;
	m_texc[1] = textop;
	m_texc[2] = texright;
	m_texc[3] = texbottom;
	m_ldown = false;
	m_rgba[0] = r;
	m_rgba[1] = g;
	m_rgba[2] = b;
	m_rgba[3] = a;
	m_alignment = align;
	reframe();
}

void Image::reframe()	//resized or moved
{
	m_pos[0].recalc(m_parent);
	m_pos[1].recalc(m_parent);
	m_pos[2].recalc(m_parent);
	m_pos[3].recalc(m_parent);
	//m_tpos[0].recalc(this);
	//m_tpos[1].recalc(this);
	//m_tpos[2].recalc(this);
	//m_tpos[3].recalc(this);

	align();

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();
}

void Image::draw()
{
	//glColor4fv(rgba);
	glUniform4fv(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, m_rgba);
	DrawImage(g_texture[m_tex].texname, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached, m_pos[3].m_cached, m_texc[0], m_texc[1], m_texc[2], m_texc[3]);
	//glColor4f(1,1,1,1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

