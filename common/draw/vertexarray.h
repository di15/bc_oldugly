

#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H

#include "../platform.h"
#include "../draw/ms3d.h"
#include "../math/vec3f.h"
#include "../math/vec2f.h"

class VertexArray
{
public:
	int numverts;
	Vec3f* vertices;
	Vec2f* texcoords;
	Vec3f* normals;
	//Vec3f* tangents;
	int* orignum;

	VertexArray(const VertexArray& original);
	VertexArray& operator=(VertexArray const &original);
	VertexArray()
	{
		numverts = 0;
	}

	~VertexArray()
	{
		free();
	}

	void alloc(int numv);
	void free();
};

#endif