


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



void Link::draw()
{
	glDisable(GL_TEXTURE_2D);

	float color[] = {1, 1, 1, 1};

	if(m_over)
	{
		//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		//glColor4f(0.8f, 0.8f, 0.8f, 1.0f);
		//glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.8f, 0.8f, 0.8f, 1.0f);
		color[0] = 0.8f;
		color[1] = 0.8f;
		color[2] = 0.8f;
	}

	DrawShadowedText(m_font, m_pos[0].m_cached, m_pos[1].m_cached, m_text.c_str(), color);

	//glColor4f(1, 1, 1, 1);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);

	//glEnable(GL_TEXTURE_2D);
}

bool Link::mousemove()
{
	if(g_mouse.x >= m_pos[0].m_cached && g_mouse.y >= m_pos[1].m_cached && 
		g_mouse.x <= m_pos[0].m_cached+strlen(m_text.c_str())*g_font[m_font].gheight/2 && 
		g_mouse.y <= m_pos[1].m_cached+g_font[m_font].gheight)
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

