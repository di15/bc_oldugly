



#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "../platform.h"
#include "3dmath.h"
#include "plane.h"

class Frustum 
{
public:
	void construct(const Plane left, const Plane right, const Plane top, const Plane bottom, const Plane front, const Plane back);
	void CalculateFrustum(const float* proj, const float* modl);
	bool PointInFrustum(float x, float y, float z);
	bool SphereInFrustum(float x, float y, float z, float radius);
	bool CubeInFrustum(float x, float y, float z, float size);
	//bool BoxInFrustum(float x, float y, float z, float sizeX, float sizeY, float sizeZ);
	bool BoxInFrustum(float x, float y, float z, float x2, float y2, float z2);
	bool BoxInFrustum2(float x, float y, float z, float x2, float y2, float z2);

private:
	float m_Frustum[6][4];	// This holds the A B C and D values for each side of our frustum.
};

extern Frustum g_frustum;

#endif
