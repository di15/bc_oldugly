

#include "keymap.h"
#include "gmain.h"
#include "../common/gui/gui.h"
#include "../common/draw/shader.h"
#include "res.h"
#include "../common/gui/font.h"
#include "../common/texture.h"
#include "../common/draw/model.h"
#include "../common/math/frustum.h"
#include "../common/draw/billboard.h"
#include "ggui.h"
#include "../common/draw/particle.h"
#include "../common/draw/shadow.h"
#include "../common/platform.h"
#include "../common/utils.h"
#include "../common/window.h"
#include "../common/sim/sim.h"
#include "../common/math/matrix.h"
#include "../common/math/vec3f.h"
#include "../common/math/camera.h"
#include "../common/save/savemap.h"
#include "../common/sim/map.h"
#include "../common/sim/entity.h"
#include "../common/sim/entitytype.h"
#include "../common/sim/selection.h"
#include "../common/sim/order.h"
#include "../common/sim/entity.h"
#include "../common/save/edmap.h"

APPMODE g_mode = LOADING;
bool g_mouseout = false;
bool g_gameover = false;
bool g_moved = false;	//Was the mouse moved after it was first pressed down?
string g_startmap;

string g_coldebug = "";

void TrackMouse()
{
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = HOVER_DEFAULT;	//100;
	tme.hwndTrack = g_hWnd;
	TrackMouseEvent(&tme);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_SYSCOMMAND:
		{
			switch (wParam)
			{
				case SC_SCREENSAVE: // Screensaver trying to start?
				case SC_MONITORPOWER: // Monitor trying to enter powersave?
				return 0; // Prevent from happening
			}
			break;
		}
			
		case WM_CREATE:
		{
			if(g_fullscreen)
				CenterMouse();
		}break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_MOUSELEAVE:
		{
			//TrackMouse();
			g_mouseout = true;
		}break;

		case WM_MOUSEHOVER:
		{
			TrackMouse();
			g_mouseout = false;
		}break;

		case WM_LBUTTONDOWN:
		{
			g_mousekeys[0] = true;
			g_moved = false;
			g_GUI.lbuttondown();
		}break;
			
		case WM_LBUTTONUP:
		{
			g_mousekeys[0] = false;
			g_GUI.lbuttonup(g_moved);
		}break;
		
		case WM_RBUTTONDOWN:
		{
			g_mousekeys[2] = true;
			g_moved = false;
			g_GUI.rbuttondown();
		}break;
			
		case WM_RBUTTONUP:
		{
			g_mousekeys[2] = false;
			g_GUI.rbuttonup(g_moved);
		}break;
		
		case WM_MBUTTONDOWN:
		{
			g_mousekeys[1] = true;
			g_moved = false;
			g_GUI.mbuttondown();
		}break;
			
		case WM_MBUTTONUP:
		{
			g_mousekeys[1] = false;
			g_GUI.mbuttonup(g_moved);
		}break;

		case WM_MOUSEMOVE:
		{
			if(g_mouseout)
			{
				TrackMouse();
				g_mouseout = false;
			}
			if(MousePosition())
			{
				g_moved = true;
				g_GUI.mousemove();
			}
		}break;

		case WM_MOUSEWHEEL:
		{
			g_GUI.mousewheel(GET_WHEEL_DELTA_WPARAM(wParam) / WHEEL_DELTA);
		}break;

		case WM_KEYDOWN:
		{
			if(!g_GUI.keydown(wParam))
				g_keys[wParam] = true;
			return 0;
		}

		case WM_KEYUP:
		{
			if(!g_GUI.keyup(wParam))
				g_keys[wParam] = false;	
			return 0;
		}

		case WM_CHAR:
		{
			g_GUI.charin(wParam);
			return 0;
		}

		case WM_SIZE:
		{
			Resize(LOWORD(lParam), HIWORD(lParam)); 
			return 0;
		}
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void AfterDraw(Matrix projection, Matrix viewmat, Matrix modelmat)
{
	//g_log<<"sizeof(g_scenery) = "<<sizeof(g_scenery)<<endl;

	Matrix mvp = projection;
	mvp.postMultiply(viewmat);
	mvp.postMultiply(modelmat);
	
	//DrawSelection(projection, viewmat, modelmat);
	
	UseS(SHADER_COLOR3D);
    glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_PROJECTION], 1, 0, projection.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_COLOR3D].m_slot[SSLOT_VIEWMAT], 1, 0, viewmat.m_matrix);
	glEnableVertexAttribArray(g_shader[SHADER_COLOR3D].m_slot[SSLOT_POSITION]);
	glUniform4f(g_shader[SHADER_COLOR3D].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	//DrawGrid();
	//DrawUnitSquares();
	//DrawPaths();
	//DrawVelocities();

	UseS(SHADER_BILLBOARD);
    glUniformMatrix4fv(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_PROJECTION], 1, 0, projection.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_MODELMAT], 1, 0, modelmat.m_matrix);
	glUniformMatrix4fv(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_VIEWMAT], 1, 0, viewmat.m_matrix);
	glUniform4f(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	glEnableVertexAttribArray(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_TEXCOORD0]);
	//glEnableVertexAttribArray(g_shader[SHADER_BILLBOARD].m_slot[SSLOT_NORMAL]);
    glActiveTextureARB(GL_TEXTURE0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//DrawOrders();
	//DrawProjectiles();
	//SortParticles();
	//DrawParticles();
	//BeginVertexArrays();
	SortBillboards();
	DrawBillboards();
	//EndVertexArrays();
	
	Ortho(g_width, g_height, 1, 1, 1, 1);
	
	//DrawUnitStatus(mvp);
	//DrawBStatus(mvp);
	//DrawTransactions(mvp);
	
	UseS(SHADER_COLOR2D);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)g_width);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)g_height);
    glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], 1, 1, 1, STATUS_ALPHA);
    glEnableVertexAttribArray(g_shader[SHADER_COLOR2D].m_slot[SSLOT_POSITION]);

	//DrawUnitStats(mvp);
	//DrawBStats(mvp);
}

