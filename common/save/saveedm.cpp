

#include "../sim/map.h"
#include "save.h"
#include "../platform.h"
#include "../math/polygon.h"
#include "../texture.h"
#include "../draw/vertexarray.h"
#include "../utils.h"
#include "saveedm.h"
#include "../save/edmap.h"
#include "savemap.h"

void SaveEdBrushSide(FILE* fp, BrushSide* s, int* texrefs)
{
	fwrite(&s->m_plane, sizeof(Plane), 1, fp);

	SaveVertexArray(fp, &s->m_drawva);

	fwrite(&texrefs[ s->m_diffusem ], sizeof(int), 1, fp);
	fwrite(&s->m_ntris, sizeof(int), 1, fp);
	fwrite(s->m_tris, sizeof(Triangle2), s->m_ntris, fp);
	fwrite(s->m_tceq, sizeof(Plane), 2, fp);

	SavePolygon(fp, &s->m_outline);

	fwrite(s->m_vindices, sizeof(int), s->m_outline.m_edv.size(), fp);
	fwrite(&s->m_centroid, sizeof(Vec3f), 1, fp);
}

void SaveEdBrushSides(FILE* fp, Brush* b, int* texrefs)
{
	int nsides = b->m_nsides;
	fwrite(&nsides, sizeof(int), 1, fp);

	for(int i=0; i<nsides; i++)
	{
		/*
	Plane m_plane;
	VertexArray m_drawva;
	unsigned int m_diffusem;
		*/
	/*
	int m_ntris;
	Triangle2* m_tris;
	Plane m_tceq[2];	//tex coord uv equations
	Polyg m_outline;
	int* m_vindices;	//indices into parent brush's shared vertex array
	Vec3f m_centroid;
	*/
		BrushSide* s = &b->m_sides[i];

		SaveEdBrushSide(fp, s, texrefs);
	}
}

