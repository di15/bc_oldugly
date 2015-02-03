


#include "wemain.h"
#include "../common/math/3dmath.h"
#include "../common/gui/gui.h"
#include "../common/texture.h"
#include "wegui.h"
#include "../common/window.h"
#include "weviewport.h"
#include "wesim.h"
#include "../common/save/save.h"
#include "../common/save/saveedm.h"
#include "../common/sim/sim.h"
#include "../common/draw/sortb.h"
#include "undo.h"
#include "../common/save/compilemap.h"
#include "../common/draw/shadow.h"
#include "../common/keycodes.h"

int g_projtype = PROJ_ORTHO;
bool g_showsky = false;
float g_snapgrid = 25;
char g_lastsave[MAX_PATH+1];

void AnyKeyDown(int k)
{
	SkipLogo();
}

void AnyKeyUp(int k)
{
}

void Escape()
{
	if(g_mode != EDITOR)
		return;
}

void Delete()
{
	if(g_mode != EDITOR)
		return;

	if(g_selB.size() > 0)
		LinkPrevUndo();

	for(int i=0; i<g_selB.size(); i++)
	{
		for(auto j=g_edmap.m_brush.begin(); j!=g_edmap.m_brush.end(); j++)
		{
			if(g_selB[i] == &*j)
			{
				g_edmap.m_brush.erase(j);
				break;
			}
		}
	}

	g_selB.clear();
	g_sel1b = NULL;

	CloseSideView();

	//LinkLatestUndo();
	//g_log<<"delete b"<<endl;
	//g_log.flush();
}

void MouseLeftButtonDown()
{
	if(g_mode == LOGO)
		SkipLogo();
	else if(g_mode == EDITOR)
	{

	}
}

void MouseLeftButtonUp()
{
}

void Change_RotDeg(int dummy)
{
}

void Change_Zoom(int dummy)
{
	Widget* zoombox = g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("zoom", WIDGET_EDITBOX);

	//if(zoombox->m_value.c_str()[0] == '\0')
	//	return;

	g_zoom = StrToFloat(zoombox->m_value.c_str());

	if(g_zoom <= 0.0f)
		g_zoom = 1.0f;
}

//void Change_SnapGrid(int dummy)
void Change_SnapGrid()
{
	Widget* snapgbox = g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("snapgrid", WIDGET_DROPDOWNSELECTOR);

	//if(snapgbox->m_value.c_str()[0] == '\0')
	//	return;

	//g_snapgrid = StrToFloat(snapgbox->m_value.c_str());
	//int power = snapgbox->m_selected;
	//g_snapgrid = 400 / pow(2, power);

	float cm_scales[] = CM_SCALES;

	g_snapgrid = cm_scales[ snapgbox->m_selected ];

	if(g_snapgrid <= 0.0f)
		g_snapgrid = 1;
}

void Change_MaxElev(int dummy)
{
	Widget* maxelevbox = g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("maxelev", WIDGET_EDITBOX);

	g_maxelev = StrToFloat(maxelevbox->m_value.c_str());
}

void SkipLogo()
{
	if(g_mode != LOGO)
		return;

	//g_mode = LOADING;
	//OpenSoleView("loading");
	g_mode = EDITOR;
	OpenSoleView("editor");
	//OpenAnotherView("brush edit");
}

