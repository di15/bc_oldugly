

#include "octree.h"
#include "../math/brush.h"

OcNode::OcNode()
{
	child[OCNODE_TOP_LEFT_FRONT] = NULL;
	child[OCNODE_TOP_LEFT_BACK] = NULL;
	child[OCNODE_TOP_RIGHT_BACK] = NULL;
	child[OCNODE_TOP_RIGHT_FRONT] = NULL;
	child[OCNODE_BOTTOM_LEFT_FRONT] = NULL;
	child[OCNODE_BOTTOM_LEFT_BACK] = NULL;
	child[OCNODE_BOTTOM_RIGHT_BACK] = NULL;
	child[OCNODE_BOTTOM_RIGHT_FRONT] = NULL;
}

OcNode::~OcNode()
{
	for(int i=0; i<8; i++)
	{
		if(child[i])
			delete child[i];
	}
}

void MakeOcNode(OcNode **ocnode, list<OcBrushRef> &parentbrushes, Vec3f vmin, Vec3f vmax, int level)
{
	list<OcBrushRef> herebrushes;

	for(auto biter = parentbrushes.begin(); biter != parentbrushes.end(); biter++)
	{
		Brush* pbrush = biter->pbrush;
		
		bool allin = true;

		for(int vertindex = 0; vertindex < pbrush->m_nsharedv; vertindex++)
		{
			Vec3f v = pbrush->m_sharedv[vertindex];

			if(v.x < vmin.x || v.y < vmin.y || v.z < vmin.z || v.x > vmax.x || v.y > vmax.y || v.z > vmax.z)
			{
				allin = false;
				break;
			}
		}

		if(!allin)
			continue;

		herebrushes.push_back(*biter);
		parentbrushes.erase(biter);
	}

	if(herebrushes.size() <= 0)
		return;

	(*ocnode) = new OcNode;

	// pass to lower level nodes

	(*ocnode)->brushes = herebrushes;
}

void MapMinMax(list<Brush> &brushes, Vec3f *vmin, Vec3f *vmax)
{
	bool firstset = false;

	for(auto brushitr = brushes.begin(); brushitr != brushes.end(); brushitr++)
	{
		Brush* b = &*brushitr;

		for(int vertindex = 0; vertindex < b->m_nsharedv; vertindex++)
		{
			Vec3f v = b->m_sharedv[vertindex];

			if(!firstset)
			{
				*vmin = v;
				*vmax = v;
				firstset = true;
			}
			else
			{
				if(v.x < vmin->x)
					vmin->x = v.x;
				if(v.y < vmin->y)
					vmin->y = v.y;
				if(v.z < vmin->z)
					vmin->z = v.z;
				
				if(v.x > vmax->x)
					vmax->x = v.x;
				if(v.y > vmax->y)
					vmax->y = v.y;
				if(v.z > vmax->z)
					vmax->z = v.z;
			}
		}
	}
}

void ConstructOc(OcNode **ochead, list<Brush> &brushes)
{
	list<OcBrushRef> passbrushes;

	int brushindex = 0;
	for(auto brushitr = brushes.begin(); brushitr != brushes.end(); brushitr++)
	{
		OcBrushRef ocbrushref;
		ocbrushref.brushindex = brushindex;
		ocbrushref.pbrush = &*brushitr;
		passbrushes.push_back(ocbrushref);
		brushindex++;
	}

	Vec3f vmin, vmax;

	MapMinMax(brushes, &vmin, &vmax);

	MakeOcNode(ochead, passbrushes, vmin, vmax, 0);
}