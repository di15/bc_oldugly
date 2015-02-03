
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

void Brush::moveto(Vec3f newp)
{
	Vec3f currp;

	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];
		currp = currp + s->m_centroid;
	}

	currp = currp / (float)m_nsides;

	Vec3f delta = newp - currp;

	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		Vec3f pop = PointOnPlane(s->m_plane);
		pop = pop + delta;
		s->m_plane.m_d = PlaneDistance(s->m_plane.m_normal, pop);

		for(int j=0; j<2; j++)
		{
			pop = PointOnPlane(s->m_tceq[j]);
			pop = pop + delta;
			s->m_tceq[j].m_d = PlaneDistance(s->m_tceq[j].m_normal, pop);
		}
	}

	collapse();
	remaptex();
}

//#define DEBUG_COLLAPSE

//collapse geometry into triangles
void Brush::collapse()
{
	int oldnsharedv = m_nsharedv;

#ifdef DEBUG_COLLAPSE
	g_log<<"==================collapse this===================="<<endl;
	g_log.flush();
	
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];
		Vec3f n = s->m_plane.m_normal;
		float d = s->m_plane.m_d;
		g_log<<"\t side["<<i<<"] plane=("<<n.x<<","<<n.y<<","<<n.z<<"),"<<d<<endl;
	}