void ReadBrushSide(FILE* fp, BrushSide* s, TexRef* texrefs)
{
	fread(&s->m_plane, sizeof(Plane), 1, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"s->m_plane = "<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<","<<s->m_plane.m_d<<endl;
	g_log.flush();
#endif

	ReadVertexArray(fp, &s->m_drawva);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 1"<<endl;
	g_log.flush();
#endif

	int texrefindex;
	fread(&texrefindex, sizeof(int), 1, fp);
	s->m_diffusem = texrefs[ texrefindex ].diffindex;
	s->m_specularm = texrefs[ texrefindex ].specindex;
	s->m_normalm = texrefs[ texrefindex ].normindex;
	fread(&s->m_ntris, sizeof(int), 1, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 2"<<endl;
	g_log.flush();
#endif

	s->m_tris = new Triangle2[ s->m_ntris ];
	fread(s->m_tris, sizeof(Triangle2), s->m_ntris, fp);
	fread(s->m_tceq, sizeof(Plane), 2, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 3"<<endl;
	g_log.flush();
#endif

	ReadPolygon(fp, &s->m_outline);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 4"<<endl;
	g_log.flush();
#endif

	s->m_vindices = new int[ s->m_outline.m_edv.size() ];
	fread(s->m_vindices, sizeof(int), s->m_outline.m_edv.size(), fp);
	fread(&s->m_centroid, sizeof(Vec3f), 1, fp);
}

void ReadBrushSides(FILE* fp, Brush* b, TexRef* texrefs)
{
	int nsides;
	fread(&nsides, sizeof(int), 1, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"nsides = "<<nsides<<endl;
	g_log.flush();
#endif

	if(b->m_sides)
	{
		delete [] b->m_sides;
		b->m_sides = NULL;
		b->m_nsides = 0;
	}

	for(int i=0; i<nsides; i++)
	{
		BrushSide s;

		ReadBrushSide(fp, &s, texrefs);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 5"<<endl;
	g_log.flush();
#endif

		b->add(s);

#ifdef LOADMAP_DEBUG
	g_log<<"load ed brush side 6"<<endl;
	g_log.flush();
#endif
	}
}

Brush* GetBrushNum(int target, EdMap* map)
{
	int cnt = 0;
	for(auto i=map->m_brush.begin(); i!=map->m_brush.end(); i++, cnt++)
	{
		if(cnt == target)
			return &*i;
	}

	return NULL;
}

void SaveEdDoor(FILE* fp, EdDoor* door, int* texrefs)
{
	/*
	Vec3f axis;
	Vec3f point;
	float opendeg;	//open degrees
	bool startopen;
	Brush* brushp;
	Brush closedstate;*/

	fwrite(&door->axis, sizeof(Vec3f), 1, fp);
	fwrite(&door->point, sizeof(Vec3f), 1, fp);
	fwrite(&door->opendeg, sizeof(float), 1, fp);
	fwrite(&door->startopen, sizeof(bool), 1, fp);

	fwrite(&door->m_nsides, sizeof(int), 1, fp);
	for(int i=0; i<door->m_nsides; i++)
		SaveEdBrushSide(fp, &door->m_sides[i], texrefs);

#if 0
	g_log<<"save ed door"<<endl;
	for(int i=0; i<door->m_nsides; i++)
	{
		g_log<<"side "<<i<<endl;
		Plane* p = &door->m_sides[i].m_plane;

		g_log<<"plane = "<<p->m_normal.x<<","<<p->m_normal.y<<","<<p->m_normal.z<<",d="<<p->m_d<<endl;
	}
#endif
}

void ReadEdDoor(FILE* fp, EdDoor* door, TexRef* texrefs)
{
	/*
	Vec3f axis;
	Vec3f point;
	float opendeg;	//open degrees
	bool startopen;
	Brush* brushp;
	Brush closedstate;*/

#if 0
	MessageBox(g_hWnd, "read door", "aasd", NULL);
#endif

	fread(&door->axis, sizeof(Vec3f), 1, fp);
	fread(&door->point, sizeof(Vec3f), 1, fp);
	fread(&door->opendeg, sizeof(float), 1, fp);
	fread(&door->startopen, sizeof(bool), 1, fp);

	fread(&door->m_nsides, sizeof(int), 1, fp);


#if 0
	char msg[128];
	sprintf(msg, "door sides %d", door->m_nsides);
	MessageBox(g_hWnd, msg, "asd", NULL);
#endif

	door->m_sides = new BrushSide[door->m_nsides];
	for(int i=0; i<door->m_nsides; i++)
		ReadBrushSide(fp, &door->m_sides[i], texrefs);

#if 0
	g_log<<"read ed door"<<endl;
	for(int i=0; i<door->m_nsides; i++)
	{
		g_log<<"side "<<i<<endl;
		Plane* p = &door->m_sides[i].m_plane;

		g_log<<"plane = "<<p->m_normal.x<<","<<p->m_normal.y<<","<<p->m_normal.z<<",d="<<p->m_d<<endl;
	}
#endif
}

void SaveBrush(FILE* fp, int* texrefs, Brush* b)
{
	/*
		int m_nsides;
		BrushSide* m_sides;
		int m_nsharedv;
		Vec3f* m_sharedv;	//shared vertices array
	*/

	SaveEdBrushSides(fp, b, texrefs);
	fwrite(&b->m_nsharedv, sizeof(int), 1, fp);
	fwrite(b->m_sharedv, sizeof(Vec3f), b->m_nsharedv, fp);
	fwrite(&texrefs[b->m_texture], sizeof(int), 1, fp);

	bool hasdoor = false;

	if(b->m_door)
		hasdoor = true;

	fwrite(&hasdoor, sizeof(bool), 1, fp);

	if(hasdoor)
		SaveEdDoor(fp, b->m_door, texrefs);
}

void SaveBrushes(FILE* fp, int* texrefs, EdMap* map)
{
	int nbrush = map->m_brush.size();
	fwrite(&nbrush, sizeof(int), 1, fp);

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		SaveBrush(fp, texrefs, &*b);
	}
}

void ReadBrush(FILE* fp, TexRef* texrefs, Brush* b)
{
	ReadBrushSides(fp, b, texrefs);

	if(b->m_sharedv)
	{
		delete [] b->m_sharedv;
		b->m_sharedv = NULL;
		b->m_nsharedv = 0;
	}

	fread(&b->m_nsharedv, sizeof(int), 1, fp);
#ifdef LOADMAP_DEBUG
	g_log<<"b.m_nsharedv = "<<b.m_nsharedv<<endl;
	g_log.flush();
#endif

	b->m_sharedv = new Vec3f[ b->m_nsharedv ];
	fread(b->m_sharedv, sizeof(Vec3f), b->m_nsharedv, fp);

	//b.remaptex();	//comment this out

	int texrefindex;
	fread(&texrefindex, sizeof(int), 1, fp);
	b->m_texture = texrefs[texrefindex].diffindex;

	bool hasdoor = false;
	fread(&hasdoor, sizeof(bool), 1, fp);

	if(hasdoor)
	{
		b->m_door = new EdDoor();
		ReadEdDoor(fp, b->m_door, texrefs);
	}
}

void ReadBrushes(FILE* fp, TexRef* texrefs, EdMap* map)
{
	int nbrush;
	fread(&nbrush, sizeof(int), 1, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"nbrush = "<<nbrush<<endl;
	g_log.flush();
#endif

	for(int i=0; i<nbrush; i++)
	{
		Brush b;
		ReadBrush(fp, texrefs, &b);
		map->m_brush.push_back(b);

#ifdef LOADMAP_DEBUG
	g_log<<"added b"<<endl;
	g_log.flush();
#endif
	}
}

void ReadEdTexs(FILE* fp, TexRef** texrefs)
{
	int nrefs;
	fread(&nrefs, sizeof(int), 1, fp);

#ifdef LOADMAP_DEBUG
	g_log<<"nrefs = "<<nrefs<<endl;
	g_log.flush();
#endif

	(*texrefs) = new TexRef[nrefs];

	for(int i=0; i<nrefs; i++)
	{
		TexRef* tr = &(*texrefs)[i];
		int strl;
		fread(&strl, sizeof(int), 1, fp);

		char* filepath = new char[strl];
		fread(filepath, sizeof(char), strl, fp);
#ifdef LOADMAP_DEBUG
	g_log<<"filepath = "<<filepath<<endl;
	g_log.flush();
#endif
		tr->filepath = filepath;
		delete [] filepath;
		CreateTexture(tr->diffindex, tr->filepath.c_str(), false);
		tr->texname = g_texture[tr->diffindex].texname;

		char basepath[MAX_PATH+1];
		strcpy(basepath, tr->filepath.c_str());
		StripExtension(basepath);

		char specpath[MAX_PATH+1];
		SpecPath(basepath, specpath);

		CreateTexture(tr->specindex, specpath, false);

		char normpath[MAX_PATH+1];
		NormPath(basepath, normpath);

		CreateTexture(tr->normindex, normpath, false);
	}
}

int BrushNum(Brush* b, EdMap* map)
{
	int cnt = 0;
	for(auto i=map->m_brush.begin(); i!=map->m_brush.end(); i++, cnt++)
	{
		if(&*i == b)
			return cnt;
	}

	return -1;
}

void SaveEdMap(const char* fullpath, EdMap* map)
{
	FILE* fp = fopen(fullpath, "wb");

	char tag[] = TAG_EDMAP;
	fwrite(tag, sizeof(char), 5, fp);

	float version = EDMAP_VERSION;
	fwrite(&version, sizeof(float), 1, fp);

	int texrefs[TEXTURES];
	SaveTexs(fp, texrefs, map->m_brush);
	SaveBrushes(fp, texrefs, map);

	fclose(fp);
}

void ScaleAll(float factor)
{
	EdMap* map = &g_edmap;

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		list<float> oldus;
		list<float> oldvs;

		for(int i=0; i<b->m_nsides; i++)
		{
			BrushSide* s = &b->m_sides[i];

			Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
			float u = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float v = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			oldus.push_back(u);
			oldvs.push_back(v);
			s->m_tceq[0].m_normal = s->m_tceq[0].m_normal / factor;
			s->m_tceq[1].m_normal = s->m_tceq[1].m_normal / factor;
			Vec3f pop = PointOnPlane(s->m_plane);
			pop = pop * factor;
			s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, pop);
		}

		b->collapse();

		auto oldu = oldus.begin();
		auto oldv = oldvs.begin();

		for(int i=0; i<b->m_nsides; i++, oldu++, oldv++)
		{
			BrushSide* s = &b->m_sides[i];

			Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];

			float newu = newsharedv.x*s->m_tceq[0].m_normal.x + newsharedv.y*s->m_tceq[0].m_normal.y + newsharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
			float newv = newsharedv.x*s->m_tceq[1].m_normal.x + newsharedv.y*s->m_tceq[1].m_normal.y + newsharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
			float changeu = newu - *oldu;
			float changev = newv - *oldv;
			s->m_tceq[0].m_d -= changeu;
			s->m_tceq[1].m_d -= changev;
		}

		b->remaptex();

		if(b->m_door)
		{
			oldus.clear();
			oldvs.clear();
			//list<float> oldus;
			//list<float> oldvs;

			for(int i=0; i<b->m_door->m_nsides; i++)
			{
				BrushSide* s = &b->m_door->m_sides[i];

				Vec3f sharedv = b->m_sharedv[ s->m_vindices[0] ];
				float u = sharedv.x*s->m_tceq[0].m_normal.x + sharedv.y*s->m_tceq[0].m_normal.y + sharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
				float v = sharedv.x*s->m_tceq[1].m_normal.x + sharedv.y*s->m_tceq[1].m_normal.y + sharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
				oldus.push_back(u);
				oldvs.push_back(v);
				s->m_tceq[0].m_normal = s->m_tceq[0].m_normal / factor;
				s->m_tceq[1].m_normal = s->m_tceq[1].m_normal / factor;
				Vec3f pop = PointOnPlane(s->m_plane);
				pop = pop * factor;
				s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, pop);
			}

			b->collapse();

			auto oldu = oldus.begin();
			auto oldv = oldvs.begin();

			for(int i=0; i<b->m_door->m_nsides; i++, oldu++, oldv++)
			{
				BrushSide* s = &b->m_door->m_sides[i];

				Vec3f newsharedv = b->m_sharedv[ s->m_vindices[0] ];

				float newu = newsharedv.x*s->m_tceq[0].m_normal.x + newsharedv.y*s->m_tceq[0].m_normal.y + newsharedv.z*s->m_tceq[0].m_normal.z + s->m_tceq[0].m_d;
				float newv = newsharedv.x*s->m_tceq[1].m_normal.x + newsharedv.y*s->m_tceq[1].m_normal.y + newsharedv.z*s->m_tceq[1].m_normal.z + s->m_tceq[1].m_d;
				float changeu = newu - *oldu;
				float changev = newv - *oldv;
				s->m_tceq[0].m_d -= changeu;
				s->m_tceq[1].m_d -= changev;
			}
		}
	}

	char msg[128];
	sprintf(msg, "scaled %f", factor);
	//MessageBox(g_hWnd, msg, "asdasd", NULL);
	InfoMessage("asdads", msg);
}

