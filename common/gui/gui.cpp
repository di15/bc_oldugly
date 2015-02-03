


#include "../draw/shader.h"
#include "gui.h"
#include "../texture.h"
#include "font.h"
#include "../math/3dmath.h"
#include "../platform.h"
#include "../window.h"
#include "draw2d.h"
#include "../draw/shadow.h"

GUI g_GUI;
int g_currw;
int g_currh;

void GUI::draw()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	Ortho(g_width, g_height, 1, 1, 1, 1);

	for(auto i=view.begin(); i!=view.end(); i++)
	{
		if(i->opened)
			//LastNum(view[i].name.c_str());
			i->draw();
	}

	for(auto i=view.begin(); i!=view.end(); i++)
		if(i->opened)
			i->draw2();

	//DrawImage(g_depth, g_width - 300, 0, g_width, 300, 0, 1, 1, 0);

	UseS(SHADER_COLOR2D);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)g_width);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)g_height);
	//DrawMarquee();
}

void AssignKey(int i, void (*down)(), void (*up)())
{
	g_GUI.assignKey(i, down, up);
}

void AssignAnyKey(void (*down)(int k), void (*up)(int k))
{
	g_GUI.assignAnyKey(down, up);
}

void AssignMouseWheel(void (*wheel)(int delta))
{
	g_GUI.assignMouseWheel(wheel);
}

void AssignMouseMove(void (*mouse)())
{
	g_GUI.assignMouseMove(mouse);
}

void AssignLButton(void (*down)(), void (*up)())
{
	g_GUI.assignLButton(down, up);
}

void AssignRButton(void (*down)(), void (*up)())
{
	g_GUI.assignRButton(down, up);
}

void AssignMButton(void (*down)(), void (*up)())
{
	g_GUI.assignMButton(down, up);
}

View* AddView(const char* name, int page)
{
	View view(name, page);
	g_GUI.view.push_back(view);
	return &*g_GUI.view.rbegin();
}

bool ViewOpen(const char* name, int page)
{
	for(auto i=g_GUI.view.begin(); i!=g_GUI.view.end(); i++)
		if(_stricmp(i->name.c_str(), name) == 0 && i->page == page)
			return i->opened;

	return false;
}

void CloseView(const char* name)
{
	for(auto i=g_GUI.view.begin(); i!=g_GUI.view.end(); i++)
		if(_stricmp(i->name.c_str(), name) == 0)
			i->opened = false;
}

void OpenSoleView(const char* name, int page)
{
	for(auto i=g_GUI.view.begin(); i!=g_GUI.view.end(); i++)
	{
		if(_stricmp(i->name.c_str(), name) == 0 && i->page == page)
			i->opened = true;
		else
			i->opened = false;
	}
}

bool OpenAnotherView(const char* name, int page)
{
	for(auto i=g_GUI.view.begin(); i!=g_GUI.view.end(); i++)
	{
		if(_stricmp(i->name.c_str(), name) == 0 && i->page == page)
		{
			i->opened = true;
			return true;
		}
	}

	return false;
}

void NextPage(const char* name)
{
	int page = 0;

	for(auto i=g_GUI.view.begin(); i!=g_GUI.view.end(); i++)
	{
		if(_stricmp(i->name.c_str(), name) == 0 && i->opened)
		{
			page = i->page;
			i->opened = false;
			break;
		}
	}

	if(!OpenAnotherView(name, page+1))
		OpenAnotherView(name, 0);
}

void UpdateStatus(const char* status, bool logthis)
{
	if(logthis)
	{
		g_log<<status<<endl;
		g_log.flush();
	}
	/*
	char upper[1024];
	int i;
	for(i=0; i<strlen(status); i++)
	{
		upper[i] = toupper(status[i]);
	}
	upper[i] = '\0';*/

	//g_GUI.getview("loading")->getwidget("status", WIDGET_TEXT)->m_text = upper;
	g_GUI.getview("loading")->getwidget("status", WIDGET_TEXT)->m_text = status;
}

bool MousePosition()
{
	Vec2i old = g_mouse;
#ifdef PLATFORM_WIN32
	POINT mouse;
	GetCursorPos(&mouse);
	ScreenToClient(g_hWnd, &mouse);
#else
    Vec2i mouse;
    SDL_GetMouseState(&mouse.x, &mouse.y);
#endif

	if(mouse.x == old.x && mouse.y == old.y)
		return false;

	g_mouse.x = mouse.x;
	g_mouse.y = mouse.y;

	return true;
}

void CenterMouse()
{
	g_mouse.x = g_width/2;
	g_mouse.y = g_height/2;
#ifdef PLATFORM_WIN32
	POINT temp;
	temp.x = g_mouse.x;
	temp.y = g_mouse.y;
	ClientToScreen(g_hWnd, &temp);
	SetCursorPos(temp.x, temp.y);
#else
    SDL_WarpMouseInWindow(g_window, g_mouse.x, g_mouse.y);
#endif
}

void Ortho(int width, int height, float r, float g, float b, float a)
{
	UseS(SHADER_ORTHO);
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_WIDTH], (float)width);
	glUniform1f(g_shader[SHADER_ORTHO].m_slot[SSLOT_HEIGHT], (float)height);
	glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], r, g, b, a);
	glEnableVertexAttribArray(g_shader[SHADER_ORTHO].m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXCOORD0]);
	g_currw = width;
	g_currh = height;
}
