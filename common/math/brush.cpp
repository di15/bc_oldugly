
/*

Convex hulls or "brushes" are how first person shooters that derive from 
Quake/Doom define geometry for use in collision detection and response. 
Using a set of planes, 6 for all sides of a cube, we can define any convex 
shape. This simplifies collision detection against axis-aligned bounding boxes 
and spheres because all we have to do is check each plane/side of the brush 
against the farthest and nearest point of the AA box or sphere, and check if 
both points are on both sides of the plane or inside. If there's a point on 
the inside side of each plane there's an overlap with the sphere or AA box 
and its not hard to get the exact distance we have to move to just be touching
the surface using the dot product.

To be drawn, brushes must be broken down into triangles. To do this I loop 
through each plane/side of the brush "i". And for each side "i" I get another, 
different side "j". I get the line intersection between them. This is the code 
I used.

http://devmaster.net/forums/topic/8676-2-plane-intersection/

Then we need another side (each is a different side) "k" that I then get the 
point intersection of the line with, and another side "l" that I get another point 
intersection with. I use a for-loop to go through all the sides and for "l" I 
started counting at "k+1" so we don't get any repeats (this becomes important 
later when building a polygon for the brush side). The two point intersections 
form a side edge for a polygon for the brush side. I store it in an array of STL 
lists of lines. Each brush side has a list of lines. I store the line for side "i" 
because that is the brush side that the side edge belongs to and is along.

Then I loop the side edges for each brush side, making a "polygon" - basically an 
outline, with a point for each vertex. I use an epsilon value to check if two points 
are within a certain distance, and use the side edge's other vertex as the next point 
to check for proximity, starting over from the first side edge and making sure to 
exclude checking the last connecting edge.

Then I check the polygon to be clockwise order (because that is how I cull my 
polygons) by checking the normal of a triangle formed by the first three vertices 
of the polygon and checking if its closer to the plane normal or if the opposite 
normal is closer. If the opposite is closer I reverse the list of vertices.

Oh before I make the polygon I discard any side edges with at least one point that 
is not inside or on any one plane of the brush. This is necessary to cull away 
bounding planes that are outside the brush, resulting from moving the other planes. 
Later I remove these planes that have less than 3 side edges, the minimum to form 
a triangle.

Next I allocate (v-2) triangles where "v" is the number of vertices in the side's 
polygon. I construct the triangles in a fan pattern.

There's probably some improvements that can be made like storing shared edges and 
not having to reconnect them by checking distance, which I will probably learn as 
I follow in the footsteps of q3map and other Quake/Doom games' source code.

[edit2] By "nearest point to the plane" I mean nearest to the "inside" side of the 
plane, according to the normal. For an AA box we just check the signedness of each 
axis of the normal and use the min or max on each axis to get the innermost or 
outermost point of the 8 points of the AA box.

[edit3] And actually, the farthest point has to be the one from before the AA box 
moved and the "nearest" point has to be from the moved position. 

 */

#include "../platform.h"
#include "brush.h"
#include "plane.h"
#include "3dmath.h"
#include "line.h"
#include "polygon.h"
#include "../utils.h"
#include "3dmath.h"
#include "../sim/map.h"
#include "../save/edmap.h"

Brush& Brush::operator=(const Brush& original)
{
	/*
	int m_nsides;
	BrushSide* m_sides;
	*/

	//g_log<<"copy edbrush nsides="<<original.m_nsides<<endl;
	//g_log.flush();

	if(m_sides)
	{
		delete [] m_sides;
		m_sides = NULL;
	}

	if(m_sharedv)
	{
		delete [] m_sharedv;
		m_sharedv = NULL;
	}

	m_nsides = original.m_nsides;
	m_sides = new BrushSide[m_nsides];
	for(int i=0; i<m_nsides; i++)
		m_sides[i] = original.m_sides[i];
	m_nsharedv = original.m_nsharedv;
	m_sharedv = new Vec3f[m_nsharedv];
	for(int i=0; i<m_nsharedv; i++)
		m_sharedv[i] = original.m_sharedv[i];

	m_texture = original.m_texture;
	
	if(m_door)
	{
		delete m_door;
		m_door = NULL;
	}

	if(original.m_door)
	{
		m_door = new EdDoor();
		*m_door = *original.m_door;
	}

	return *this;
}

