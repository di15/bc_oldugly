


#ifndef FONT_H
#define FONT_H

#include "../platform.h"

using namespace std;

#define MAX_CHARS	256
#define CODE_POINTS	110000

class Glyph
{
public:
    int pixel[2];
	int texsize[2];
    int offset[2];
	int origsize[2];
	float texcoord[4];

    Glyph()
    {
        pixel[0] = 0;
		pixel[1] = 0;
		texsize[0] = 0;
		texsize[1] = 0;
		offset[0] = 0;
		offset[1] = 0;
		origsize[0] = 0;
		origsize[1] = 0;
		texcoord[0] = 0;
		texcoord[1] = 0;
		texcoord[2] = 0;
		texcoord[3] = 0;
    }
};

class BitmapFont
{
public:
    unsigned int texindex;
    float width;    //image width
    float height;   //image height
    float gheight;  //glyph height
    Glyph glyph[CODE_POINTS];

	BitmapFont()
	{
		width = 0;
		height = 0;
		gheight = 0;
	}
};

#define FONT_CORBEL28		0
#define FONT_EUROSTILE16	1
#define FONT_EUROSTILE32	2
#define FONT_GULIM10		3
#define FONT_GULIM16		4
#define FONT_GULIM32		5
#define FONT_MSUIGOTHIC10	6
#define FONT_MSUIGOTHIC16	7
#define FONT_SMALLFONTS10	8
#define FONTS				9
extern BitmapFont g_font[FONTS];

#define MAINFONT8 (FONT_SMALLFONTS10)
#define MAINFONT16 (FONT_MSUIGOTHIC16)
//#define MAINFONT32 (FONT::GULIM32)
//#define MAINFONT32 (FONT::CORBEL28)
//#define MAINFONT32 (UNSTEADYOVERSTEER20)
#define MAINFONT32 (FONT_CORBEL28)

void LoadBitmapFonts();
void DrawGlyph(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom);
void DrawGlyphF(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom);
void HighlGlyphF(float left, float top, float right, float bottom);
void DrawLine(int fnt, float startx, float starty, const char* text, const float* color=NULL, int caret=-1);
void DrawShadowedText(int fnt, float startx, float starty, const char* text, const float* color=NULL, int caret=-1);
void DrawShadowedTextF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const char* text, const float* color=NULL, int caret=-1);
void DrawCenterShadText(int fnt, float startx, float starty, const char* text, const float* color=NULL, int caret=-1);
void DrawBoxShadText(int fnt, float startx, float starty, float width, float height, const char* text, const float* color, int ln, int caret);
int CountLines(const char* text, int fnt, float startx, float starty, float width, float height);
int EndX(const char* text, int lastc, int fnt, float startx, float starty);
int MatchGlyphF(const char* text, int fnt, int matchx, float startx, float starty, float framex1, float framey1, float framex2, float framey2);
void HighlightF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const char* text, int starti, int endi);
void NextLineBreak();
void AdvanceGlyph();

#endif
