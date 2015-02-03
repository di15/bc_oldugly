

#include "savemap.h"
#include "compilemap.h"
#include "edmap.h"
#include "../texture.h"
#include "save.h"
#include "saveedm.h"
#include "../sim/map.h"
#include "../utils.h"

// chooses the brush texture from one of its sides
// that defines the attributes for the whole brush.
void BrushAttribTex(Brush* brush)
{
	int finaltex = 0;

	for(int sideidx=0; sideidx<brush->m_nsides; sideidx++)
	{
		BrushSide* side = &brush->m_sides[sideidx];

		int sidetex = side->m_diffusem;

		if(g_texture[sidetex].transp)
			finaltex = sidetex;

		if(g_texture[sidetex].grate)
			finaltex = sidetex;

		if(g_texture[sidetex].breakable)
			finaltex = sidetex;

		if(g_texture[sidetex].passthru)
			finaltex = sidetex;

		if(g_texture[sidetex].fabric)
			finaltex = sidetex;
	}

	brush->m_texture = finaltex;
}

// Compile a list of textures used by map brushes
// and save that table to file. Also, a list of
// texture references is set (texrefs) that
// indexes into the written texture table based
// on the diffuse texture index (which indexes into
// g_texture, the global texture array).
void SaveTexs(FILE* fp, int* texrefs, list<Brush>& brushes)
{
	for(int i=0; i<TEXTURES; i++)
		texrefs[i] = -1;

	// the compiled index of textures used in the map
	list<TexRef> compilation;

	for(auto b=brushes.begin(); b!=brushes.end(); b++)
	{
		//List of sides we will check the textures of.
		//We make a list because we include not only
		//sides of the brush, but the door closed-state model
		//sides too, which might theoretically have different
		//textures if the door model/whatever wasn't updated.
		list<BrushSide*> sides;

		for(int i=0; i<b->m_nsides; i++)
			sides.push_back(&b->m_sides[i]);

		if(b->m_door)
		{
			EdDoor* door = b->m_door;

			for(int i=0; i<door->m_nsides; i++)
				sides.push_back(&door->m_sides[i]);
		}

		for(auto si=sides.begin(); si!=sides.end(); si++)
		{
			BrushSide* s = *si;

			bool found = false;
			for(auto j=compilation.begin(); j!=compilation.end(); j++)
			{
				if(j->diffindex == s->m_diffusem)
				{
					found = true;
					break;
				}
			}

			if(!found)
			{
				Texture* t = &g_texture[s->m_diffusem];
				TexRef tr;
				tr.filepath = t->filepath;
				tr.texname = t->texname;
				tr.diffindex = s->m_diffusem;
				compilation.push_back(tr);
			}
		}

		bool found = false;
		for(auto j=compilation.begin(); j!=compilation.end(); j++)
		{
			if(j->diffindex == b->m_texture)
			{
				found = true;
				break;
			}
		}

		if(!found)
		{
			Texture* t = &g_texture[b->m_texture];
			TexRef tr;
			tr.filepath = t->filepath;
			tr.texname = t->texname;
			tr.diffindex = b->m_texture;
			compilation.push_back(tr);
		}
	}

	//Write the texture table to file.
	int nrefs = compilation.size();
	fwrite(&nrefs, sizeof(int), 1, fp);

#if 0
	g_log<<"writing "<<nrefs<<" tex refs"<<endl;
	g_log.flush();
#endif

	int j=0;
	for(auto i=compilation.begin(); i!=compilation.end(); i++, j++)
	{
		texrefs[ i->diffindex ] = j;
		int strl = i->filepath.length()+1;
		fwrite(&strl, sizeof(int), 1, fp);

#if 0
		g_log<<"writing "<<strl<<"-long tex ref"<<endl;
		g_log.flush();
#endif

		fwrite(i->filepath.c_str(), sizeof(char), strl, fp);
	}
}