Brush::Brush(const Brush& original)
{
	m_sides = NULL;
	m_nsides = 0;
	m_sharedv = NULL;
	m_nsharedv = 0;
	m_texture = 0;
	m_door = NULL;
	*this = original;
}

Brush::Brush()
{
	m_sides = NULL;
	m_nsides = 0;
	m_sharedv = NULL;
	m_nsharedv = 0;
	m_texture = 0;
	m_door = NULL;

	//MessageBox(g_hWnd, "ed b constr", "asd", NULL);
}

Brush::~Brush()
{
#if 0
	g_log<<"~Brush"<<endl;
#endif

	if(m_sides)
	{
		delete [] m_sides;
		m_sides = NULL;
	}

	if(m_sharedv)
	{
		delete [] m_sharedv;
		m_sharedv = NULL;
	}

	m_nsharedv = 0;
	m_nsides = 0;

	if(m_door)
	{
		delete m_door;
		m_door = NULL;
	}
}

//#define SELECT_DEBUG

#ifdef SELECT_DEBUG
Brush* g_debugb = NULL;
#endif

Vec3f Brush::traceray(Vec3f line[], Vec3f* intersectionnormal)
{
	float startRatio = -1.0f;
	float endRatio = 1.0f;
	Vec3f intersection = line[1];

	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		float startD = Dot(line[0], s->m_plane.m_normal) + s->m_plane.m_d;
		float endD = Dot(line[1], s->m_plane.m_normal) + s->m_plane.m_d;

#ifdef SELECT_DEBUG
		if(g_debugb == this)
		{
			g_log<<"side "<<i<<endl;
			g_log<<"startD="<<startD<<", endD="<<endD<<endl;
			g_log.flush();
		}

		if(g_debugb == this)
		{
			g_log<<"traceray plane=("<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<"d="<<s->m_plane.m_d<<") startD="<<startD<<" endD="<<endD<<endl;
			g_log.flush();
		}
#endif

		if(startD > 0 && endD > 0)
		{
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"startD > 0 && endD > 0"<<endl;
				g_log.flush();
			}
#endif

			return line[1];
		}

		if(startD <= 0 && endD <= 0)
		{
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"startD <= 0 && endD <= 0"<<endl;
				g_log.flush();
			}
#endif

			continue;
		}

		if(startD > endD)
		{
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"startD > endD"<<endl;
				g_log.flush();
			}
#endif

			// This gets a ratio from our starting point to the approximate collision spot
			float ratio1 = (startD - EPSILON) / (startD - endD);
			
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"ratio1 ="<<ratio1<<endl;
				g_log.flush();
			}
#endif

			if(ratio1 > startRatio)
			{
				startRatio = ratio1;

				*intersectionnormal = s->m_plane.m_normal;
				
#ifdef SELECT_DEBUG
				if(g_debugb == this)
				{
					g_log<<"ratio1 > startRatio == "<<startRatio<<endl;
					g_log.flush();
				}
#endif
			}
		}
		else
		{
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"else startD <= endD"<<endl;
				g_log.flush();
			}
#endif

			float ratio = (startD + EPSILON) / (startD - endD);
			
#ifdef SELECT_DEBUG
			if(g_debugb == this)
			{
				g_log<<"ratio ="<<ratio<<endl;
				g_log.flush();
			}
