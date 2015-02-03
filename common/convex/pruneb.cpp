
#include "pruneb.h"


bool PruneB2(Brush* b, Plane* p, float epsilon)
{
	for(int svi=0; svi<b->m_nsharedv; svi++)
	{
		Vec3f sv = b->m_sharedv[svi];

		if(PointOnOrBehindPlane(sv, *p, epsilon))
			return false;
	}

	return true;
}

//#define PRUNEB_DEBUG

bool PruneB(EdMap* map, Brush* b)
{
#ifdef PRUNEB_DEBUG
	g_log<<"-------------"<<endl;
	g_log.flush();
#endif

	//remove sides that share the exact same set of vertices
	for(int i=0; i<b->m_nsides; i++)
	{
#ifdef PRUNEB_DEBUG
		g_log<<"\t side"<<i<<endl;
#endif
		BrushSide* s1 = &b->m_sides[i];

		if(s1->m_ntris <= 0)
		{
			b->removeside(i);
			i--;
			continue;
		}

		bool allclose = true;
		Vec3f matchv = b->m_sharedv[ s1->m_vindices[0] ];

#ifdef PRUNEB_DEBUG
		g_log<<"s1->m_ntris = "<<s1->m_ntris<<endl;
#endif

		for(int v=0; v<s1->m_outline.m_edv.size(); v++)
		{
			Vec3f thisv = b->m_sharedv[ s1->m_vindices[v] ];

#ifdef PRUNEB_DEBUG
			g_log<<"vertex "<<v<<" = "<<thisv.x<<","<<thisv.y<<","<<thisv.z<<endl;
			g_log.flush();
#endif

			float mag = Magnitude(matchv - thisv);

			if(mag > MERGEV_D)
			{
				allclose = false;
#ifndef PRUNEB_DEBUG
				break;
#endif
			}
		}

		if(allclose)
		{
			b->removeside(i);
			i--;
			continue;
		}

		for(int j=i+1; j<b->m_nsides; j++)
		{
			BrushSide* s2 = &b->m_sides[j];
			bool same = true;

			for(int k=0; k<s1->m_outline.m_edv.size(); k++)
			{
				int matchindex = s1->m_vindices[k];
				bool have = false;

				for(int l=0; l<s2->m_outline.m_edv.size(); l++)
				{
					if(s2->m_vindices[l] == matchindex)
					{
						have = true;
						break;
					}
				}

				if(!have)
				{
					same = false;
					break;
				}
			}

			if(same)
			{
				b->removeside(j);
				break;
			}

			for(int k=0; k<s2->m_outline.m_edv.size(); k++)
			{
				int matchindex = s2->m_vindices[k];
				bool have = false;

				for(int l=0; l<s1->m_outline.m_edv.size(); l++)
				{
					if(s1->m_vindices[l] == matchindex)
					{
						have = true;
						break;
					}
				}

				if(!have)
				{
					same = false;
					break;
				}
			}

			if(same)
			{
				b->removeside(j);
				break;
			}
		}
	}

	//for(auto b=map->m_brush.begin(); b!=map->m_brush.end(); b++)
	{
		// remove brushes with less than 3 vertices or 4 sides
		if(b->m_nsharedv < 3 || b->m_nsides < 4)
		{
			for(int i=0; i<g_selB.size(); i++)
			{
				//if(g_selB[i] == &*b)
				if(g_selB[i] == b)
				{
					g_selB.erase( g_selB.begin() + i );
					//continue;
					i--;
				}
			}

			//if(g_sel1b == &*b)
			if(g_sel1b == b)
			{
				g_sel1b = NULL;
				g_dragV = -1;
				g_dragS = -1;
			}

			//b = map->m_brush.erase(b);

			for(auto i=map->m_brush.begin(); i!=map->m_brush.end(); i++)
			{
				if(&*i == b)
				{
					map->m_brush.erase(i);
					//break;
					return true;
				}
			}

			//g_log<<"pruned brush"<<endl;
			//g_log.flush();
			//continue;
			
			return true;
		}
	}

	return false;
}