void UpdateLogo()
{
	static int stage = 0;
	//g_log<<"update logo "<<stage<<endl;
	//g_log.flush();

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

void Click_NewBrush()
{
	LinkPrevUndo();
	//Vec3f pos = g_focus;
	Vec3f pos = g_camera.m_view;
	pos.x = Snap(g_snapgrid, pos.x);
	pos.y = Snap(g_snapgrid, pos.y);
	pos.z = Snap(g_snapgrid, pos.z);
	Brush b;
	BrushSide top(Vec3f(0,1,0), pos + Vec3f(0,STOREY_HEIGHT/2.0f,0));
	BrushSide bottom(Vec3f(0,-1,0), pos + Vec3f(0,-STOREY_HEIGHT/2.0f,0));
	BrushSide left(Vec3f(-1,0,0), pos + Vec3f(-STOREY_HEIGHT/2.0f,0,0));
	BrushSide right(Vec3f(1,0,0), pos + Vec3f(STOREY_HEIGHT/2.0f,0,0));
	BrushSide front(Vec3f(0,0,1), pos + Vec3f(0,0,STOREY_HEIGHT/2.0f));
	BrushSide back(Vec3f(0,0,-1), pos + Vec3f(0,0,-STOREY_HEIGHT/2.0f));
	b.add(top);		//0
	b.add(bottom);	//1
	b.add(left);	//2
	b.add(right);	//3
	b.add(front);	//4
	b.add(back);	//5
	b.collapse();
	b.remaptex();
	//g_log<<"---------push back brush-----------"<<endl;
	g_edmap.m_brush.push_back(b);
	//g_selB.clear();
	//auto i = g_edmap.m_brush.rbegin();
	//g_selB.push_back(&*i);

	ViewportT* t = &g_viewportT[VIEWPORT_ANGLE45O];
	//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
	SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
	//LinkLatestUndo();
}

void Click_LoadEdMap()
{
#ifdef PLATFORM_WIN32
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("map projects\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("map projects\\map project", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	FreeEdMap(&g_edmap);
	if(LoadEdMap(filepath, &g_edmap))
		strcpy(g_lastsave, filepath);

	ViewportT* t = &g_viewportT[VIEWPORT_ANGLE45O];
	//SortEdB(&g_edmap, g_focus, g_focus + t->m_offset);
	SortEdB(&g_edmap, g_camera.m_view, g_camera.m_pos);
	ClearUndo();
#endif
}

void Click_SaveEdMap()
{
#ifdef PLATFORM_WIN32
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("map projects\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("map projects\\map project", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if(!GetSaveFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	SaveEdMap(filepath, &g_edmap);
	strcpy(g_lastsave, filepath);
#endif
}

void Click_QSaveEdMap()
{
	if(g_lastsave[0] == '\0')
		return;

	SaveEdMap(g_lastsave, &g_edmap);
}

void Click_CompileMap()
{
#ifdef PLATFORM_WIN32
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("maps\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("maps\\map", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if(!GetSaveFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	CompileMap(filepath, &g_edmap);
#endif
}

void RunMap(const char* full)
{
	// TO DO: launch game

	char relativeexe[] = "BattleComputer.exe";
	char fullexe[1024];
	FullPath(relativeexe, fullexe);
	char relativemap[] = "temp/tempmap";
	char fullmap[1024];
	FullPath(relativemap, fullmap);

	char cmd[2048];
	sprintf(cmd, "%s +devmap %s", fullexe, fullmap);
	system(cmd);
}

void Click_CompileRunMap()
{
	/*
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("maps\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("maps\\map", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL  ;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_OVERWRITEPROMPT;

	if(!GetSaveFileName(&ofn))
		return;

	//CorrectSlashes(filepath);
	CompileMap(filepath, &g_edmap);
	*/

	char filepath[MAX_PATH+1];
	FullPath("temp/tempmap", filepath);
	CompileMap(filepath, &g_edmap);
	RunMap(filepath);
}

void Click_BChooseTex()
{
#ifdef PLATFORM_WIN32
	OPENFILENAME ofn;

	char filepath[MAX_PATH+1];

	ZeroMemory( &ofn , sizeof( ofn));

	char initdir[MAX_PATH+1];
	FullPath("textures\\", initdir);
	CorrectSlashes(initdir);
	//strcpy(filepath, initdir);
	FullPath("textures\\texture", filepath);
	CorrectSlashes(filepath);

	ofn.lStructSize = sizeof ( ofn );
	ofn.hwndOwner = NULL;
	ofn.lpstrInitialDir = initdir;
	ofn.lpstrFile = filepath;
	//ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof( filepath );
	//ofn.lpstrFilter = "Save\0*.map\0All\0*.*\0";
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = MAX_PATH;	//0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST|OFN_FILEMUSTEXIST;

	if(!GetOpenFileName(&ofn))
		return;

	LinkPrevUndo();

	unsigned int diffuseindex;
	string relativepath = MakePathRelative(filepath);
	CreateTexture(diffuseindex, relativepath.c_str(), false);
	unsigned int texname = g_texture[diffuseindex].texname;

	char specpath[MAX_PATH+1];
	strcpy(specpath, relativepath.c_str());
	StripExtension(specpath);
	strcat(specpath, ".spec.jpg");

	unsigned int specindex;
	CreateTexture(specindex, specpath, false);

	char normpath[MAX_PATH+1];
	strcpy(normpath, relativepath.c_str());
	StripExtension(normpath);
	strcat(normpath, ".norm.jpg");

	unsigned int normindex;
	CreateTexture(normindex, normpath, false);

	if(g_sel1b == NULL)
	{
		for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
		{
			Brush* b = *i;

			b->m_texture = diffuseindex;

			for(int j=0; j<b->m_nsides; j++)
			{
				BrushSide* s = &b->m_sides[j];
				s->m_diffusem = diffuseindex;
				s->m_specularm = specindex;
				s->m_normalm = normindex;
			}
		}
	}
	else
	{
		Brush* b = g_sel1b;

		b->m_texture = diffuseindex;

		if(g_dragS >= 0)
		{
			BrushSide* s = &b->m_sides[g_dragS];
			s->m_diffusem = diffuseindex;
			s->m_specularm = specindex;
			s->m_normalm = normindex;
		}
		else
		{
			for(int i=0; i<b->m_nsides; i++)
			{
				BrushSide* s = &b->m_sides[i];
				s->m_diffusem = diffuseindex;
				s->m_specularm = specindex;
				s->m_normalm = normindex;
			}
		}
	}
#endif
}

void Click_FitToFace()
{
	if(g_sel1b == NULL)
	{
		for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
		{
			Brush* b = *i;

			for(int j=0; j<b->m_nsides; j++)
				b->m_sides[j].fittex();
		}
	}
	else
	{
		Brush* b = g_sel1b;

		if(g_dragS >= 0)
			b->m_sides[g_dragS].fittex();
		else
		{
			for(int i=0; i<b->m_nsides; i++)
				b->m_sides[i].fittex();
		}
	}

	RedoBSideGUI();
}

void CloseSideView()
{
	CloseView("brush side edit");
	CloseView("brush edit");
	CloseView("door edit");
}

void Click_DoorView()
{
	CloseSideView();
	OpenAnotherView("door edit");
}

void CopyBrush()
{
	if(g_selB.size() <= 0)
		return;

	g_copyB = *g_selB[0];

	//g_log<<"copy brush"<<endl;
	//g_log.flush();
}

void PasteBrush()
{
	//g_log<<"paste brush?"<<endl;
	//g_log.flush();

	if(g_copyB.m_nsides <= 0)
		return;

	//Vec3f pos = g_focus;
	Vec3f pos = g_camera.m_view;
	pos.x = Snap(g_snapgrid, pos.x);
	pos.y = Snap(g_snapgrid, pos.y);
	pos.z = Snap(g_snapgrid, pos.z);
	Brush b = g_copyB;
	//b.moveto(pos);
/*
	for(int i=0; i<b.m_nsides; i++)
	{
		BrushSide* s = &b.m_sides[i];

		g_log<<"side"<<i<<" plane="<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<","<<s->m_plane.m_d<<endl;
	}
	*/
	g_edmap.m_brush.push_back(b);

	//g_log<<"paste brush"<<endl;
	//g_log.flush();
}

void Down_C()
{
	if(g_mode != EDITOR)
		return;

	//if(g_keys[VK_CONTROL])
		CopyBrush();
}

void Down_V()
{
	if(g_mode != EDITOR)
		return;

	//if(g_keys[VK_CONTROL])
		PasteBrush();
}

void Click_CutBrush()
{
	g_edtool = EDTOOL_CUT;
}

void RotateBrushes(float radians, Vec3f axis)
{
	LinkPrevUndo();
	//EdMap* m = &g_edmap;

	//for(auto i=map->m_brush.begin(); i!=map->m_brush.end(); i++)
	for(auto i=g_selB.begin(); i!=g_selB.end(); i++)
	{
		Brush* b = *i;
		Vec3f centroid(0,0,0);

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* s = &b->m_sides[j];

			centroid = centroid + s->m_centroid;
		}

		centroid = centroid / (float)b->m_nsides;

		list<float> oldus;
		list<float> oldvs;

		for(int j=0; j<b->m_nsides; j++)
		{
			BrushSide* s = &b->m_sides[j];

			RotatePlane(s->m_plane, centroid, radians, axis);
			//RotatePlane(s->m_tceq[0], centroid, radians, axis);
			//RotatePlane(s->m_tceq[1], centroid, radians, axis);
			//s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
			//s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);
			//RotatePlane(s->m_tceq[0], s->m_centroid, radians, axis);
			//RotatePlane(s->m_tceq[1], s->m_centroid, radians, axis);
			//Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
			//s->m_tceq[0].m_normal = RotateAround(s->m_tceq[0].m_normal, sharedv, radians, axis.x, axis.y, axis.z);
			//s->m_tceq[1].m_normal = RotateAround(s->m_tceq[1].m_normal, sharedv, radians, axis.x, axis.y, axis.z);
			//Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
			//Vec3f offcenter = sharedv - centroid;
			//Vec3f newoffcenter = Rotate(offcenter, radians, axis.x, axis.y, axis.z);
			//float uminusd = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z;
			//float vminusd = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z;
			//s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal - offcenter, radians, axis.x, axis.y, axis.z) + newoffcenter;
			//s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal - offcenter, radians, axis.x, axis.y, axis.z) + newoffcenter;
			//s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
			//s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);
			//float newuminusd = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z;
			//float newvminusd = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z;
			//float uscale = uminusd / newuminusd;
			//float vscale = vminusd / newvminusd;
			//s->m_tceq[0].m_normal = s->m_tceq[0].m_normal * uscale;
			//s->m_tceq[1].m_normal = s->m_tceq[1].m_normal * vscale;
			/*
			Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
			float oldu = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float oldv = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
			s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);
			float newu = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float newv = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			float changeu = newu - oldu;
			float changev = newv - oldv;
			s->m_tceq[0].m_d += changeu;
			s->m_tceq[1].m_d += changev;
			*/
			Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
			float u = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float v = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			oldus.push_back(u);
			oldvs.push_back(v);
			s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
			s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);
		}

		b->collapse();

		auto oldu = oldus.begin();
		auto oldv = oldvs.begin();

		for(int j=0; j<b->m_nsides; j++, oldu++, oldv++)
		{
			BrushSide* s = &b->m_sides[j];

			Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];

			float newu = newsharedv.x*s->m_tceq[0].m_normal.x + newsharedv.y*s->m_tceq[0].m_normal.y + newsharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float newv = newsharedv.x*s->m_tceq[1].m_normal.x + newsharedv.y*s->m_tceq[1].m_normal.y + newsharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			float changeu = newu - *oldu;
			float changev = newv - *oldv;
			s->m_tceq[0].m_d -= changeu;
			s->m_tceq[1].m_d -= changev;
		}

		b->remaptex();
	}
}

void Click_RotXCCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(degrees), Vec3f(1, 0, 0));
}