#endif

	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];
		if(s->m_tris)
		{
			delete [] s->m_tris;
			s->m_tris = NULL;
		}
		s->m_ntris = 0;

		if(s->m_vindices)
		{
			delete [] s->m_vindices;
			s->m_vindices = NULL;
		}
	}

	if(m_sharedv)
	{
		delete [] m_sharedv;
		m_sharedv = NULL;
	}
	m_nsharedv = 0;

	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* a = &m_sides[i];
		for(int j=i+1; j<m_nsides; j++)
		{
			BrushSide* b = &m_sides[j];
			if(Close(a->m_plane, b->m_plane))
			{
				
#ifdef DEBUG_COLLAPSE
				g_log<<"planes["<<i<<","<<j<<"] ("<<a->m_plane.m_normal.x<<","<<a->m_plane.m_normal.y<<","<<a->m_plane.m_normal.z<<"),"<<a->m_plane.m_d<<" and ("<<b->m_plane.m_normal.x<<","<<b->m_plane.m_normal.y<<","<<b->m_plane.m_normal.z<<"),"<<b->m_plane.m_d<<endl;
				g_log.flush();
#endif
				removeside(j);
				j--;
			}
		}
	}

	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag6. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif
	
	list<Line>* sideedges = new list<Line>[m_nsides];	// a line along a plane intersecting two other planes. both vertices form the edge of a polygon.

	for(int i=0; i<m_nsides; i++)
	{
		//for(int j=i+1; j<m_nsides; j++)
		for(int j=0; j<m_nsides; j++)
		{
			if(j == i)
				continue;

			for(int k=0; k<m_nsides; k++)
			{
				if(k == i || k == j)
					continue;
				
				//for(int l=0; l<m_nsides; l++)
				for(int l=k+1; l<m_nsides; l++)
				{
					if(l == i || l == j || l == k)
						continue;

					BrushSide* a = &m_sides[i];
					BrushSide* b = &m_sides[j];
					BrushSide* c = &m_sides[k];
					BrushSide* d = &m_sides[l];

					//if(a->m_plane == b->m_plane)
					//if(Close(a->m_plane, b->m_plane))
					//{
					//	g_log<<"close "<<i<<","<<j<<endl;
					//	g_log.flush();
					//	continue;
					//}

					//if(Close(a->m_plane, c->m_plane))
					//{
					//	g_log<<"close "<<i<<","<<k<<endl;
					//	g_log.flush();
					//	continue;
					//}
				
#ifdef DEBUG_COLLAPSE
					g_log<<"--------new side"<<i<<" edge--------"<<endl;
					g_log.flush();
#endif

					// http://devmaster.net/forums/topic/8676-2-plane-intersection/page__view__findpost__p__47568
					Vec3f linedir = Cross(a->m_plane.m_normal, b->m_plane.m_normal);
					Vec3f pointonplanea = PointOnPlane(a->m_plane);	// arbitrary point on plane A
					Vec3f v = Cross(linedir, a->m_plane.m_normal); // direction toward plane B, parallel to plane A
					Vec3f l0;
					
#ifdef DEBUG_COLLAPSE
					g_log<<"collapse ("<
				
					g_log<<"linedir="<<linedir.x<<","<<linedir.y<<","<<linedir.z<<endl;
					g_log<<"pointonplanea="<<pointonplanea.x<<","<<pointonplanea.y<<","<<pointonplanea.z<<endl;
					g_log.flush();
#endif

					if(!Intersection(pointonplanea, v, b->m_plane, l0))
						continue;
					
#ifdef DEBUG_COLLAPSE
					g_log<<"l0="<<l0.x<<","<<l0.y<<","<<l0.z<<endl;
					g_log.flush();
					
					g_log<<"\tcollapse ("<<i<<","<<j<<","<<k<<","<<l<<") 2"<<endl;
					g_log.flush();
#endif
					Vec3f lineorigin;

					if(!Intersection(l0, linedir, c->m_plane, lineorigin))
						continue;
					
#ifdef DEBUG_COLLAPSE
					g_log<<"\t\tcollapse ("<<i<<","<<j<<","<<k<<","<<l<<") 3"<<endl;
					g_log.flush();
#endif
					Vec3f lineend;

					if(!Intersection(l0, linedir, d->m_plane, lineend))
						continue;
					
#ifdef DEBUG_COLLAPSE
					g_log<<"\t\t\tcollapse ("<<i<<","<<j<<","<<k<<","<<l<<") 4"<<endl;
					g_log.flush();
#endif

					if(Close(lineorigin, lineend))
					//if(lineorigin == lineend)
					{
#ifdef DEBUG_COLLAPSE
						g_log<<"close side["<<i<<"] newedge ("<<lineorigin.x<<","<<lineorigin.y<<","<<lineorigin.z<<")->("<<lineend.x<<","<<lineend.y<<","<<lineend.z<<")"<<endl;
#endif
						continue;
					}

					Line edge(lineorigin, lineend);
					sideedges[i].push_back(edge);
					
#ifdef DEBUG_COLLAPSE
					g_log<<"\t\t\t\tfinal=("<<lineorigin.x<<","<<lineorigin.y<<","<<lineorigin.z<<") final=("<<lineend.x<<","<<lineend.y<<","<<lineend.z<<")"<<endl;
					g_log.flush();
#endif
				}
			}
		}
		
#ifdef DEBUG_COLLAPSE
		g_log<<"side"<<i<<" remaining edges"<<endl;
		for(auto k=sideedges[i].begin(); k!=sideedges[i].end(); k++)
		{
			Line l = *k;
			g_log<<"("<<l.m_vertex[0].x<<","<<l.m_vertex[0].y<<","<<l.m_vertex[0].z<<")->("<<l.m_vertex[1].x<<","<<l.m_vertex[1].y<<","<<l.m_vertex[1].z<<")"<<endl;
		}
#endif
	}

	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag7. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif
			
	//remove side edges that have any vertex outside (and not on) of at least one other plane
	for(int i=0; i<m_nsides; i++)
	{
		for(int j=0; j<m_nsides; j++)
		{
			if(i == j)
				continue;

			BrushSide* s = &m_sides[j];

			auto k=sideedges[i].begin();
			while(k != sideedges[i].end())
			{
				if(!PointOnOrBehindPlane(k->m_vertex[0], s->m_plane) || !PointOnOrBehindPlane(k->m_vertex[1], s->m_plane))
				{
#ifdef DEBUG_COLLAPSE
					g_log<<"-----------remove side["<<i<<"] edge ("<<k->m_vertex[0].x<<","<<k->m_vertex[0].y<<","<<k->m_vertex[0].z<<")->("<<k->m_vertex[1].x<<","<<k->m_vertex[1].y<<","<<k->m_vertex[1].z<<") out of plane["<<j<<"]=("<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<"d="<<s->m_plane.m_d<<")--------------"<<endl;
					g_log.flush();
#endif
					k = sideedges[i].erase(k);
					continue;
				}

				k++;
			}
		}

#ifdef DEBUG_COLLAPSE
		g_log<<"side"<<i<<" 2remaining edges"<<endl;
		for(auto k=sideedges[i].begin(); k!=sideedges[i].end(); k++)
		{
			Line l = *k;
			g_log<<"("<<l.m_vertex[0].x<<","<<l.m_vertex[0].y<<","<<l.m_vertex[0].z<<")->("<<l.m_vertex[1].x<<","<<l.m_vertex[1].y<<","<<l.m_vertex[1].z<<")"<<endl;
		}
#endif
	}
	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag8. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif
	
	//construct outlines of polygons from side edges
	//Polyg* sidepolys = new Polyg[m_nsides];
	for(int i=0; i<m_nsides; i++)
	{
#ifdef DEBUG_COLLAPSE
		for(auto j=sideedges[i].begin(); j!=sideedges[i].end(); j++)
		{
			Line* edge = &*j;
			if(Close(edge->m_vertex[0], edge->m_vertex[1]))
			{
				//j = sideedges[i].erase(j);
				g_log<<"close side["<<i<<"] edge ("<<edge->m_vertex[0].x<<","<<edge->m_vertex[0].y<<","<<edge->m_vertex[0].z<<")->("<<edge->m_vertex[1].x<<","<<edge->m_vertex[1].y<<","<<edge->m_vertex[1].z<<")"<<endl;
			}
		}
#endif

		if(sideedges[i].size() < 3)
		{
#ifdef DEBUG_COLLAPSE
			g_log<<"sideedges["<<i<<"] < 3"<<endl;
			g_log.flush();
#endif
			continue;
		}

		auto j=sideedges[i].begin();
		//auto lastconnection = j;
		set<Line*> connected;
		//set<Vec3f> connectedv;
		list<Vec3f> connectedv;
		//set<Vec3f> connected;

		Vec3f first = j->m_vertex[0];
		//connectedv.insert(first);
		connectedv.push_back(first);
		//sidepolys[i].m_vertex.push_back(first);
		BrushSide* s = &m_sides[i];
		s->m_outline.m_edv.clear();
		s->m_outline.m_edv.push_back(first);

		Vec3f curr = j->m_vertex[1];
		for(j++; j!=sideedges[i].end(); j++)
		{
#ifdef DEBUG_COLLAPSE
			g_log<<"sideedges["<<i<<"] first="<<first.x<<","<<first.y<<","<<first.z<<endl;
			g_log.flush();
#endif

			//if(j == lastconnection)
			//	continue;

			if(connected.count(&*j) > 0)
				continue;

			int whichclose = -1;
			
#ifdef DEBUG_COLLAPSE
			g_log<<"\tclose? "<<j->m_vertex[0].x<<","<<j->m_vertex[0].y<<","<<j->m_vertex[0].z<<" == "<<curr.x<<","<<curr.y<<","<<curr.z<<endl;
			g_log.flush();
#endif

			if(Close(j->m_vertex[0], curr))
			{
#ifdef DEBUG_COLLAPSE
				g_log<<"\t\tyes"<<endl;
				g_log.flush();
#endif
				whichclose = 0;
				goto foundnext;
			}
			
#ifdef DEBUG_COLLAPSE
			g_log<<"\tclose? "<<j->m_vertex[1].x<<","<<j->m_vertex[1].y<<","<<j->m_vertex[1].z<<" == "<<curr.x<<","<<curr.y<<","<<curr.z<<endl;
			g_log.flush();
#endif

			if(Close(j->m_vertex[1], curr))
			{
#ifdef DEBUG_COLLAPSE
				g_log<<"\t\tyes"<<endl;
				g_log.flush();
#endif
				whichclose = 1;
				goto foundnext;
			}

			foundnext:

			if(whichclose >= 0)
			{
				//lastconnection = j;
				connected.insert(&*j);

				Vec3f next = j->m_vertex[1-whichclose];

				bool found = false;
				for(auto k=connectedv.begin(); k!=connectedv.end(); k++)
				{
					if(Close(*k, next))
					{
						found = true;
						break;
					}
				}

				//if(connectedv.count( next ) > 0)
				if(found)
				{
#ifdef DEBUG_COLLAPSE
					g_log<<"\t\t found"<<endl;
#endif
					if(Close(next, first))
					{
#ifdef DEBUG_COLLAPSE
						g_log<<"\t\t\t next close to first"<<endl;
#endif
						if(s->m_outline.m_edv.size() < 2)
						{
#ifdef DEBUG_COLLAPSE
							g_log<<"found close(next,first) outline verts < 3"<<endl;
#endif
							continue;
						}
					}
					else
						continue;
				}
#ifdef DEBUG_COLLAPSE
				else
					g_log<<"\t\t unique"<<endl;
#endif

				//connectedv.insert(next);
				connectedv.push_back(next);

				//sidepolys[i].m_vertex.push_back(j->m_vertex[whichclose]);
				s->m_outline.m_edv.push_back(curr);

				if(Close(curr, next))
					break;	//avoid infinite loop in degenerate sides

				curr = next;
				
#ifdef DEBUG_COLLAPSE
				g_log<<"\t\tcurr = "<<curr.x<<","<<curr.y<<","<<curr.z<<endl;
				g_log.flush();
#endif
				
				//if(Close(j->m_vertex[0], first) || Close(j->m_vertex[1], first))
				if(Close(curr, first))
				{
#ifdef DEBUG_COLLAPSE
					g_log<<"\t\t\tpolygon loop complete"<<endl;

					for(auto k=s->m_outline.m_edv.begin(); k!=s->m_outline.m_edv.end(); k++)
					{
						g_log<<"("<<(*k).x<<","<<(*k).y<<","<<(*k).z<<")->";
					}

					g_log<<endl;

					g_log.flush();
#endif
					//sidepolys[i].m_vertex.push_back(curr);
					break;	//loop completed
				}

				j = sideedges[i].begin();
			}
		}
	}

	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag9. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif
	
	//make sure all polys are clockwise
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		Vec3f tri[3];
		//auto j = sidepolys[i].m_vertex.begin();
		auto j = s->m_outline.m_edv.begin();
		tri[0] = *j;	j++;
		tri[1] = *j;	j++;
		tri[2] = *j;

		Vec3f norm = Normal(tri);
		Vec3f revnorm = Normal2(tri);

		if(Magnitude(s->m_plane.m_normal - revnorm) < Magnitude(s->m_plane.m_normal - norm))
		{
#ifdef DEBUG_COLLAPSE
			g_log<<"reverse polygon loop order (revnorm("<<revnorm.x<<","<<revnorm.y<<","<<revnorm.z<<") is closer to planenorm("<<s->m_plane.m_normal.x<<","<<s->m_plane.m_normal.y<<","<<s->m_plane.m_normal.z<<")d="<<s->m_plane.m_d<<")"<<endl;
#endif
			//sidepolys[i].m_vertex.reverse();
			s->m_outline.m_edv.reverse();
		}
	}
	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag10. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif

	//construct triangles from polygons
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];
		//s->m_ntris = (int)sidepolys[i].m_vertex.size() - 2;
		s->m_ntris = (int)s->m_outline.m_edv.size() - 2;
		
		//g_log<<"sidepolys["<<i<<"].m_vertex.size() = "<<(int)sidepolys[i].m_vertex.size()<<endl;
		//g_log<<"sidepolys["<<i<<"].m_vertex.size() = "<<(int)s->m_outline.m_edv.size()<<endl;

		if(s->m_ntris <= 0)
		{
#ifdef DEBUG_COLLAPSE
			g_log<<"tris["<<i<<"] = "<<0<<endl;
#endif
			continue;
		}
		
