


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



void Text::draw()
{
	//glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
	//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.8f, 0.8f, 0.8f, 1.0f);
	//float color[] = {0.8f, 0.8f, 0.8f, 1.0f};
	//DrawShadowedText(font, pos[0], pos[1], text.c_str(), color);
	if(m_shadow)
		DrawShadowedText(m_font, m_pos[0].m_cached, m_pos[1].m_cached, m_text.c_str(), m_rgba);
	else
	{
		glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], m_rgba[0], m_rgba[1], m_rgba[2], m_rgba[3]);
		DrawLine(m_font, m_pos[0].m_cached, m_pos[1].m_cached, m_text.c_str());
	}
	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

