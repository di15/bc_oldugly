

#include "compilemap.h"
#include "../utils.h"
#include "savemap.h"

void ToCutSide(CutBrushSide* cuts, BrushSide* eds)
{
	/*
class BrushSide
{
public:
	Plane m_plane;
	VertexArray m_drawva;
	unsigned int m_diffusem;
	unsigned int m_specularm;
	unsigned int m_normalm;

class BrushSide : public BrushSide
{
public:
	int m_ntris;
	Triangle2* m_tris;
	Plane m_tceq[2];	//tex coord uv equations
	Polyg m_outline;
	int* m_vindices;	//indices into parent brush's shared vertex array; only stores unique vertices as defined by polygon outline
	Vec3f m_centroid;
	*/

	/*
class CutBrushSide
{
public:
	unsigned int m_diffusem;
	unsigned int m_specularm;
	unsigned int m_normalm;
	Plane m_tceq[2];
	list<Triangle> m_frag;
	*/
	
	cuts->m_diffusem = eds->m_diffusem;
	cuts->m_specularm = eds->m_specularm;
	cuts->m_normalm = eds->m_normalm;
	cuts->m_tceq[0] = eds->m_tceq[0];
	cuts->m_tceq[1] = eds->m_tceq[1];

	for(int i=0; i<eds->m_ntris; i++)
		cuts->m_frag.push_back(eds->m_tris[i]);
}

void ToCutBrush(CutBrush* cutb, Brush* edb)
{
	for(int i=0; i<edb->m_nsides; i++)
	{
		CutBrushSide cuts;
		ToCutSide(&cuts, &edb->m_sides[i]);
		cutb->m_side.push_back(cuts);
	}
}

bool BrushTouch(Brush* b1, Brush* b2)
{
	for(int i=0; i<b1->m_nsides; i++)
	{
		BrushSide* s = &b1->m_sides[i];
		Plane p = s->m_plane;
		p.m_d += EPSILON;
		bool found = false;

		for(int j=0; j<b2->m_nsharedv; j++)
		{
			Vec3f v = b2->m_sharedv[j];

			if(PointOnOrBehindPlane(v, p))
			{
				found = true;
				break;
			}
		}

		if(!found)
			return false;
	}

	return true;
}

void FragBrush(CutBrush* cutb, Brush* edb)
{
	for(auto i=cutb->m_side.begin(); i!=cutb->m_side.end(); i++)
	{
		for(auto j=i->m_frag.begin(); j!=i->m_frag.end(); j++)
		{

		}
	}
}

static int g_fragerased;

// Remove hidden triangles (triangles that are completely
// covered up by convex hulls)
void RemoveHiddenFrags(CutBrush* cutb, Brush* edb)
{
	for(auto i=cutb->m_side.begin(); i!=cutb->m_side.end(); i++)
	{
		auto j = i->m_frag.begin();

		while(j != i->m_frag.end())
		{
			bool inall = true;

			for(int k=0; k<edb->m_nsides; k++)
			{
				BrushSide* s = &edb->m_sides[k];
				Plane* p = &s->m_plane;

				if(!PointOnOrBehindPlane(j->m_vertex[0], *p))
				{
					inall = false;
					break;
				}
				
				if(!PointOnOrBehindPlane(j->m_vertex[1], *p))
				{
					inall = false;
					break;
				}
				
				if(!PointOnOrBehindPlane(j->m_vertex[2], *p))
				{
					inall = false;
					break;
				}
			}

			if(inall)
			{
				j = i->m_frag.erase( j );
				g_fragerased ++;
				continue;
			}

			j++;
		}
	}
}

static list<CutBrush> cutbs;	//the cut brush sides
static int ntouch = 0;	//the number of touching brushes
static EdMap* cmap;	//the map to compile
static list<Brush> finalbs; //This will hold the final brushes

void CutBrushes()
{
	cutbs.clear();

	for(auto i=cmap->m_brush.begin(); i!=cmap->m_brush.end(); i++)
	{
		CutBrush cutb;
		ToCutBrush(&cutb, &*i);
		cutbs.push_back(cutb);
	}

	ntouch = 0;
	
	auto a = cmap->m_brush.begin();
	auto cuta = cutbs.begin();
	for(; a!=cmap->m_brush.end(); a++, cuta++)
	{
		auto b = cmap->m_brush.begin();
		auto cutb = cutbs.begin();

		while(&*b != &*a)
		{
			b++;
			cutb++;
		}

		b++;
		cutb++;

		for(; b!=cmap->m_brush.end(); b++)
		{
			if(!BrushTouch(&*a, &*b) && !BrushTouch(&*b, &*a))
				continue;

			ntouch++;
			
			//FragBrush(&*cuta, &*b);	
			//FragBrush(&*cutb, &*a);
		}
	}

	g_log<<"num touches: "<<ntouch<<endl;
	
	// For each brush, see which other brush
	//it touches, and remove any hidden fragments
	//covered up by the other brush.
	g_fragerased = 0;
	a = cmap->m_brush.begin();
	cuta = cutbs.begin();
	for(; a!=cmap->m_brush.end(); a++, cuta++)
	{
		auto b = cmap->m_brush.begin();
		auto cutb = cutbs.begin();

		while(&*b != &*a)
		{
			b++;
			cutb++;
		}

		b++;
		cutb++;

		for(; b!=cmap->m_brush.end(); b++)
		{
			if(!BrushTouch(&*a, &*b) && !BrushTouch(&*b, &*a))
				continue;
			
			RemoveHiddenFrags(&*cuta, &*b);	
			RemoveHiddenFrags(&*cutb, &*a);
		}
	}
}

void MakeFinalBrushes()
{
	finalbs.clear();

	//Copy the map brushes to final brushes
	//and substitute its drawable vertex array
	//of triangles to the cut-down list.
	auto brushitr = cmap->m_brush.begin();
	auto cutbrushitr = cutbs.begin();
	for(; brushitr!=cmap->m_brush.end(); brushitr++, cutbrushitr++)
	{
		Brush finalb = *brushitr;
		auto cutsideitr = cutbrushitr->m_side.begin();
		
		for(int sideindex = 0; sideindex < finalb.m_nsides; cutsideitr++, sideindex++)
		{
			BrushSide* s = &finalb.m_sides[sideindex];
			CutBrushSide* cuts = &*cutsideitr;
			s->vafromcut(cuts);
		}

		finalbs.push_back(finalb);
	}
}

void CleanUpMapCompile()
{
	cutbs.clear();
	finalbs.clear();
}

void CompileMap(const char* full, EdMap* map)
{
	cmap = map;

	g_log<<"Compiling map "<<full<<endl;
	g_log<<"num brushes: "<<map->m_brush.size()<<endl;

	CutBrushes();
	
	g_log<<"frags removed: "<<g_fragerased<<endl;

	MakeFinalBrushes();

	SaveMap(full, finalbs);

	CleanUpMapCompile();
}