#ifdef DEBUG_COLLAPSE
			g_log<<"tris["<<i<<"] = "<<s->m_ntris<<endl;
#endif
			g_log.flush();

		s->m_tris = new Triangle2[s->m_ntris];
		
		//auto j = sidepolys[i].m_vertex.begin();
		auto j = s->m_outline.m_edv.begin();
		Vec3f first = *j;
		j++;
		Vec3f prev = *j;
		j++;
		//for(int tri=0; j!=sidepolys[i].m_vertex.end(); j++, tri++)
		for(int tri=0; j!=s->m_outline.m_edv.end(); j++, tri++)
		{
			Vec3f curr = *j;
			s->m_tris[tri].m_vertex[0] = first;
			s->m_tris[tri].m_vertex[1] = prev;
			s->m_tris[tri].m_vertex[2] = curr;
			prev = curr;
		}

		s->makeva();
	}

	
#ifdef DEBUG_COLLAPSE
	g_log<<"tag11. m_nsides="<<m_nsides<<endl;
	g_log.flush();
#endif
	
	//delete [] sidepolys;
	bool* removes = new bool[m_nsides];	//degenerate sides to remove
	
	//remove bounding planes outside of the brush.
	//if brush side has no side edges, remove it.
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		//if(sideedges[i].size() <= 0)
		if(sideedges[i].size() < 3 || 
			s->m_outline.m_edv.size() < 3 ||	//3 is the minimum number of edges to form a polygon
			s->m_ntris <= 0)
		{
#ifdef DEBUG_COLLAPSE
			g_log<<"remove side. "<<i<<" sideedges[i].size()="<<sideedges[i].size()<<" s->m_outline.m_edv.size()="<<s->m_outline.m_edv.size()<<" s->m_ntris="<<s->m_ntris<<endl;
			g_log.flush();
#endif
			//removeside(i);
			//i--;
			removes[i] = true;
		}
		else
			removes[i] = false;
	}

	for(int i=m_nsides-1; i>=0; i--)
		if(removes[i])
			removeside(i);

	delete [] removes;

	delete [] sideedges;
	
