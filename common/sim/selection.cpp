
#include "../draw/shader.h"
#include "../texture.h"
#include "entity.h"
#include "entitytype.h"
#include "../platform.h"
#include "../math/matrix.h"
#include "../window.h"
#include "../math/plane.h"
#include "../math/frustum.h"
#include "../math/brush.h"

// Selected entity list
list<int> g_selection;

// Selection circle texture index
unsigned int g_circle = 0;

// Selection frustum for drag/area-selection
static Vec3f normalLeft;
static Vec3f normalTop;
static Vec3f normalRight;
static Vec3f normalBottom;
static float distLeft;
static float distTop;
static float distRight;
static float distBottom;

static Frustum g_selfrust;	//selection frustum

void DrawSelectionCircles(Matrix* projection, Matrix* modelmat, Matrix* viewmat)
{
	//if(g_projtype == PROJ_PERSPECTIVE)
	UseS(SHADER_BILLBOARDPERSP);
	//else
	//	UseS(SHADER_BILLBOAR);

	Shader* s = &g_shader[g_curS];

	glUniformMatrix4fv(s->m_slot[SSLOT_PROJECTION], 1, 0, projection->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_MODELMAT], 1, 0, modelmat->m_matrix);
	glUniformMatrix4fv(s->m_slot[SSLOT_VIEWMAT], 1, 0, viewmat->m_matrix);
	glUniform4f(s->m_slot[SSLOT_COLOR], 0, 1, 0, 1);
	glEnableVertexAttribArray(s->m_slot[SSLOT_POSITION]);
	glEnableVertexAttribArray(s->m_slot[SSLOT_TEXCOORD0]);
	glEnableVertexAttribArray(s->m_slot[SSLOT_NORMAL]);

	glActiveTextureARB(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, g_texture[ g_circle ].texname);
	glUniform1iARB(s->m_slot[SSLOT_TEXTURE0], 0);

	for(auto selectioniterator = g_selection.begin(); selectioniterator != g_selection.end(); selectioniterator++)
	{
		Entity* e = g_entity[ *selectioniterator ];
		//Entity* e = g_entity[ 0 ];
		Camera* c = &e->camera;
		EntityT* t = &g_entityT[ e->type ];

		Vec3f p = c->m_pos + Vec3f(0, t->vmin.y, 0) + Vec3f(0, 1.0f, 0);

		const float r = t->vmax.x * 3.0f;

		float vertices[] =
		{
			//posx, posy posz   texx, texy
			p.x + r, p.y + 1, p.z - r,          1, 0,
			p.x + r, p.y + 1, p.z + r,          1, 1,
			p.x - r, p.y + 1, p.z + r,          0, 1,

			p.x - r, p.y + 1, p.z + r,          0, 1,
			p.x - r, p.y + 1, p.z - r,          0, 0,
			p.x + r, p.y + 1, p.z - r,          1, 0
		};

		//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
		//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

		glVertexAttribPointer(s->m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[0]);
		glVertexAttribPointer(s->m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, &vertices[3]);
		//glVertexAttribPointer(s->m_slot[SLOT::NORMAL], 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, va->normals);

		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void AfterSelection()
{
}

void SelectOne(Vec3f campos, Vec3f camside, Vec3f camup2, Vec3f viewdir)
{
	Vec3f ray = ScreenPerspRay(g_mouse.x, g_mouse.y, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f line[2];
	line[0] = campos;
	line[1] = campos + (ray * 10000.0f);

	float closestd = -1;

	for(int i=0; i<ENTITIES; i++)
	{
		Entity* e = g_entity[i];

		if(!e)
			continue;

		Camera* c = &e->camera;
		EntityT* t = &g_entityT[ e->type ];

		/*
		Vec3f p1 = c->m_pos;
		Vec3f p2 = c->m_pos + Vec3f(0, t->vmin.y, 0);
		Vec3f p3 = c->m_pos + Vec3f(0, t->vmax.y, 0);
		Vec3f p4 = c->m_pos + Vec3f(0, (t->vmin.y + t->vmax.y) / 2.0f, 0);

		//Not within marquee?
		//if(!PointInsidePlanes( c->m_pos + Vec3f(0,t->vmin.y,0) ))
		//if(!PointInsidePlanes( c->m_pos ))
		if(!PointInsidePlanes( p1 )
			&& !PointInsidePlanes( p2 )
			&& !PointInsidePlanes( p3 )
			&& !PointInsidePlanes( p4 ))
			continue;*/
		
		//Vec3f vmin = c->m_pos + t->vmin;
		//Vec3f vmax = c->m_pos + t->vmax;

		//if(!g_selfrust.BoxInFrustum2(vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z))
		//	continue;

		Vec3f normals[6];
		float dists[6];

		MakeHull(normals, dists, c->m_pos, t->vmin, t->vmax);

		Vec3f intersection;

		if(!LineInterHull(line, normals, dists, 6, &intersection))
			continue;

		float thisd = Magnitude(line[0] - intersection);

		if(thisd < closestd || closestd < 0)
		{
			g_selection.clear();
			g_selection.push_back(i);
			closestd = thisd;
		}
	}
}

bool PointBehindPlane(Vec3f p, Vec3f normal, float dist)
{
        float result = p.x*normal.x + p.y*normal.y + p.z*normal.z + dist;

        if(result <= EPSILON)
                return true;

        return false;
}

bool PointInsidePlanes(Vec3f p)
{
        if(!PointBehindPlane(p, normalLeft, distLeft))
                return false;

        if(!PointBehindPlane(p, normalTop, distTop))
                return false;

        if(!PointBehindPlane(p, normalRight, distRight))
                return false;

        if(!PointBehindPlane(p, normalBottom, distBottom))
                return false;

        return true;
}

#if 0
void SelectAreaOrtho()
{
	int minx = min(g_mousestart.x, g_mouse.x);
	int maxx = max(g_mousestart.x, g_mouse.x);
	int miny = min(g_mousestart.y, g_mouse.y);
	int maxy = max(g_mousestart.y, g_mouse.y);

	Vec3f ray = g_camera.View() - g_camera.Position();
	Vec3f onnear = OnNear(minx, miny);
	Vec3f lineTopLeft[2];
	lineTopLeft[0] = onnear;
	lineTopLeft[1] = onnear + (ray * 100000.0f);

	onnear = OnNear(maxx, miny);
	Vec3f lineTopRight[2];
	lineTopRight[0] = onnear;
	lineTopRight[1] = onnear + (ray * 100000.0f);

	onnear = OnNear(minx, maxy);
	Vec3f lineBottomLeft[2];
	lineBottomLeft[0] = onnear;
	lineBottomLeft[1] = onnear + (ray * 100000.0f);

	onnear = OnNear(maxx, maxy);
	Vec3f lineBottomRight[2];
	lineBottomRight[0] = onnear;
	lineBottomRight[1] = onnear + (ray * 100000.0f);

	Vec3f interTopLeft;
	Vec3f interTopRight;
	Vec3f interBottomLeft;
	Vec3f interBottomRight;

	GetMapIntersection2(lineTopLeft, &interTopLeft);
	GetMapIntersection2(lineTopRight, &interTopRight);
	GetMapIntersection2(lineBottomLeft, &interBottomLeft);
	GetMapIntersection2(lineBottomRight, &interBottomRight);

	Vec3f leftPoly[3];
	Vec3f topPoly[3];
	Vec3f rightPoly[3];
	Vec3f bottomPoly[3];

	leftPoly[0] = g_camera.m_pos;
	leftPoly[1] = interBottomLeft;
	leftPoly[2] = interTopLeft;

	topPoly[0] = g_camera.m_pos;
	topPoly[1] = interTopLeft;
	topPoly[2] = interTopRight;

	rightPoly[0] = g_camera.m_pos;
	rightPoly[1] = interTopRight;
	rightPoly[2] = interBottomRight;

	bottomPoly[0] = g_camera.Position();
	bottomPoly[1] = interBottomRight;
	bottomPoly[2] = interBottomLeft;

	g_normalLeft = Normal(leftPoly);
	g_normalTop = Normal(topPoly);
	g_normalRight = Normal(rightPoly);
	g_normalBottom = Normal(bottomPoly);

	g_distLeft = PlaneDistance(g_normalLeft, leftPoly[0]);
	g_distTop = PlaneDistance(g_normalTop, topPoly[0]);
	g_distRight = PlaneDistance(g_normalRight, rightPoly[0]);
	g_distBottom = PlaneDistance(g_normalBottom, bottomPoly[0]);
}
#endif

#if 0
// This determines if a BOX is in or around our frustum by it's min and max points
bool BoxInsidePlanes( float x, float y, float z, float x2, float y2, float z2)
{
	// Go through all of the corners of the box and check then again each plane
	// in the frustum.  If all of them are behind one of the planes, then it most
	// like is not in the frustum.
	for(int i = 0; i < 6; i++ )
	{
		if(m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y  + m_Frustum[i][C] * z  + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x  + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z  + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x  + m_Frustum[i][B] * y  + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y  + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x  + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0)  continue;
		if(m_Frustum[i][A] * x2 + m_Frustum[i][B] * y2 + m_Frustum[i][C] * z2 + m_Frustum[i][D] > 0)  continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	// Return a true for the box being inside of the frustum
	return true;
}
#endif

void SelectAreaPersp(Vec3f campos, Vec3f camside, Vec3f camup2, Vec3f viewdir)
{
	int minx = min(g_mousestart.x, g_mouse.x);
	int maxx = max(g_mousestart.x, g_mouse.x);
	int miny = min(g_mousestart.y, g_mouse.y);
	int maxy = max(g_mousestart.y, g_mouse.y);

	//Vec3f campos = g_camera.m_pos;
	//Vec3f camside = g_camera.m_strafe;
	//Vec3f camup2 = g_camera.up2();
	//Vec3f viewdir = Normalize( g_camera.m_view - g_camera.m_pos );

	Vec3f topLeftRay = ScreenPerspRay(minx, miny, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineTopLeft[2];
	lineTopLeft[0] = campos;
	lineTopLeft[1] = campos + (topLeftRay * 10000.0f);

	Vec3f topRightRay = ScreenPerspRay(maxx, miny, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineTopRight[2];
	lineTopRight[0] = campos;
	lineTopRight[1] = campos + (topRightRay * 10000.0f);

	Vec3f bottomLeftRay = ScreenPerspRay(minx, maxy, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineBottomLeft[2];
	lineBottomLeft[0] = campos;
	lineBottomLeft[1] = campos + (bottomLeftRay * 10000.0f);

	Vec3f bottomRightRay = ScreenPerspRay(maxx, maxy, g_width, g_height, campos, camside, camup2, viewdir, FIELD_OF_VIEW);
	Vec3f lineBottomRight[2];
	lineBottomRight[0] = campos;
	lineBottomRight[1] = campos + (bottomRightRay * 10000.0f);
	
#if 0
	Vec3f interTopLeft;
	Vec3f interTopRight;
	Vec3f interBottomLeft;
	Vec3f interBottomRight;

	GetMapIntersection2(lineTopLeft, &interTopLeft);
	GetMapIntersection2(lineTopRight, &interTopRight);
	GetMapIntersection2(lineBottomLeft, &interBottomLeft);
	GetMapIntersection2(lineBottomRight, &interBottomRight);
#endif

	Vec3f leftPoly[3];
	Vec3f topPoly[3];
	Vec3f rightPoly[3];
	Vec3f bottomPoly[3];
	Vec3f frontPoly[3];
	Vec3f backPoly[3];

	//Assemble polys in clockwise order so that their normals face outward

	leftPoly[0] = campos;
#if 0
	leftPoly[1] = interBottomLeft;
	leftPoly[2] = interTopLeft;
#endif
	leftPoly[1] = lineBottomLeft[1];
	leftPoly[2] = lineTopLeft[1];

	topPoly[0] = campos;
#if 0
	topPoly[1] = interTopLeft;
	topPoly[2] = interTopRight;
#endif
	topPoly[1] = lineTopLeft[1];
	topPoly[2] = lineTopRight[1];

	rightPoly[0] = campos;
#if 0
	rightPoly[1] = interTopRight;
	rightPoly[2] = interBottomRight;
#endif
	rightPoly[1] = lineTopRight[1];
	rightPoly[2] = lineBottomRight[1];

	bottomPoly[0] = campos;
#if 0
	bottomPoly[1] = interBottomRight;
	bottomPoly[2] = interBottomLeft;
#endif
	bottomPoly[1] = lineBottomRight[1];
	bottomPoly[2] = lineBottomLeft[1];
	
	frontPoly[0] = lineBottomLeft[1];
	frontPoly[1] = lineBottomRight[1];
	frontPoly[2] = lineTopRight[1];

	backPoly[0] = lineTopRight[0] + topLeftRay;
	backPoly[1] = lineBottomRight[0] + bottomRightRay;
	backPoly[2] = lineBottomLeft[0] + bottomLeftRay;

	// Normals will actually face inward
	// (using counter-clockwise function Normal2)

	normalLeft = Normal2(leftPoly);
	normalTop = Normal2(topPoly);
	normalRight = Normal2(rightPoly);
	normalBottom = Normal2(bottomPoly);
	Vec3f normalFront = Normal(frontPoly);
	Vec3f normalBack = Normal(backPoly);

	distLeft = PlaneDistance(normalLeft, leftPoly[0]);
	distTop = PlaneDistance(normalTop, topPoly[0]);
	distRight = PlaneDistance(normalRight, rightPoly[0]);
	distBottom = PlaneDistance(normalBottom, bottomPoly[0]);
	float distFront = PlaneDistance(normalFront, frontPoly[0]);
	float distBack = PlaneDistance(normalBack, backPoly[0]);

	g_selfrust.construct(
		Plane(normalLeft.x, normalLeft.y, normalLeft.z, distLeft),
		Plane(normalRight.x, normalRight.y, normalRight.z, distRight),
		Plane(normalTop.x, normalTop.y, normalTop.z, distTop),
		Plane(normalBottom.x, normalBottom.y, normalBottom.z, distBottom),
		Plane(normalFront.x, normalFront.y, normalFront.z, distFront),
		Plane(normalBack.x, normalBack.y, normalBack.z, distBack));

	for(int i=0; i<ENTITIES; i++)
	{
		Entity* e = g_entity[i];

		if(!e)
			continue;

		Camera* c = &e->camera;
		EntityT* t = &g_entityT[ e->type ];

		/*
		Vec3f p1 = c->m_pos;
		Vec3f p2 = c->m_pos + Vec3f(0, t->vmin.y, 0);
		Vec3f p3 = c->m_pos + Vec3f(0, t->vmax.y, 0);
		Vec3f p4 = c->m_pos + Vec3f(0, (t->vmin.y + t->vmax.y) / 2.0f, 0);

		//Not within marquee?
		//if(!PointInsidePlanes( c->m_pos + Vec3f(0,t->vmin.y,0) ))
		//if(!PointInsidePlanes( c->m_pos ))
		if(!PointInsidePlanes( p1 )
			&& !PointInsidePlanes( p2 )
			&& !PointInsidePlanes( p3 )
			&& !PointInsidePlanes( p4 ))
			continue;*/
		
		Vec3f vmin = c->m_pos + t->vmin;
		Vec3f vmax = c->m_pos + t->vmax;

		if(!g_selfrust.BoxInFrustum2(vmin.x, vmin.y, vmin.z, vmax.x, vmax.y, vmax.z))
			continue;

		g_selection.push_back(i);
	}
}

void DoSelection(Vec3f campos, Vec3f camside, Vec3f camup2, Vec3f viewdir)
{
	g_selection.clear();

	if(g_mousestart.x == g_mouse.x && g_mousestart.y == g_mouse.y)
		SelectOne(campos, camside, camup2, viewdir);
	else
		SelectAreaPersp(campos, camside, camup2, viewdir);

	AfterSelection();
}
