

#ifndef VEC2I_H
#define VEC2I_H

class Vec2i
{
public:
	int x, y;

	Vec2i() 
	{
		x = y = 0;
	}

	Vec2i(int X, int Y)
	{
		x = X; y = Y;
	}
};

#endif