void Click_RotXCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(-degrees), Vec3f(1, 0, 0));
}

void Click_RotYCCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(degrees), Vec3f(0, 1, 0));
}

void Click_RotYCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(-degrees), Vec3f(0, 1, 0));
}

void Click_RotZCCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(degrees), Vec3f(0, 0, 1));
}

void Click_RotZCW()
{
	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());
	RotateBrushes(DEGTORAD(-degrees), Vec3f(0, 0, 1));
}

void Click_RotateTex()
{
	LinkPrevUndo();

	float degrees = StrToFloat(g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("rotdeg", WIDGET_EDITBOX)->m_value.c_str());

	Brush* b = g_sel1b;
	BrushSide* s = &b->m_sides[g_dragS];

	//Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
	float oldu = s->m_centroid.x*s->m_tceq[0].m_normal.x + s->m_centroid.y*s->m_tceq[0].m_normal.y + s->m_centroid.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
	float oldv = s->m_centroid.x*s->m_tceq[1].m_normal.x + s->m_centroid.y*s->m_tceq[1].m_normal.y + s->m_centroid.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
	Vec3f axis = s->m_plane.m_normal;
	float radians = DEGTORAD(degrees);
	s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
	s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);

	//Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];
	float newu = s->m_centroid.x*s->m_tceq[0].m_normal.x + s->m_centroid.y*s->m_tceq[0].m_normal.y + s->m_centroid.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
	float newv = s->m_centroid.x*s->m_tceq[1].m_normal.x + s->m_centroid.y*s->m_tceq[1].m_normal.y + s->m_centroid.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
	float changeu = newu - oldu;
	float changev = newv - oldv;
	s->m_tceq[0].m_d -= changeu;
	s->m_tceq[1].m_d -= changev;

	s->remaptex();
}

