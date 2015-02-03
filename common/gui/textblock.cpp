


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



void TextBlock::draw()
{
    float width = m_pos[2].m_cached - m_pos[0].m_cached;
    float height = m_pos[3].m_cached - m_pos[1].m_cached;
    
    DrawBoxShadText(m_font, m_pos[0].m_cached, m_pos[1].m_cached, width, height, m_text.c_str(), m_rgba, 0, -1);
	
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void TextBlock::changevalue(const char* newv)
{
	m_value = newv;
	if(m_caret > strlen(newv))
		m_caret = strlen(newv);
	m_lines = CountLines(m_value.c_str(), MAINFONT8, m_pos[0].m_cached, m_pos[1].m_cached, m_pos[2].m_cached-m_pos[0].m_cached-square(), m_pos[3].m_cached-m_pos[1].m_cached);
}

int TextBlock::square()
{
	return g_font[m_font].gheight;
}