void DrawFilled()
{
	//UseS(SHADER_COLOR3D);
	Shader* shader = &g_shader[g_curS];
	glUniform4f(shader->m_slot[SSLOT_COLOR], 0.2f, 0.3f, 0.9f, 0.5f);

	for(int i=0; i<g_selB.size(); i++)
	{
		Brush* b = g_selB[i];

		int brushindex = b - g_map.m_brush;
		char part[128];
		sprintf(part, "%d,", brushindex);
		g_coldebug.append(part);

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* side = &b->m_sides[j];

			//glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, va->vertices);
			//glVertexAttribPointer(shader->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, 0, va->texcoords);
			//glVertexAttribPointer(shader->m_slot[SSLOT_NORMAL], 3, GL_FLOAT, GL_FALSE, 0, va->normals);

			//glDrawArrays(GL_TRIANGLES, 0, va->numverts);

			glVertexAttribPointer(shader->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, side->m_drawva.vertices);

			glDrawArrays(GL_TRIANGLES, 0, side->m_drawva.numverts);
		}
	}

	g_selB.clear();
}

void DrawScene(Matrix projection, Matrix viewmat, Matrix modelmat, Matrix modelviewinv, float mvLightPos[3], float lightDir[3])
{
	UseShadow(SHADER_MAP, projection, viewmat, modelmat, modelviewinv, mvLightPos, lightDir);
	glActiveTextureARB(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1iARB(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 4);
	/*
	glActiveTextureARB(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, g_transparency);
	glUniform1iARB(g_shader[SHADER_OWNED].m_slot[SSLOT_TEXTURE1], 1);
	glActiveTextureARB(GL_TEXTURE0);*/
	
	DrawMap(&g_map);
	DrawEntities();
#if 0
	for(int i=0; i<10; i++)
		for(int j=0; j<5; j++)
			g_model[themodel].draw(0, Vec3f(-5*180 + 180*i,0,-2.5f*90 + j*90), 0);
#endif
	DrawSelectionCircles(&projection, &modelmat, &viewmat);
	DrawOrders(&projection, &modelmat, &viewmat);
	
	UseShadow(SHADER_MAP, projection, viewmat, modelmat, modelviewinv, mvLightPos, lightDir);
	glActiveTextureARB(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, g_depth);
	glUniform1iARB(g_shader[g_curS].m_slot[SSLOT_SHADOWMAP], 4);
	DrawMap2(&g_map);
	
#if 0
	UseS(SHADER_COLOR3DPERSP);
	Shader* s = &g_shader[g_curS];
	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, GL_FALSE, projection.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, GL_FALSE, viewmat.m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, GL_FALSE, modelmat.m_matrix);
	glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	DrawFilled();
	//g_log<<"draw scene"<<endl;
	//g_log.flush();
#endif

	Ortho(g_width, g_height, 1, 1, 1, 1);
	glDisable(GL_DEPTH_TEST);
	DrawShadowedText(MAINFONT8, 5, 15, g_coldebug.c_str());
	g_coldebug = "";
	glEnable(GL_DEPTH_TEST);
}