void RedoBSideGUI()
{
	if(g_sel1b == NULL)
		return;

	if(g_dragS < 0)
		return;

	Widget* uwidg = g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("u equation", WIDGET_EDITBOX);
	Widget* vwidg = g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("v equation", WIDGET_EDITBOX);

	Plane* tceq = g_sel1b->m_sides[g_dragS].m_tceq;

	char tceqstr[256];
	sprintf(tceqstr, "%f %f %f %f", tceq[0].m_normal.x, tceq[0].m_normal.y, tceq[0].m_normal.z, tceq[0].m_d);
	uwidg->changevalue(tceqstr);
	sprintf(tceqstr, "%f %f %f %f", tceq[1].m_normal.x, tceq[1].m_normal.y, tceq[1].m_normal.z, tceq[1].m_d);
	vwidg->changevalue(tceqstr);

	uwidg->m_scroll[0] = 0;
	vwidg->m_scroll[0] = 0;
}

void Change_TexEq(int c)
{
	if(g_sel1b == NULL)
		return;

	if(g_dragS < 0)
		return;

	Widget* uwidg = g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("u equation", WIDGET_EDITBOX);
	Widget* vwidg = g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("v equation", WIDGET_EDITBOX);

	float A = 0;
	float B = 0;
	float C = 0;
	float D = 0;

	sscanf(uwidg->m_value.c_str(), "%f %f %f %f", &A, &B, &C, &D);

	Plane* tceq = g_sel1b->m_sides[g_dragS].m_tceq;
	tceq[0].m_normal.x = A;
	tceq[0].m_normal.y = B;
	tceq[0].m_normal.z = C;
	tceq[0].m_d = D;

	A = 0;
	B = 0;
	C = 0;
	D = 0;

	sscanf(vwidg->m_value.c_str(), "%f %f %f %f", &A, &B, &C, &D);

	tceq[1].m_normal.x = A;
	tceq[1].m_normal.y = B;
	tceq[1].m_normal.z = C;
	tceq[1].m_d = D;

	g_sel1b->m_sides[g_dragS].remaptex();
}

void Change_SelComp()
{
	//RedoBSideGUI();
}

int GetComponent()
{
	int c = g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("select component", WIDGET_DROPDOWNSELECTOR)->m_selected;

	if(c != 0 && c != 1)
		c = 0;

	return c;
}

void Click_ScaleTex()
{
	if(g_sel1b == NULL)
		return;

	if(g_dragS < 0)
		return;

	int c = GetComponent();

	float scale = 1.0f/StrToFloat(g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("texture scale", WIDGET_EDITBOX)->m_value.c_str());

	Brush* b = g_sel1b;
	BrushSide* s = &b->m_sides[g_dragS];

	float oldcomp = s->m_centroid.x*s->m_tceq[c].m_normal.x + s->m_centroid.y*s->m_tceq[c].m_normal.y + s->m_centroid.z*s->m_tceq[c].m_normal.z + s->m_tceq[c].m_d;
	s->m_tceq[c].m_normal = s->m_tceq[c].m_normal * scale;

	//Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];
	float newcomp = s->m_centroid.x*s->m_tceq[c].m_normal.x + s->m_centroid.y*s->m_tceq[c].m_normal.y + s->m_centroid.z*s->m_tceq[c].m_normal.z + s->m_tceq[c].m_d;
	float changecomp = newcomp - oldcomp;
	s->m_tceq[c].m_d -= changecomp;

	s->remaptex();

	RedoBSideGUI();
}

