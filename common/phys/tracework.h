
#ifndef TRACEWORK_H
#define TRACEWORK_H


#include "../math/vec3f.h"

#define TRACE_BOX		0
#define TRACE_SPHERE	1
#define TRACE_RAY		2

class TraceJob	//assignment of job
{
public:
	int type;
	Vec3f vmin;
	Vec3f vmax;
	float radius;
	Vec3f start;
	Vec3f end;
	Vec3f extents;
	float maxstep;
	Vec3f absstart;
	Vec3f absend;
};

class TraceWork	//results
{
public:
	Vec3f clip;
	bool onground;
	bool atladder;
	bool stuck;
	bool trytostep;
	bool collided;
	float traceratio;
	Vec3f collisionnormal;
};

#endif	//TRACEWORK_H