void DrawSceneDepth()
{
	//g_model[themodel].draw(0, Vec3f(0,0,0), 0);

	DrawMap(&g_map);
	
	DrawEntities();
#if 0
	for(int i=0; i<10; i++)
		for(int j=0; j<5; j++)
			g_model[themodel].draw(0, Vec3f(-5*180 + 180*i,0,-2.5f*90 + j*90), 0);
#endif

	DrawMap2(&g_map);
}

void Draw()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*
	if(g_mode == PLAY)
	{
		
		//TurnOffShader();
		//g_camera.Look();
		
		float aspect = fabsf((float)g_width / (float)g_height);
		//Matrix projection = BuildPerspProjMat(FIELD_OF_VIEW, aspect, MIN_DISTANCE, MAX_DISTANCE);
		Matrix projection = setorthographicmat(-PROJ_RIGHT*aspect, PROJ_RIGHT*aspect, PROJ_RIGHT, -PROJ_RIGHT, MIN_DISTANCE, MAX_DISTANCE);
		
		Vec3f viewvec = g_camera.m_view;
        Vec3f posvec = g_camera.m_pos;
        Vec3f posvec2 = g_camera.lookpos();
        Vec3f upvec = g_camera.m_up;

		//char msg[128];
		//sprintf(msg, "y = %f", posvec.y);
		//Chat(msg);
        
        Matrix viewmat = gluLookAt2(posvec2.x, posvec2.y, posvec2.z,
                                    viewvec.x, viewvec.y, viewvec.z,
                                    upvec.x, upvec.y, upvec.z);
		
		Matrix modelview;
		Matrix modelmat;
		float translation[] = {0, 0, 0};
		modelview.setTranslation(translation);
		modelmat.setTranslation(translation);
		modelview.postMultiply(viewmat);

		g_frustum.CalculateFrustum(projection.m_matrix, modelview.m_matrix);
		
		RenderToShadowMap(projection, viewmat, modelmat, viewvec);
		RenderShadowedScene(projection, viewmat, modelmat, modelview);
		AfterDraw(projection, viewmat, modelmat);
	}*/

	g_GUI.frameupd();
	g_GUI.draw();

	DrawMarquee();

	Ortho(g_width, g_height, 1, 1, 1, 1);
	DrawShadowedText(MAINFONT8, 0, 0, g_startmap.c_str());

	SwapBuffers(g_hDC);
}