void Click_ShiftTex()
{
	if(g_sel1b == NULL)
		return;

	if(g_dragS < 0)
		return;

	int c = GetComponent();

	float shift = StrToFloat(g_GUI.getview("brush side edit")->getwidget("left panel", WIDGET_FRAME)->getsubwidg("texture shift", WIDGET_EDITBOX)->m_value.c_str());

	Brush* b = g_sel1b;
	BrushSide* s = &b->m_sides[g_dragS];

	s->m_tceq[c].m_d += shift;

	s->remaptex();

	RedoBSideGUI();
}

void Change_ShowSky()
{
	Widget* showskchbox = g_GUI.getview("editor")->getwidget("top panel", WIDGET_FRAME)->getsubwidg("showsky", WIDGET_CHECKBOX);

	g_showsky = (bool)showskchbox->m_selected;
}

void Click_ProjPersp()
{
	g_projtype = PROJ_PERSP;
}

void Click_ProjOrtho()
{
	g_projtype = PROJ_ORTHO;
}

void Click_Explode()
{
	g_edtool = EDTOOL_EXPLOSION;
}

void Click_SetDoor()
{
	if(g_selB.size() <= 0)
	{
		//MessageBox(g_hWnd, "NULL 1b", "asdasd", NULL);
		return;
	}

	LinkPrevUndo();

	Brush* b = *g_selB.begin();
	EdDoor* door = b->m_door;

	if(!b->m_door)
	{
		b->m_door = new EdDoor();
		door = b->m_door;

		door->axis = Vec3f(0,200,0);
		door->point = b->m_sides[2].m_centroid;
		door->startopen = false;
	}

	float opendeg = 90;

	View* dooredview = g_GUI.getview("door edit");
	Frame* leftpanel = (Frame*)dooredview->getwidget("left panel", WIDGET_FRAME);
	EditBox* opendegedit = (EditBox*)leftpanel->getsubwidg("opendeg", WIDGET_EDITBOX);
	opendeg = StrToFloat(opendegedit->m_value.c_str());

	door->opendeg = opendeg;

	b->getsides(&door->m_nsides, &door->m_sides);

#if 0
	g_log<<"set ed door"<<endl;
	for(int i=0; i<door->m_nsides; i++)
	{
		g_log<<"side "<<i<<endl;
		Plane* p = &door->m_sides[i].m_plane;

		g_log<<"plane = "<<p->m_normal.x<<","<<p->m_normal.y<<","<<p->m_normal.z<<",d="<<p->m_d<<endl;
	}
#endif
}

void Click_UnmakeDoor()
{
	if(g_selB.size() <= 0)
	{
		//MessageBox(g_hWnd, "NULL 1b", "asdasd", NULL);
		return;
	}

	Brush* b = *g_selB.begin();

	if(b->m_door)
	{
		LinkPrevUndo();
		delete [] b->m_door;
		b->m_door = NULL;
	}
}

void Click_OpenCloseDoor()
{
	if(g_selB.size() <= 0)
		return;

	Brush* b = *g_selB.begin();
	EdDoor* door = b->m_door;

	if(!b->m_door)
		return;

	LinkPrevUndo();

	Brush transformed;
	transformed.setsides(door->m_nsides, door->m_sides);
	transformed.collapse();
	transformed.remaptex();

	door->startopen = !door->startopen;

	if(door->startopen)
	{
		//transform

		Vec3f centroid = door->point;
		Vec3f axis = Normalize(door->axis);
		float radians = DEGTORAD(door->opendeg);

		//char msg[128];
		//sprintf(msg, "point(%f,%f,%f) axis(%f,%f,%f)

		list<float> oldus;
		list<float> oldvs;

		for(int j=0; j<transformed.m_nsides; j++)
		{
			BrushSide* s = &transformed.m_sides[j];
			RotatePlane(s->m_plane, centroid, radians, axis);

			Vec3f sharedv = transformed.m_sharedv[ s->m_vindices[0] ];
			float u = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float v = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			oldus.push_back(u);
			oldvs.push_back(v);
			s->m_tceq[0].m_normal = Rotate(s->m_tceq[0].m_normal, radians, axis.x, axis.y, axis.z);
			s->m_tceq[1].m_normal = Rotate(s->m_tceq[1].m_normal, radians, axis.x, axis.y, axis.z);
		}

		//transformed.collapse();

		auto oldu = oldus.begin();
		auto oldv = oldvs.begin();

		for(int j=0; j<transformed.m_nsides; j++, oldu++, oldv++)
		{
			BrushSide* s = &transformed.m_sides[j];

			Vec3f newsharedv = transformed.m_sharedv[ s->m_vindices[0] ];

			float newu = newsharedv.x*s->m_tceq[0].m_normal.x + newsharedv.y*s->m_tceq[0].m_normal.y + newsharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float newv = newsharedv.x*s->m_tceq[1].m_normal.x + newsharedv.y*s->m_tceq[1].m_normal.y + newsharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			float changeu = newu - *oldu;
			float changev = newv - *oldv;
			s->m_tceq[0].m_d -= changeu;
			s->m_tceq[1].m_d -= changev;
		}

		//transformed.remaptex();
	}

	b->setsides(transformed.m_nsides, transformed.m_sides);
	b->collapse();
	b->remaptex();
}

