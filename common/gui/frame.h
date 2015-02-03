

#ifndef FRAME_H
#define FRAME_H

#include "widget.h"

class Frame : public Widget
{
public:
	Frame(Widget* parent, const char* n, Margin left, Margin top, Margin right, Margin bottom);

	void draw();
	void draw2();
	bool prelbuttonup(bool moved);
	bool lbuttonup(bool moved);
	bool prelbuttondown();
	bool lbuttondown();
	void premousemove();
	bool mousemove();
	bool prerbuttonup(bool moved);
	bool rbuttonup(bool moved);
	bool prerbuttondown();
	bool rbuttondown();
	bool keyup(int k);
	bool keydown(int k);
	bool charin(int k);
	bool mousewheel(int delta);
	void frameupd();
};

#endif