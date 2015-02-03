

#ifndef EDITBOX_H
#define EDITBOX_H

#include "widget.h"

class EditBox : public Widget
{
public:
	int m_highl[2];	// highlighted (selected) text

	EditBox(Widget* parent, const char* n, const char* t, int f, Margin left, Margin top, Margin right, Margin bottom, bool pw, int maxl, void (*change2)(int p), int parm);

	void draw();
	//void newover();
	bool lbuttonup(bool moved);
	bool lbuttondown();
	bool prelbuttondown();
	void frameupd();
	bool mousemove();
	bool keydown(int k);
	bool keyup(int k);
	bool charin(int k);
	void placechar(int k);
	void placestr(const char* str);
	string drawvalue();
	void changevalue(const char* str);
};

#endif