#endif

			if(ratio < endRatio)
			{
				endRatio = ratio;
				
#ifdef SELECT_DEBUG
				if(g_debugb == this)
				{
					g_log<<"ratio < endRatio == "<<endRatio<<endl;
					g_log.flush();
				}
#endif
			}
		}
	}

	if(startRatio < endRatio)
	{
#ifdef SELECT_DEBUG
		if(g_debugb == this)
		{
			g_log<<"startRatio ("<<startRatio<<") < endRatio ("<<endRatio<<")"<<endl;
			g_log.flush();
		}
#endif

		if(startRatio > -1)
		{
#ifdef SELECT_DEBUG
				if(g_debugb == this)
				{
					g_log<<"startRatio > -1"<<endl;
					g_log.flush();
				}
#endif

			if(startRatio < 0)
				startRatio = 0;
			
#ifdef SELECT_DEBUG
			if(g_debugb == NULL)
				g_debugb = this;
#endif

			return line[0] + (line[1]-line[0]) * startRatio;
		}
	}

	return line[1];
}

void Brush::prunev(bool* invalidv)
{
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		for(int j=0; j<m_nsharedv; j++)
		{
			if(!PointOnOrBehindPlane(m_sharedv[j], s->m_plane))
				invalidv[j] = true;
		}
	}
}

void Brush::add(BrushSide b)
{
#if 0
	g_log<<"addside before: "<<endl;
	for(int vertindex = 0; vertindex < b.m_drawva.numverts; vertindex++)
	{
		Vec3f vert = b.m_drawva.vertices[vertindex];
		g_log<<"\taddvert: "<<vert.x<<","<<vert.y<<","<<vert.z<<endl;
	}
#endif

	BrushSide* newsides = new BrushSide[m_nsides + 1];
	
	if(m_nsides > 0)
	{
		//memcpy(newsides, m_sides, sizeof(BrushSide)*m_nsides);
		for(int i=0; i<m_nsides; i++)
			newsides[i] = m_sides[i];
		delete [] m_sides;
	}
	
	newsides[m_nsides] = b;
	m_sides = newsides;
	m_nsides ++;
	
#if 0
	g_log<<"addside after: "<<endl;
	for(int vertindex = 0; vertindex < b.m_drawva.numverts; vertindex++)
	{
		Vec3f vert = b.m_drawva.vertices[vertindex];
		g_log<<"\taddvert: "<<vert.x<<","<<vert.y<<","<<vert.z<<endl;
	}
#endif
}

void Brush::setsides(int nsides, BrushSide* sides)
{
	if(m_sides)
	{
		delete [] m_sides;
		m_sides = NULL;
		m_nsides = 0;
	}

	for(int i=0; i<nsides; i++)
		add(sides[i]);
}

void Brush::getsides(int* nsides, BrushSide** sides)
{
	*nsides = m_nsides;

	if(*sides)
	{
		delete [] *sides;
		*sides = NULL;
	}

	if(m_nsides > 0)
	{
		*sides = new BrushSide[m_nsides];

		for(int i=0; i<m_nsides; i++)
		{
			(*sides)[i] = m_sides[i];
		}
	}
}

//#define REMOVESIDE_DEBUG

void Brush::removeside(int i)
{
	BrushSide* newsides = new BrushSide[m_nsides-1];

	memcpy(&newsides[0], &m_sides[0], sizeof(BrushSide)*i);
	memcpy(&newsides[i], &m_sides[i+1], sizeof(BrushSide)*(m_nsides-i-1));

	m_nsides --;
	m_sides = newsides;

#ifdef REMOVESIDE_DEBUG
	g_log<<"remove "<<i<<endl;
	g_log.flush();
#endif
}

void Brush::remaptex()
{
	for(int i=0; i<m_nsides; i++)
		m_sides[i].remaptex();
}

