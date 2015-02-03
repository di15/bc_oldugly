
#include "gmain.h"
#include "../common/math/3dmath.h"
#include "../common/gui/gui.h"
#include "../common/texture.h"
#include "keymap.h"
#include "ggui.h"
#include "../common/math/vec3f.h"
#include "gviewport.h"
#include "../common/draw/shadow.h"

int g_projtype = PROJ_PERSP;

#define LEFT_PANEL_WIDTH	200
#define TOP_PANEL_HEIGHT 200

void FillGUI()
{
	DrawSceneDepthFunc = &DrawSceneDepth;
	DrawSceneFunc = &DrawScene;

	//AssignAnyKey(&AnyKeyDown, &AnyKeyUp);
	//AssignKey(VK_ESCAPE, &Escape, NULL);
	//AssignKey(VK_DELETE, &Delete, NULL);
	//AssignKey('C', Down_C, NULL);
	//AssignKey('V', Down_V, NULL);
	//AssignMouseWheel(&MouseWheel);
	//AssignMouseMove(&MouseMove);
	//AssignLButton(&MouseLeftButtonDown, &MouseLeftButtonUp);
	//AssignRButton(NULL, &MouseRightButtonUp);
	
	BitmapFont* f = &g_font[MAINFONT8];

	View* logoview = AddView("logo");
	logoview->widget.push_back(new Image(NULL, "logo", "gui/dmd.jpg", Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 1.0), ALIGNMENT_CENTER, 1,1,1,0));

	View* loadingview = AddView("loading");
	loadingview->widget.push_back(new Text(NULL, "status", "Loading...", MAINFONT8, Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.4f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f)));

	View* playview = AddView("play");
	
#if 0
	playview->widget.push_back(new Frame(NULL, "viewports frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));
	Widget* viewportsframe = playview->getwidget("viewports frame", WIDGET_FRAME);
#endif

	playview->widget.push_back(new ViewportW(NULL, "main 3d viewport", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f), &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, ViewportMDown, ViewportMUp, 0));
	
	//g_viewportT[VIEWPORT_MAIN3D] = ViewportT(Vec3f(0, 0, MAX_DISTANCE/2), Vec3f(0, 1, 0), "Front");
	//g_viewportT[VIEWPORT_TOP] = ViewportT(Vec3f(0, MAX_DISTANCE/2, 0), Vec3f(0, 0, -1), "Top");
	//g_viewportT[VIEWPORT_LEFT] = ViewportT(Vec3f(MAX_DISTANCE/2, 0, 0), Vec3f(0, 1, 0), "Left");
	g_viewportT[VIEWPORT_MAIN3D] = ViewportT(Vec3f(1000.0f/3, 1000.0f/3, 1000.0f/3), Vec3f(0, 1, 0), "Angle");
	g_camera.position(1000.0f/3, 1000.0f/3, 1000.0f/3, 0, 0, 0, 0, 1, 0);

	g_viewport[0] = Viewport(VIEWPORT_MAIN3D);
	
#if 0
	viewportsframe->m_subwidg.push_back(new Frame(viewportsframe, "viewports right half frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));
	viewportsframe->m_subwidg.push_back(new Frame(viewportsframe, "viewports bottom half frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));
	
	Widget* viewportsrighthalf = viewportsframe->getsubwidg("viewports right half frame", WIDGET_FRAME);
	Widget* viewportsbottomhalf = viewportsframe->getsubwidg("viewports bottom half frame", WIDGET_FRAME);
	
	viewportsrighthalf->m_subwidg.push_back(new Image(viewportsrighthalf, "h divider", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1), ALIGNMENT_LESSERSIDE));
	viewportsbottomhalf->m_subwidg.push_back(new Image(viewportsbottomhalf, "v divider", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 1), ALIGNMENT_LESSERSIDE));
#endif

	OpenSoleView("logo");
}

void DrawMarquee()
{
	if(!g_mousekeys[0])
		return;

	UseS(SHADER_COLOR2D);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)g_width);
	glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)g_height);
	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], 0, 1, 0, 0.75f);
	glEnableVertexAttribArray(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(g_shader[SHADER_COLOR2D].m_slot[SSLOT_TEXCOORD0]);

	float vertices[] =
	{
		//posx, posy    texx, texy
		g_mousestart.x, g_mousestart.y, 0,          0, 0,
		g_mousestart.x, g_mouse.y,0,         1, 0,
		g_mouse.x, g_mouse.y,0,      1, 1,

		g_mouse.x, g_mousestart.y,0,      1, 1,
		g_mousestart.x, g_mousestart.y,0,       0, 1
	};

	glVertexAttribPointer(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);

	glDrawArrays(GL_LINE_STRIP, 0, 5);
}