void ReadTexs(FILE* fp, TexRef** texrefs)
{
	int nrefs;
	fread(&nrefs, sizeof(int), 1, fp);

#if 0
	g_log<<"reading "<<nrefs<<" tex refs"<<endl;
	g_log.flush();
#endif

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

#if 0
		g_log<<"reading "<<strl<<"-long tex ref"<<endl;
		g_log.flush();
#endif

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

void SaveBrushes(FILE* fp, int* texrefs, list<Brush>& brushes)
{
	int nbrush = brushes.size();
	fwrite(&nbrush, sizeof(int), 1, fp);

#if 0
	g_log<<"writing "<<nbrush<<" brushes at "<<ftell(fp)<<endl;
	g_log.flush();

	int i=0;
#endif

	for(auto b=brushes.begin(); b!=brushes.end(); b++)
	{
		SaveBrush(fp, texrefs, &*b);

#if 0
		g_log<<"wrote brush "<<i<<" end at "<<ftell(fp)<<endl;
		i++;
#endif
	}
}

// We don't draw the sky textured brushes.
// They are replaced by a sky box.
// But we might want to know which
// ones are sky brushes for some reason.
// We need to save a table of transparent brushes
// because we need to alpha-sort them for proper drawing.
// Now that we saved the transparent brushes,
// we need to store the opaque ones.
// This is because we draw the opaque ones first,
// and then the transparent ones in the right order.
void SaveBrushRefs(FILE* fp, list<Brush>& brushes)
{
	list<int> opaqbrushrefs;
	list<int> transpbrushrefs;
	list<int> skybrushrefs;

	int brushidx = 0;
	for(auto brushitr=brushes.begin(); brushitr!=brushes.end(); brushitr++, brushidx++)
	{
		bool hassky = false;
		bool hastransp = false;

		//Check if any of the sides have a transparent texture
		for(int sideidx = 0; sideidx < brushitr->m_nsides; sideidx++)
		{
			BrushSide* pside = &brushitr->m_sides[sideidx];
			unsigned int sidetex = pside->m_diffusem;

			//If the brush has a sky texture attribute,
			//we want to write it.
			if(g_texture[sidetex].sky)
			{
				skybrushrefs.push_back(brushidx);
				hassky = true;
				break;
			}

			//If side has transparent texture,
			//the brush index will be added to the table.
			if(g_texture[sidetex].transp)
			{
				transpbrushrefs.push_back(brushidx);
				hastransp = true;
				break;
			}
		}

		if(!hassky && !hastransp)
		{
			opaqbrushrefs.push_back(brushidx);
		}
	}

	//Write the brush references tables.

	// Opaque brushes
	int nbrushrefs = opaqbrushrefs.size();
	fwrite(&nbrushrefs, sizeof(int), 1, fp);

	for(auto refitr = opaqbrushrefs.begin(); refitr != opaqbrushrefs.end(); refitr++)
	{
		fwrite(&*refitr, sizeof(int), 1, fp);
	}

	// Transparent brushes
	nbrushrefs = transpbrushrefs.size();
	fwrite(&nbrushrefs, sizeof(int), 1, fp);

	for(auto refitr = transpbrushrefs.begin(); refitr != transpbrushrefs.end(); refitr++)
	{
		fwrite(&*refitr, sizeof(int), 1, fp);
	}

	// Sky brushes
	nbrushrefs = skybrushrefs.size();
	fwrite(&nbrushrefs, sizeof(int), 1, fp);

	for(auto refitr = skybrushrefs.begin(); refitr != skybrushrefs.end(); refitr++)
	{
		fwrite(&*refitr, sizeof(int), 1, fp);
	}
}

//Read the brush references tables
void ReadBrushRefs(FILE* fp, Map* map)
{
	// Opaque brushes
	int nbrushrefs = 0;
	fread(&nbrushrefs, sizeof(int), 1, fp);
	for(int refindex = 0; refindex < nbrushrefs; refindex++)
	{
		int ref;
		fread(&ref, sizeof(int), 1, fp);
		map->m_opaquebrush.push_back(ref);
	}

	// Transparent brushes
	nbrushrefs = 0;
	fread(&nbrushrefs, sizeof(int), 1, fp);
	for(int refindex = 0; refindex < nbrushrefs; refindex++)
	{
		int ref;
		fread(&ref, sizeof(int), 1, fp);
		map->m_transpbrush.push_back(ref);
	}

	// Sky brushes
	nbrushrefs = 0;
	fread(&nbrushrefs, sizeof(int), 1, fp);
	for(int refindex = 0; refindex < nbrushrefs; refindex++)
	{
		int ref;
		fread(&ref, sizeof(int), 1, fp);
		map->m_skybrush.push_back(ref);
	}
}

void SaveMap(const char* fullpath, list<Brush>& brushes)
{
	FILE* fp = fopen(fullpath, "wb");

	char tag[] = TAG_MAP;
	fwrite(tag, sizeof(char), 5, fp);

	float version = MAP_VERSION;
	fwrite(&version, sizeof(float), 1, fp);

#if 0
	g_log<<"brushes to write: "<<brushes.size()<<endl;
#endif

	int texrefs[TEXTURES];
	SaveTexs(fp, texrefs, brushes);

#if 0
	g_log<<"write brushes at "<<ftell(fp)<<endl;
	g_log.flush();
#endif

	SaveBrushes(fp, texrefs, brushes);
	SaveBrushRefs(fp, brushes);

	fclose(fp);
}

void ReadBrushes(FILE* fp, TexRef* texrefs, Map* map)
{
	int nbrush;
	fread(&nbrush, sizeof(int), 1, fp);

	map->m_nbrush = nbrush;
	map->m_brush = new Brush[nbrush];

#ifdef LOADMAP_DEBUG
	g_log<<"nbrush = "<<nbrush<<endl;
	g_log.flush();
#endif

	for(int i=0; i<nbrush; i++)
	{
		ReadBrush(fp, texrefs, &map->m_brush[i]);

#ifdef LOADMAP_DEBUG
	g_log<<"added b"<<endl;
	g_log.flush();
#endif

		g_log<<"read brush "<<i<<" end at "<<ftell(fp)<<endl;
	}
}

void ProcessMap(Map* map)
{
	bool firstset = false;

	for(int brushindex = 0; brushindex < map->m_nbrush; brushindex++)
	{
		Brush* b = &map->m_brush[brushindex];

		for(int vertindex = 0; vertindex < b->m_nsharedv; vertindex++)
		{
			Vec3f v = b->m_sharedv[vertindex];

			if(!firstset)
			{
				map->m_max = v;
				map->m_min = v;
				firstset = true;
			}
			else
			{
				if(v.x < map->m_min.x)
					map->m_min.x = v.x;
				if(v.y < map->m_min.y)
					map->m_min.y = v.y;
				if(v.z < map->m_min.z)
					map->m_min.z = v.z;
				
				if(v.x > map->m_max.x)
					map->m_max.x = v.x;
				if(v.y > map->m_max.y)
					map->m_max.y = v.y;
				if(v.z > map->m_max.z)
					map->m_max.z = v.z;
			}
		}
	}
}

bool LoadMap(const char* fullpath, Map* map)
{
	map->destroy();

	FILE* fp = fopen(fullpath, "rb");

	if(!fp)
	{
		g_log<<"Failed to open map "<<fullpath<<endl;
		return false;
	}

	char tag[5];
	fread(tag, sizeof(char), 5, fp);

	char realtag[] = TAG_MAP;
	//if(false)
	if(tag[0] != realtag[0] ||  tag[1] != realtag[1] || tag[2] != realtag[2] || tag[3] != realtag[3] || tag[4] != realtag[4])
	{
		fclose(fp);
		//MessageBox(g_hWnd, "Not a map file (invalid header tag).", "Error", NULL);
		ErrorMessage("Error", "Not a map file (invalid header tag).");
		return false;
	}

	float version;
	fread(&version, sizeof(float), 1, fp);

	if(version != MAP_VERSION)
	{
		fclose(fp);
		char msg[128];
		sprintf(msg, "Map's version (%f) doesn't match %f.", version, MAP_VERSION);
		//MessageBox(g_hWnd, msg, "Error", NULL);
		ErrorMessage("Error", msg);
		return false;
	}

#ifdef LOADMAP_DEBUG
	g_log<<"load map 1"<<endl;
	g_log.flush();
#endif

	TexRef* texrefs = NULL;

	ReadTexs(fp, &texrefs);

	g_log<<"read brushes at "<<ftell(fp)<<endl;

	ReadBrushes(fp, texrefs, map);
	ReadBrushRefs(fp, map);

	g_log<<"loaded "<<map->m_nbrush<<" brushes "<<endl;
	g_log.flush();

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

	ProcessMap(&g_map);

	return true;
}
