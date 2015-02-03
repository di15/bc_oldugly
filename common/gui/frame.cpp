

#include "widget.h"
#include "barbutton.h"
#include "button.h"
#include "checkbox.h"
#include "editbox.h"
#include "dropdowns.h"
#include "hscroller.h"
#include "image.h"
#include "insdraw.h"
#include "link.h"
#include "listbox.h"
#include "text.h"
#include "textarea.h"
#include "textblock.h"
#include "touchlistener.h"
#include "frame.h"
#include "../platform.h"

Frame::Frame(Widget* parent, const char* n, Margin left, Margin top, Margin right, Margin bottom)
{
	m_parent = parent;
	m_type = WIDGET_FRAME;
	m_name = n;
	m_pos[0] = left;
	m_pos[1] = top;
    m_pos[2] = right;
	m_pos[3] = bottom;
	m_ldown = false;
	reframe();
}

void Frame::draw()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->draw();
}

void Frame::draw2()
{
	for(auto i=m_subwidg.begin(); i!=m_subwidg.end(); i++)
		(*i)->draw2();
}

bool Frame::prelbuttonup(bool moved)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->prelbuttonup(moved))
			return true;

	return false;
}

bool Frame::lbuttonup(bool moved)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->lbuttonup(moved))
			return true;

	return false;
}

bool Frame::prelbuttondown()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->prelbuttondown())
			return true;

	return false;
}

bool Frame::lbuttondown()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->lbuttondown())
			return true;

	return false;
}

void Frame::premousemove()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->premousemove();
}

bool Frame::mousemove()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->mousemove())
			return true;

	return false;
}

bool Frame::prerbuttonup(bool moved)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->prerbuttonup(moved))
			return true;

	return false;
}

bool Frame::rbuttonup(bool moved)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->rbuttonup(moved))
			return true;

	return false;
}

bool Frame::prerbuttondown()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->prerbuttondown())
			return true;

	return false;
}

bool Frame::rbuttondown()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->rbuttondown())
			return true;

	return false;
}

bool Frame::mousewheel(int delta) 
{ 
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->mousewheel(delta))
			return true;

	return false; 
}

void Frame::frameupd()
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		(*i)->frameupd();
}

bool Frame::keyup(int k)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->keyup(k))
			return true;

	return false;
}

bool Frame::keydown(int k)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->keydown(k))
			return true;

	return false;
}

bool Frame::charin(int k)
{
	for(auto i=m_subwidg.rbegin(); i!=m_subwidg.rend(); i++)
		if((*i)->charin(k))
			return true;

	return false;
}