void FillGUI()
{
	DrawSceneDepthFunc = &DrawSceneDepth;
	DrawSceneFunc = &DrawScene;

#if FINISHED_KEYCODES
	AssignAnyKey(&AnyKeyDown, &AnyKeyUp);
	AssignKey(VK_ESCAPE, &Escape, NULL);
	AssignKey(VK_DELETE, &Delete, NULL);
	AssignKey('C', Down_C, NULL);
	AssignKey('V', Down_V, NULL);
	//AssignMouseWheel(&MouseWheel);
	//AssignMouseMove(&MouseMove);
	AssignLButton(&MouseLeftButtonDown, &MouseLeftButtonUp);
	//AssignRButton(NULL, &MouseRightButtonUp);
#endif

	BitmapFont* f = &g_font[MAINFONT8];

	View* logoview = AddView("logo");
	logoview->widget.push_back(new Image(NULL, "logo", "gui/dmd.jpg", Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_MIN, MARGIN_FUNC_RATIO, 1.0), ALIGNMENT_CENTER, 1,1,1,0));

	View* loadingview = AddView("loading");
	loadingview->widget.push_back(new Text(NULL, "status", "Loading...", MAINFONT8, Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.4f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f)));

	View* edview = AddView("editor");

	edview->widget.push_back(new Frame(NULL, "top panel", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT)));
	edview->widget.push_back(new Frame(NULL, "left panel", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1)));

	Widget* toppanel = edview->getwidget("top panel", WIDGET_FRAME);
	Widget* leftpanel = edview->getwidget("left panel", WIDGET_FRAME);

	toppanel->m_subwidg.push_back(new Image(toppanel, "top panel bg", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1), ALIGNMENT_LESSERSIDE));
	leftpanel->m_subwidg.push_back(new Image(leftpanel, "left panel bg", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1), ALIGNMENT_LESSERSIDE));

	int i=0;
	toppanel->m_subwidg.push_back(new Button(toppanel, "load", "gui/load.png", "", "Load",											MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_LoadEdMap, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "save", "gui/save.png", "", "Save",											MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_SaveEdMap, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "qsave", "gui/qsave.png", "", "Quick save",									MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_QSaveEdMap, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "build", "gui/build.png", "", "Compile",										MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_CompileMap, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "run", "gui/run.png", "", "Compile and run",									MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_CompileRunMap, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "undo", "gui/undo.png", "", "Undo",											MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Undo, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "redo", "gui/redo.png", "", "Redo",											MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Redo, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "newbrush", "gui/newbrush.png", "", "New brush",								MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), &Click_NewBrush, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "cutbrush", "gui/cutbrush.png", "", "Cut brush",								MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_CutBrush, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "newent", "gui/newent.png", "", "New entity",								MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), NULL, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "selent", "gui/selent.png", "", "Select entities only",						MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), NULL, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "selbrush", "gui/selbrush.png", "", "Select brushes only",					MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), NULL, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "selentbrush", "gui/selentbrush.png", "", "Select entities and brushes",		MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), NULL, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotxccw", "gui/rotxccw.png", "", "Rotate counter-clockwise on x axis",		MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotXCCW, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotxcw", "gui/rotxcw.png", "", "Rotate clockwise on x axis",				MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotXCW, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotyccw", "gui/rotyccw.png", "", "Rotate counter-clockwise on y axis",		MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotYCCW, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotycw", "gui/rotycw.png", "", "Rotate clockwise on y axis",				MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotYCW, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotzccw", "gui/rotzccw.png", "", "Rotate counter-clockwise on z axis",		MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotZCCW, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotzcw", "gui/rotzcw.png", "", "Rotate clockwise on z axis",				MAINFONT8, Margin(0+32*i), Margin(0), Margin(32+32*i++), Margin(32), Click_RotZCW, NULL, NULL));
	i = 0;
	toppanel->m_subwidg.push_back(new EditBox(toppanel, "rotdeg", "15",															MAINFONT8, Margin(0+32*i++), Margin(32), Margin(32+32*i++), Margin(32+16), false, 6, &Change_RotDeg, 0));
	toppanel->m_subwidg.push_back(new Text(toppanel, "rotdegtext", "degrees",													MAINFONT8, Margin(0+32*(i-2)), Margin(32+16)));
	toppanel->m_subwidg.push_back(new EditBox(toppanel, "zoom", "1",															MAINFONT8, Margin(0+32*i++), Margin(32), Margin(32+32*i++), Margin(32+16), false, 6, &Change_Zoom, 0));
	toppanel->m_subwidg.push_back(new Text(toppanel, "zoomtext", "zoom",														MAINFONT8, Margin(0+32*(i-2)), Margin(32+16)));
	toppanel->m_subwidg.push_back(new DropDownS(toppanel, "snapgrid", MAINFONT8, Margin(0+32*i++), Margin(32), Margin(50+32*i++), Margin(32+16), Change_SnapGrid));
	Widget* snapgs = toppanel->getsubwidg("snapgrid", WIDGET_DROPDOWNSELECTOR);
	//snapgs->m_options.push_back("4 m");	//0
	//snapgs->m_options.push_back("2 m");	//1
	//snapgs->m_options.push_back("1 m");	//2
	//snapgs->m_options.push_back("50 cm");	//3
	//snapgs->m_options.push_back("25 cm");	//4
	//snapgs->m_options.push_back("12.5 cm");	//5
	//snapgs->m_options.push_back("6.25 cm");	//6
	//snapgs->m_options.push_back("3.125 cm");	//7
	//snapgs->m_options.push_back("1.5625 cm");	//8
	//snapgs->select(4);

	float cm_scales[] = CM_SCALES;
	string cm_scales_txt[] = CM_SCALES_TXT;

	for(int j=0; j<sizeof(cm_scales)/sizeof(float); j++)
	{
		snapgs->m_options.push_back(cm_scales_txt[j].c_str());
	}

	snapgs->select(6);

	//toppanel->m_subwidg.push_back(new EditBox(toppanel, "snapgrid", "25",														MAINFONT8, Margin(0+32*i++), Margin(32), Margin(32+32*i++), Margin(32+16), false, 6, &Change_SnapGrid, 0));
	toppanel->m_subwidg.push_back(new Text(toppanel, "snapgrid", "snap grid",													MAINFONT8, Margin(0+32*(i-2)), Margin(32+16)));
	toppanel->m_subwidg.push_back(new EditBox(toppanel, "maxelev", "10000",														MAINFONT8, Margin(0+32*i++), Margin(32), Margin(32+32*i++), Margin(32+16), false, 6, &Change_MaxElev, 0));
	toppanel->m_subwidg.push_back(new Text(toppanel, "maxelev", "max elev.",													MAINFONT8, Margin(0+32*(i-2)), Margin(32+16)));
	toppanel->m_subwidg.push_back(new CheckBox(toppanel, "showsky", "show sky",													MAINFONT8, Margin(0+32*i++), Margin(32), Margin(32+32*i++), Margin(32+16), 0, 1.0f, 1.0f, 1.0f, 1.0f, &Change_ShowSky));

	toppanel->m_subwidg.push_back(new Button(toppanel, "rotzcw", "gui/projpersp.png", "", "Perspective projection",				MAINFONT8, Margin(0+32*i), Margin(32), Margin(32+32*i++), Margin(32*2), Click_ProjPersp, NULL, NULL));
	toppanel->m_subwidg.push_back(new Button(toppanel, "rotzcw", "gui/projortho.png", "", "Orthographic projection",			MAINFONT8, Margin(0+32*i), Margin(32), Margin(32+32*i++), Margin(32*2), Click_ProjOrtho, NULL, NULL));
	
	toppanel->m_subwidg.push_back(new Button(toppanel, "explosion", "gui/explosion.png", "", "Explode crater",					MAINFONT8, Margin(0+32*i), Margin(32), Margin(32+32*i++), Margin(32*2), Click_Explode, NULL, NULL));

	//toppanel->m_subwidg.push_back(new Text(toppanel, "fps", "fps: 0", MAINFONT8, Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 10), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 70), true));

	edview->widget.push_back(new Frame(NULL, "viewports frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));
	Widget* viewportsframe = edview->getwidget("viewports frame", WIDGET_FRAME);

	viewportsframe->m_subwidg.push_back(new ViewportW(viewportsframe, "bottom left viewport", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f), &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, NULL, NULL, 0));
	viewportsframe->m_subwidg.push_back(new ViewportW(viewportsframe, "top left viewport", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, NULL, NULL, 1));
	viewportsframe->m_subwidg.push_back(new ViewportW(viewportsframe, "bottom right viewport", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f), &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, NULL, NULL, 2));
	viewportsframe->m_subwidg.push_back(new ViewportW(viewportsframe, "top right viewport", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), &DrawViewport, &ViewportLDown, &ViewportLUp, &ViewportMousemove, &ViewportRDown, &ViewportRUp, ViewportMousewheel, NULL, NULL, 3));

	g_viewportT[VIEWPORT_FRONT] = ViewportT(Vec3f(0, 0, MAX_DISTANCE/2), Vec3f(0, 1, 0), "Front");
	g_viewportT[VIEWPORT_TOP] = ViewportT(Vec3f(0, MAX_DISTANCE/2, 0), Vec3f(0, 0, -1), "Top");
	g_viewportT[VIEWPORT_LEFT] = ViewportT(Vec3f(MAX_DISTANCE/2, 0, 0), Vec3f(0, 1, 0), "Left");
	//g_viewportT[VIEWPORT_ANGLE45O] = ViewportT(Vec3f(MAX_DISTANCE/3, MAX_DISTANCE/3, MAX_DISTANCE/3), Vec3f(0, 1, 0), "Angle");
	g_viewportT[VIEWPORT_ANGLE45O] = ViewportT(Vec3f(1000.0f/3, 1000.0f/3, 1000.0f/3), Vec3f(0, 1, 0), "Angle");
	g_camera.position(1000.0f/3, 1000.0f/3, 1000.0f/3, 0, 0, 0, 0, 1, 0);

	g_viewport[0] = Viewport(VIEWPORT_FRONT);
	g_viewport[1] = Viewport(VIEWPORT_TOP);
	g_viewport[2] = Viewport(VIEWPORT_LEFT);
	g_viewport[3] = Viewport(VIEWPORT_ANGLE45O);

	viewportsframe->m_subwidg.push_back(new Frame(viewportsframe, "viewports right half frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));
	viewportsframe->m_subwidg.push_back(new Frame(viewportsframe, "viewports bottom half frame", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 0.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 0.5f), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1.0f), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1.0f)));

	Widget* viewportsrighthalf = viewportsframe->getsubwidg("viewports right half frame", WIDGET_FRAME);
	Widget* viewportsbottomhalf = viewportsframe->getsubwidg("viewports bottom half frame", WIDGET_FRAME);

	viewportsrighthalf->m_subwidg.push_back(new Image(viewportsrighthalf, "h divider", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1), ALIGNMENT_LESSERSIDE));
	viewportsbottomhalf->m_subwidg.push_back(new Image(viewportsbottomhalf, "v divider", "gui/filled.jpg", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_RATIO, 1), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 1), ALIGNMENT_LESSERSIDE));

	View* brusheditview = AddView("brush edit");
	brusheditview->widget.push_back(new Frame(NULL, "left panel", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1)));

	leftpanel = brusheditview->getwidget("left panel", WIDGET_FRAME);
	i=1;
	//leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "texture path", "no texture", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 0), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), false, 64, NULL, -1));
	//i++;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "choose texture", "gui/transp.png", "Choose Texture", "Choose texture", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*(i+1)), Click_BChooseTex, NULL, NULL));
	int j=0;
	i++;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "fit to face", "gui/fittoface.png", "", "Fit to face",	MAINFONT8, Margin(0+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(32+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i+32), Click_FitToFace, NULL, NULL));
	j++;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "door view", "gui/door.png", "", "Door view",	MAINFONT8, Margin(0+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(32+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i+32), Click_DoorView, NULL, NULL));
	i+=2;

	View* brushsideeditview = AddView("brush side edit");
	brushsideeditview->widget.push_back(new Frame(NULL, "left panel", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1)));

	leftpanel = brushsideeditview->getwidget("left panel", WIDGET_FRAME);
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "rotate texture", "gui/transp.png", "Rotate Texture", "Rotate texture", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), Click_RotateTex, NULL, NULL));
	i+=2;
	leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "u equation", "A B C D", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), false, 256, Change_TexEq, 0));
	i++;
	leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "v equation", "A B C D", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), false, 256, Change_TexEq, 1));
	i++;
	leftpanel->m_subwidg.push_back(new DropDownS(leftpanel, "select component", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), Change_SelComp));
	Widget* selcompwidg = leftpanel->getsubwidg("select component", WIDGET_DROPDOWNSELECTOR);
	selcompwidg->m_options.push_back("u component");
	selcompwidg->m_options.push_back("v component");
	i++;
	leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "texture scale", "1", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH/2), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), false, 10, NULL, 0));
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "texture scale", "gui/transp.png", "Scale", "Scale texture component", MAINFONT8, Margin(LEFT_PANEL_WIDTH/2), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), Click_ScaleTex, NULL, NULL));
	i++;
	leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "texture shift", "0.05", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH/2), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), false, 10, NULL, 0));
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "texture shift", "gui/transp.png", "Shift", "Shift texture component", MAINFONT8, Margin(LEFT_PANEL_WIDTH/2), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, 32+f->gheight*(i+1)), Click_ShiftTex, NULL, NULL));

	View* dooreditview = AddView("door edit");
	dooreditview->widget.push_back(new Frame(NULL, "left panel", Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, TOP_PANEL_HEIGHT), Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_RATIO, 1)));

	leftpanel = dooreditview->getwidget("left panel", WIDGET_FRAME);
	i=1;
	j=0;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "set door", "gui/yesdoor.png", "", "Make door and set properties",	MAINFONT8, Margin(0+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(32+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i+32), Click_SetDoor, NULL, NULL));
	j++;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "unmake door", "gui/nodoor.png", "", "Unmake door",	MAINFONT8, Margin(0+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(32+32*j), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i+32), Click_UnmakeDoor, NULL, NULL));
	j=0;
	i+=3;
	leftpanel->m_subwidg.push_back(new Button(leftpanel, "open/close", "gui/transp.png", "Open / Close", "Open / Close door", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(LEFT_PANEL_WIDTH), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*(i+1)), Click_OpenCloseDoor, NULL, NULL));
	i++;
	leftpanel->m_subwidg.push_back(new Text(leftpanel, "opendeg label", "Open degrees:", MAINFONT8, Margin(0), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i)));
	leftpanel->m_subwidg.push_back(new EditBox(leftpanel, "opendeg", "90", MAINFONT8, Margin(MARGIN_SOURCE_WIDTH, MARGIN_FUNC_PIXELS, 70), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*i), Margin(LEFT_PANEL_WIDTH-10), Margin(MARGIN_SOURCE_HEIGHT, MARGIN_FUNC_PIXELS, f->gheight*(i+1)), false, 10, NULL, 0));


	OpenSoleView("loading");
	//OpenAnotherView("brush edit view");
}


