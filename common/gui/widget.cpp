

#include "widget.h"
#include "gui.h"
#include "font.h"
#include "../window.h"

void Margin::recalc(Widget* parent)
{
	int source1 = 0;
	int source2 = 0;
	int sourcepos[4];

	if(parent)
	{
		sourcepos[0] = parent->scrollframe(0);	
		sourcepos[1] = parent->scrollframe(1);	
		sourcepos[2] = parent->scrollframe(2);	
		sourcepos[3] = parent->scrollframe(3);	
	}
	else
	{
		sourcepos[0] = 0;
		sourcepos[1] = 0;
		sourcepos[2] = g_width;
		sourcepos[3] = g_height;
	}

	if(m_source == MARGIN_SOURCE_WIDTH)
	{
		source1 = 0;
		source2 = 2;
	}
	else if(m_source == MARGIN_SOURCE_HEIGHT)
	{
		source1 = 1;
		source2 = 3;
	}
	else if(m_source == MARGIN_SOURCE_MIN || m_source == MARGIN_SOURCE_MAX)
	{
		float wrange = 0;
		float hrange = 0;

		wrange = sourcepos[2] - sourcepos[0];
		hrange = sourcepos[3] - sourcepos[1];

		if(m_source == MARGIN_SOURCE_MIN)
		{
			if(wrange < hrange)
			{
				source1 = 0;
				source2 = 2;
			}
			else
			{
				source1 = 1;
				source2 = 3;
			}
		}
		else if(m_source == MARGIN_SOURCE_MAX)
		{
			if(wrange > hrange)
			{
				source1 = 0;
				source2 = 2;
			}
			else
			{
				source1 = 1;
				source2 = 3;
			}
		}
	}
		
	int minimum = sourcepos[source1];
	int range = sourcepos[source2] - sourcepos[source1];

	if(m_func == MARGIN_FUNC_RATIO)
	{
		m_cached = minimum + (float)range*m_value;
	}
	else if(m_func == MARGIN_FUNC_PIXELS)
	{
		m_cached = minimum + m_value;
	}
	else if(m_func == MARGIN_FUNC_SUBTPIXELS)
	{
		m_cached = minimum + range - m_value;
	}
}

void Widget::select(int which)
{
	m_selected = which;

	if(m_selected >= m_options.size())
		m_selected = m_options.size()-1;

	if(m_selected < 0)
		m_selected = 0;

	if(changefunc2 != NULL)
		changefunc2(m_selected);
}

void Widget::clear()
{
	m_selected = -1;
	m_scroll[1] = 0;
	m_options.clear();
	m_text = "";
}

void Widget::changetext(const char* newt)
{
	m_text = newt;

	if(m_type == WIDGET_BUTTON)
	{
		float length = 0;
		for(int i=0; i<strlen(newt); i++)
			length += g_font[m_font].glyph[newt[i]].origsize[0];
		m_tpos[0].m_cached = (m_pos[0].m_cached+m_pos[2].m_cached)/2.0f - length/2.0f;
		m_tpos[1].m_cached = (m_pos[1].m_cached+m_pos[3].m_cached)/2.0f - g_font[m_font].gheight/2.0f;
	}
}

void Widget::changevalue(const char* newv)
{
	m_value = newv;
	if(m_caret > strlen(newv))
		m_caret = strlen(newv);
}

void Widget::placechar(int k)
{
	int len = m_value.length();

	//if(m_type == WIDGET_EDITBOX && len >= m_maxlen)
	//	return;

	char addchar = k;

	string before = m_value.substr(0, m_caret);
	string after = m_value.substr(m_caret, len-m_caret);
	m_value = before + addchar + after;
	m_caret ++;
}

void Widget::align()
{
	int parentpos[4];

	if(m_parent == NULL)
	{
		parentpos[0] = 0;
		parentpos[1] = 0;
		parentpos[2] = g_width;
		parentpos[3] = g_height;
	}
	else
	{
		parentpos[0] = m_parent->scrollframe(0);
		parentpos[1] = m_parent->scrollframe(1);
		parentpos[2] = m_parent->scrollframe(2);
		parentpos[3] = m_parent->scrollframe(3);
	}

	int parentdimensions[] = {parentpos[2]-parentpos[0], parentpos[3]-parentpos[1]};
	int dimensions[] = {m_pos[2].m_cached-m_pos[0].m_cached, m_pos[3].m_cached-m_pos[1].m_cached};

	if(m_alignment == ALIGNMENT_LESSERSIDE)
	{
	}
	else if(m_alignment == ALIGNMENT_CENTER)
	{
		m_pos[0].m_cached -= m_pos[0].m_cached - parentpos[0];
		m_pos[0].m_cached += parentdimensions[0]/2 - dimensions[0]/2;
		m_pos[2].m_cached -= m_pos[2].m_cached - parentpos[0];
		m_pos[2].m_cached += parentdimensions[0]/2 + dimensions[0]/2;
	}
	else if(m_alignment == ALIGNMENT_GREATERSIDE)
	{
		m_pos[0].m_cached -= m_pos[0].m_cached - parentpos[0];
		m_pos[0].m_cached += parentdimensions[0] - dimensions[0];
		m_pos[2].m_cached -= m_pos[2].m_cached - parentpos[0];
		m_pos[2].m_cached += parentdimensions[0];
	}
}

void Widget::reframe()	//resized or moved
{
	m_pos[0].recalc(m_parent);
	m_pos[1].recalc(m_parent);
	m_pos[2].recalc(m_parent);
	m_pos[3].recalc(m_parent);
	//m_tpos[0].recalc(this);
	//m_tpos[1].recalc(this);
	//m_tpos[2].recalc(this);
	//m_tpos[3].recalc(this);

	//align();

	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->reframe();
}