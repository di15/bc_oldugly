

#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "widget.h"

class CheckBox : public Widget
{
public:
	CheckBox(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, int sel=0, float r=1, float g=1, float b=1, float a=1, void (*change)()=NULL);

	void draw();
	bool lbuttonup(bool moved);
	bool lbuttondown();
	bool mousemove();
	int square();
};

#endif