bool LoadEdMap(const char* fullpath, EdMap* map)
{
	FreeEdMap(map);

	FILE* fp = fopen(fullpath, "rb");

	if(!fp)
		return false;

	char tag[5];
	fread(tag, sizeof(char), 5, fp);

	char realtag[] = TAG_EDMAP;
	//if(false)
	if(tag[0] != realtag[0] ||  tag[1] != realtag[1] || tag[2] != realtag[2] || tag[3] != realtag[3] || tag[4] != realtag[4])
	{
		fclose(fp);
		//MessageBox(g_hWnd, "Not a map project file (invalid header tag).", "Error", NULL);
		ErrorMessage("Error", "Not a map project file (invalid header tag).");
		return false;
	}

	float version;
	fread(&version, sizeof(float), 1, fp);

	if(version != EDMAP_VERSION)
	{
		fclose(fp);
		char msg[128];
		sprintf(msg, "Map project's version (%f) doesn't match %f.", version, EDMAP_VERSION);
		//MessageBox(g_hWnd, msg, "Error", NULL);
		ErrorMessage("Error", msg);
		return false;
	}

#ifdef LOADMAP_DEBUG
	g_log<<"load map 1"<<endl;
	g_log.flush();
#endif

	TexRef* texrefs = NULL;

	ReadEdTexs(fp, &texrefs);
	ReadBrushes(fp, texrefs, map);

	if(texrefs)
	{
		delete [] texrefs;
		texrefs = NULL;
	}

#ifdef LOADMAP_DEBUG
	g_log<<"load map 2"<<endl;
	g_log.flush();
#endif

	fclose(fp);

#ifdef LOADMAP_DEBUG
	g_log<<"load map 3"<<endl;
	g_log.flush();
#endif

	//ScaleAll(0.8f);

	return true;
}

void FreeEdMap(EdMap* map)
{
	g_sel1b = NULL;
	g_selB.clear();
	g_dragV = -1;
	g_dragS = -1;
	g_dragD = -1;
	g_dragB = false;

	for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		for(int i=0; i<b->m_nsides; i++)
		{
			BrushSide* s = &b->m_sides[i];

			if(s->m_diffusem != 0)
				FreeTexture(s->m_diffusem);
			if(s->m_specularm != 0)
				FreeTexture(s->m_specularm);
			if(s->m_normalm != 0)
				FreeTexture(s->m_normalm);
		}
	}

	map->m_brush.clear();
}
