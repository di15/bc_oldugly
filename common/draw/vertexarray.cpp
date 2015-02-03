

#include "vertexarray.h"


VertexArray::VertexArray(const VertexArray& original)
{
	//g_log<<"vertex array copy constructor"<<endl;
	/*
	alloc(original.numverts);
	memcpy(vertices, original.vertices, sizeof(Vec3f)*numverts);
	memcpy(texcoords, original.texcoords, sizeof(Vec2f)*numverts);
	memcpy(normals, original.normals, sizeof(Vec3f)*numverts);*/

	*this = original;
}


VertexArray& VertexArray::operator=(VertexArray const &original)
{
	//g_log<<"vertex array assignment op"<<endl;

	alloc(original.numverts);
	memcpy(vertices, original.vertices, sizeof(Vec3f)*numverts);
	memcpy(texcoords, original.texcoords, sizeof(Vec2f)*numverts);
	memcpy(normals, original.normals, sizeof(Vec3f)*numverts);
	//memcpy(tangents, original.tangents, sizeof(Vec3f)*numverts);

	return *this;
}

void VertexArray::alloc(int numv)
{
	free();
	numverts = numv;
	vertices = new Vec3f[numv];
	texcoords = new Vec2f[numv];
	normals = new Vec3f[numv];
	//tangents = new Vec3f[numv];
	orignum = new int[numv];
}

void VertexArray::free()
{
	if(numverts <= 0)
		return;

	delete [] vertices;
	delete [] texcoords;
	delete [] normals;
	//delete [] tangents;
	delete [] orignum;
	numverts = 0;
}