void UpdateLoading()
{
	static int stage = 0;

	switch(stage)
	{
	//case 0: Status("Loading textures...", true); stage++; break;
	//case 1: LoadTiles(); LoadMap(); LoadTerrainTextures(); LoadHoverTex(); LoadSkyBox("defsky"); Status("Loading particles...", true); stage++; break;
	//case 2: LoadParticles(); Status("Loading projectiles...", true); stage++; break;
	//case 3: LoadProjectiles(); Status("Loading unit sprites...", true); stage++; break;
	//case 4: LoadUnitSprites(); Status("Loading sounds...", true); stage++; break;
	//case 5: LoadSounds(); Status("Loading Map sprites...", true); stage++; break;
	//case 6: BSprites(); Status("Loading models...", true); stage++; break;
	case 0: if(!Load1Model()) stage++; break;
	case 1: if(!Load1Texture()) stage++; break;
	case 2:
		// Were we given a command-line argument to start
		// a map?
		if(g_startmap.size() > 0)
		{
			g_log<<"Loading map "<<g_startmap<<endl;
			g_log.flush();

			LoadMap(g_startmap.c_str(), &g_map);
			
			//float height = 188;
			float height = 1500;

			PlaceEntity(ENTITY_BATTLECOMPUTER, -1, 1, 0, Vec3f(0, height, 0), rand()%360, NULL);
			PlaceEntity(ENTITY_BATTLECOMPUTER, -1, 1, 0, Vec3f(-78, height, 0), rand()%360, NULL);
			PlaceEntity(ENTITY_BATTLECOMPUTER, -1, 1, 0, Vec3f(0, height, 120), rand()%360, NULL);
			PlaceEntity(ENTITY_BATTLECOMPUTER, -1, 1, 0, Vec3f(100, height, 90), rand()%360, NULL);
			
			//for(int i=0; i<5; i++)
			for(int i=0; i<0; i++)
			{
				int entid;
				PlaceEntity(ENTITY_BATTLECOMPUTER, -1, 1, 0, Vec3f(0, height, 0), rand()%360, &entid);

				Entity* pent = g_entity[entid];
				
				Camera* cam = &pent->camera;
				EntityT* t = &g_entityT[pent->type];

				Vec3f bodymin = pent->bodymin();
				Vec3f bodymax = t->vmax;
				
				cam->grounded(false);

				
				for(int j=0; j<5; j++)
				{
					TraceWork tw;
					Vec3f old = cam->m_pos;

					cam->grounded(false);

					g_map.tracebox(&tw, old, old + Vec3f(rand()%200-100, rand()%100-50, rand()%200-100), bodymin, bodymax, t->maxStep);

					cam->moveto(tw.clip);

					if(tw.onground)
						cam->grounded(true);
				}
			}

			g_log<<"Play mode"<<endl;
			g_log.flush();

			g_mode = PLAY;
			OpenSoleView("play");
			break;
		}

		//Status("logo");
		g_mode = MENU;
		OpenSoleView("editor");
		//g_mode = LOGO;
		//OpenSoleView("logo");
		break;
	}
}

int g_reStage = 0;
void UpdateReloading()
{
	switch(g_reStage)
	{
	case 0:
		if(Load1Texture())
		{
			g_mode = MENU;
		}
		break;
	}
}

void SkipLogo()
{
	if(g_mode != LOGO)
		return;

	//g_mode = LOADING;
	//OpenSoleView("loading");
	g_mode = MENU;
	OpenSoleView("main");
}

void UpdateLogo()
{
	static int stage = 0;

	if(stage < 60)
	{
		float a = (float)stage / 60.0f;
		g_GUI.getview("logo")->getwidget("logo", WIDGET_IMAGE)->m_rgba[3] = a;
	}
	else if(stage < 120)
	{
		float a = 1.0f - (float)(stage-60) / 60.0f;
		g_GUI.getview("logo")->getwidget("logo", WIDGET_IMAGE)->m_rgba[3] = a;
	}
	else
		SkipLogo();

	stage++;
}

void UpdateScroll()
{
	if(g_keys['W'])
	{
		g_camera.accelerate(50.0f/g_zoom);
	}
	if(g_keys['S'])
	{
		g_camera.accelerate(-50.0f/g_zoom);
	}
	
	if(g_keys['A'])
	{
		g_camera.accelstrafe(-50.0f/g_zoom);
	}
	if(g_keys['D'])
	{
		g_camera.accelstrafe(50.0f/g_zoom);
	}
	
	if(g_keys['R'])
	{
		g_camera.accelrise(25.0f/g_zoom);
	}
	if(g_keys['F'])
	{
		g_camera.accelrise(-25.0f/g_zoom);
	}
	
	g_camera.frameupd();
	g_camera.friction2();
}