void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const float radius, const float height)
{
	MakePlane(&norms[0], &ds[0], pos + Vec3f(0, height, 0), Vec3f(0, 1, 0));	//up
	MakePlane(&norms[1], &ds[1], pos + Vec3f(0, 0, 0), Vec3f(0, -1, 0));		//down
	MakePlane(&norms[2], &ds[2], pos + Vec3f(-radius, 0, 0), Vec3f(-1, 0, 0));	//left
	MakePlane(&norms[3], &ds[3], pos + Vec3f(radius, 0, 0), Vec3f(1, 0, 0));	//right
	MakePlane(&norms[4], &ds[4], pos + Vec3f(0, 0, -radius), Vec3f(0, 0, -1));	//front
	MakePlane(&norms[5], &ds[5], pos + Vec3f(0, 0, radius), Vec3f(0, 0, 1));	//back
}

void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const float hwx, const float hwz, const float height)
{
	MakePlane(&norms[0], &ds[0], pos + Vec3f(0, height, 0), Vec3f(0, 1, 0));	//up
	MakePlane(&norms[1], &ds[1], pos + Vec3f(0, 0, 0), Vec3f(0, -1, 0));		//down
	MakePlane(&norms[2], &ds[2], pos + Vec3f(-hwx, 0, 0), Vec3f(-1, 0, 0));		//left
	MakePlane(&norms[3], &ds[3], pos + Vec3f(hwx, 0, 0), Vec3f(1, 0, 0));		//right
	MakePlane(&norms[4], &ds[4], pos + Vec3f(0, 0, -hwz), Vec3f(0, 0, -1));		//front
	MakePlane(&norms[5], &ds[5], pos + Vec3f(0, 0, hwz), Vec3f(0, 0, 1));		//back
}

void MakeHull(Vec3f* norms, float* ds, const Vec3f pos, const Vec3f vmin, const Vec3f vmax)
{
	MakePlane(&norms[0], &ds[0], pos + Vec3f(0, vmax.y, 0), Vec3f(0, 1, 0));		//up
	MakePlane(&norms[1], &ds[1], pos + Vec3f(0, vmin.y, 0), Vec3f(0, -1, 0));		//down
	MakePlane(&norms[2], &ds[2], pos + Vec3f(vmin.x, 0, 0), Vec3f(-1, 0, 0));		//left
	MakePlane(&norms[3], &ds[3], pos + Vec3f(vmax.x, 0, 0), Vec3f(1, 0, 0));		//right
	MakePlane(&norms[4], &ds[4], pos + Vec3f(0, 0, vmin.z), Vec3f(0, 0, -1));		//front
	MakePlane(&norms[5], &ds[5], pos + Vec3f(0, 0, vmax.z), Vec3f(0, 0, 1));		//back
}

bool HullsIntersect(Vec3f* hull1norms, float* hull1dist, int hull1planes, Vec3f* hull2norms, float* hull2dist, int hull2planes)
{
	return false;
}

// line intersects convex hull?
bool LineInterHull(const Vec3f* line, Plane* planes, const int numplanes, Vec3f* intersection)
{
	for(int i=0; i<numplanes; i++)
    {
		Vec3f planeinter;
        if(LineInterPlane(line, planes[i].m_normal, -planes[i].m_d, &planeinter))
        {
			bool allin = true;
			for(int j=0; j<numplanes; j++)
			{
				if(i == j)
					continue;

				if(!PointOnOrBehindPlane(planeinter, planes[j]))
				{
					allin = false;
					break;
				}
			}
			if(allin)
			{
				*intersection = planeinter;
				return true;
			}
        }
    }

    return false;
}

// temporary hack
bool LineInterHull(const Vec3f* line, const Vec3f* norms, const float* ds, const int numplanes, Vec3f* intersection)
{
	Plane* planes = new Plane[numplanes];

	for(int i=0; i < numplanes; i++)
	{
		planes[i] = Plane(norms[i].x, norms[i].y, norms[i].z, ds[i]);
	}

	bool ret = LineInterHull(line, planes, numplanes, intersection);

	delete [] planes;

	return ret;
}