#ifdef DEBUG_COLLAPSE
	g_log<<"1a"<<endl;
	g_log.flush();
#endif

	//get shared vertices and centroids
	vector<Vec3f> sharedv;
	for(int i=0; i<m_nsides; i++)
	{
		BrushSide* s = &m_sides[i];

		if(s->m_ntris <= 0)
			continue;
		
		//s->m_vindices = new int[s->m_outline.m_edv.size()];
		s->m_vindices = new int[s->m_ntris+2];
		
#ifdef DEBUG_COLLAPSE
	g_log<<"1b side"<<i<<" vindices="<<s->m_outline.m_edv.size()<<endl;
	g_log.flush();
#endif

		Vec3f centroid(0,0,0);
		float count = 0;

		int jj=0;
		for(auto j=s->m_outline.m_edv.begin(); j!=s->m_outline.m_edv.end(); j++, jj++)
		{
			centroid = centroid * (count/(count+1)) + (*j) * (1.0f/(count+1));
#ifdef DEBUG_COLLAPSE
			g_log<<"centroid "<<count<<" "<<centroid.x<<","<<centroid.y<<","<<centroid.z<<" *j "<<(*j).x<<","<<(*j).y<<","<<(*j).z<<endl;
			g_log.flush();
#endif

			count += 1;

			bool found = false;
			for(unsigned int k=0; k<sharedv.size(); k++)
			{
				if(Close(sharedv[k], *j))
				{
					found = true;
					s->m_vindices[jj] = k;
				}
			}

			if(!found)
			{
				sharedv.push_back(*j);
				s->m_vindices[jj] = sharedv.size()-1;
			}
		}

		s->m_centroid = centroid;
		
#ifdef DEBUG_COLLAPSE
		g_log<<"1c side"<<i<<endl;
		g_log.flush();
#endif
	}
	
#ifdef DEBUG_COLLAPSE
	g_log<<"1d"<<endl;
	g_log.flush();
#endif

	m_nsharedv = sharedv.size();
	m_sharedv = new Vec3f[m_nsharedv];
	for(int i=0; i<m_nsharedv; i++)
		m_sharedv[i] = sharedv[i];
	
#ifdef DEBUG_COLLAPSE
	g_log<<"shared vertices num = "<<m_nsharedv<<endl;
	g_log.flush();
#endif

	if(g_sel1b == this && oldnsharedv != m_nsharedv)
		g_dragV = -1;
}