void UpdateGameState()
{
	UpdateScroll();
	//CalculateFrameRate();
	//Scroll();
	//LastNum("pre upd u");
	UpdateEntities();
	//LastNum("pre upd b");
	//UpdateMaps();
	//LastNum("post upd b");
	//UpdateParticles();
	//LastNum("up pl");
	//UpdatePlayers();
	//LastNum("up ai");
	//UpdateAI();
	//ResourceTicker();
	//UpdateTimes();
	//UpdateFPS();
}

void Update()
{
	if(g_mode == LOADING)
		UpdateLoading();
	else if(g_mode == LOGO)
		UpdateLogo();
	//else if(g_mode == INTRO)
	//	UpdateIntro();
	else if(g_mode == PLAY)
		UpdateGameState();
	//else if(g_mode == EDITOR)
	//	UpdateEditor();
	else if(g_mode == RELOADING)
		UpdateReloading();
}

void LoadConfig()
{
	char cfgfull[MAX_PATH+1];
	FullPath(CONFIGFILE, cfgfull);
	ifstream config(cfgfull);

	//getline(config, g_username);
	//getline(config, g_servername);

	int fulls;
	config>>fulls;
	
	if(fulls)
		g_fullscreen = true;
	else
		g_fullscreen = false;

	config>>g_selectedRes.width>>g_selectedRes.height;
	config>>g_bpp;

	g_width = g_selectedRes.width;
	g_height = g_selectedRes.height;
}

void WriteConfig()
{
	ofstream config;
	config.open(CONFIGFILE, ios_base::out);
	
	//config<<g_username<<endl;
	//config<<g_servername<<endl;
	
	int fulls;
	if(g_fullscreen)
		fulls = 1;
	else
		fulls = 0;

	config<<fulls<<endl;
	config<<g_selectedRes.width<<" "<<g_selectedRes.height<<endl;
	config<<g_bpp;
}

/*
void EnumerateMaps()
{
	WIN32_FIND_DATA ffd;
	string bldgPath = ExePath() + "\\bldgs\\*";
	HANDLE hFind = FindFirstFile(bldgPath.c_str(), &ffd);

	if(INVALID_HANDLE_VALUE != hFind)
	{
		do
		{
			if(!strstr(ffd.cFileName, ".bsp"))
				continue;

			int pos = string( ffd.cFileName ).find_last_of( ".bsp" );
			string name = string( ffd.cFileName ).substr(0, pos-3);

			g_bldgs.push_back(name);
		} while(FindNextFile(hFind, &ffd) != 0);
		FindClose(hFind);
	}
	else
	{
		// Failure
	}
}*/

void StripMap(const char* cmdline)
{
	if(stricmp(cmdline, "") == 0)
		g_startmap = "";
	else
	{
		string cmdlinestr(cmdline);
		string find("+devmap ");
		int found = cmdlinestr.rfind(find);

		g_startmap = cmdlinestr.substr(found+find.length(), cmdlinestr.length()-found-find.length());

		g_log<<cmdline<<endl;
		g_log<<g_startmap<<endl;
	}
}

void Init()
{
	//For some reason when a map project is loaded
	//and the working/current directory is set to
	//that folder, something breaks. Working directory
	//must be exe path for drawing to work.
	char exepath[MAX_PATH+1];
	ExePath(exepath);
	SetCurrentDirectory(exepath);

	OpenLog("gamelog.txt", VERSION);

	srand(GetTickCount());
	
	LoadConfig();
	//EnumerateMaps();
	EnumerateDisplay();
	MapKeys();

	InitOnce();
}

void Deinit()
{
	//FreeScript();
	DestroyEntities();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//int main(int argc, char** argv)
{
	MSG msg;
	g_hInstance = hInstance;
	
	Init();
	StripMap(lpCmdLine);

	if(!MakeWindow(TEXT(TITLE), LoadIcon(g_hInstance, MAKEINTRESOURCE(IDI_GUN)), &WndProc))
		return 0;
	
	Queue();
	FillGUI();

	while(!g_quit)
	{
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
				g_quit = true;
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if ((g_mode == LOADING || g_mode == RELOADING) || AnimateNextFrame(FRAME_RATE))
			{
				Update();
				Draw();
			}
			else
				Sleep(1);
		}
	}

	DestroyWindow(TEXT(TITLE));
	Deinit();

	return msg.wParam;
}


