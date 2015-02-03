




#include "../platform.h"
#include "font.h"
#include "../texture.h"
#include "../draw/shader.h"
#include "../window.h"
#include "../utils.h"
#include "gui.h"

BitmapFont g_font[FONTS];

static int gx;
static int gy;
static int nextlb;  //next [i] to skip line
static int lastspace;
static int j;
static int x0;
static int i;
static int x;
static int y;
static int w;
static int h;
static int size;
static int line;
static char* str;
static int font;
static int startline;
static int starti;
static int gstartx;
static int goffstartx;
static float frame[4];

void BreakLine()
{
    BitmapFont* f = &g_font[font];
	line++;
	x = goffstartx;
	y += f->gheight;
}

void NextLineBreak()
{
    BitmapFont* f = &g_font[font];
    Glyph* g2;

	if(nextlb != starti)
    {
		BreakLine();
	}

	int lastspace = -1;
	int x0 = gstartx;

	for(int j=i; j<size; j++)
	{
		if(str[j] == '\n')
		{
			nextlb = j+1;
			return;
		}

		g2 = &f->glyph[str[j]];
		x0 += g2->origsize[0];

		if(str[j] == ' ' || str[j] == '\t')
			lastspace = j;

		if(x0 > w+gstartx)
		{
			if(lastspace < 0)
			{
				nextlb = max(j, i+1);

				//if(w <= g2->w)
				//	nextlb++;

				return;
			}

			nextlb = lastspace+1;
			return;
		}
	}
}

void DrawGlyph()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph[str[i]];

	int left = x + g->offset[0];
	int right = left + g->texsize[0];
    int top = y + g->offset[1];
	int bottom = top + g->texsize[1];
	DrawGlyph(left, top, right, bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
}

void DrawGlyphF()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph[str[i]];

	int left = x + g->offset[0];
	int right = left + g->texsize[0];
    int top = y + g->offset[1];
	int bottom = top + g->texsize[1];
	DrawGlyphF(left, top, right, bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
}

void HighlGlyphF()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph[str[i]];

	int left = x;
	int right = x + g->offset[0] + g->texsize[0];
    int top = y;
	int bottom = y + g->offset[1] + g->texsize[1];
	HighlGlyphF(left, top, right, bottom);
}

void DrawCaret()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph['|'];

	int left = x - g->origsize[1]/14;
	int right = left + g->texsize[0];
    int top = y + g->offset[1];
	int bottom = top + g->texsize[1];
	DrawGlyph(left, top, right, bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
}

void DrawCaretF()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph['|'];

	int left = x - g->origsize[1]/14;
	int right = left + g->texsize[0];
    int top = y + g->offset[1];
	int bottom = top + g->texsize[1];
	DrawGlyphF(left, top, right, bottom, g->texcoord[0], g->texcoord[1], g->texcoord[2], g->texcoord[3]);
}

void AdvanceGlyph()
{
    BitmapFont* f = &g_font[font];
    Glyph* g = &f->glyph[str[i]];
    x += g->origsize[0];
}

void StartText(const char* text, int fnt, float width, float height, int ln, int realstartx)
{
	font = fnt;
	str = (char*)text;
	size = strlen(str);
	w = width;
	h = height;
	startline = ln;
	starti = 0;
	gstartx = realstartx;
}

void UseBitmapFontTex()
{
    glActiveTextureARB(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, g_texture[ g_font[font].texindex ].texname);
    glUniform1i(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXTURE0], 0);
}

void StartTextF(const char* text, int fnt, float width, float height, int ln, int realstartx, int framex1, int framey1, int framex2, int framey2)
{
	frame[0] = framex1;
	frame[1] = framey1;
	frame[2] = framex2;
	frame[3] = framey2;
	StartText(text, fnt, width, height, ln, realstartx);
}

void TextLayer(int offstartx, int offstarty)
{
	x = offstartx;
	y = offstarty;
	goffstartx = offstartx;
	nextlb = starti;  //next [i] to skip line
	line = startline;
	i = starti;
}

void FSub(const char* substr)
{
	int subsize = strlen(substr);

	string subsubstr[9];

	int k = 0;
	for(j=0; j<9; j++)
	{
		subsubstr[j] = "";

		for(; k<subsize; k++)
		{
			if(substr[k] == ' ' || substr[k] == '\t')
				break;

			subsubstr[j] += substr[k];
		}

		for(; k<subsize; k++)
		{
			if(substr[k] != ' ' && substr[k] != '\t')
				break;
		}
	}

    unsigned int n = StrToInt(subsubstr[0].c_str());
	BitmapFont* f = &g_font[font];
	Glyph* g = &f->glyph[n];
    g->pixel[0] = StrToInt(subsubstr[1].c_str());
	g->pixel[1] = StrToInt(subsubstr[2].c_str());
    g->texsize[0] = StrToInt(subsubstr[3].c_str());
    g->texsize[1] = StrToInt(subsubstr[4].c_str());
	g->offset[0] = StrToInt(subsubstr[5].c_str());
    g->offset[1] = StrToInt(subsubstr[6].c_str());
    g->origsize[0] = StrToInt(subsubstr[7].c_str());
    g->origsize[1] = StrToInt(subsubstr[8].c_str());
	g->texcoord[0] = (float)g->pixel[0] / f->width;
	g->texcoord[1] = (float)g->pixel[1] / f->height;
	g->texcoord[2] = (float)(g->pixel[0]+g->texsize[0]) / f->width;
	g->texcoord[3] = (float)(g->pixel[1]+g->texsize[1]) / f->height;
}

void LoadBitmapFont(int id, const char* fontname)
{
    BitmapFont* f = &g_font[id];
	char texfile[128];
	strcpy(texfile, fontname);
	FindTextureExtension(texfile);
    CreateTexture(f->texindex, texfile, true);
    f->width = g_texwidth;
    f->height = g_texheight;

    char fontfile[128];
	sprintf(fontfile, "%s.fnt", fontname);
	char fullfontpath[MAX_PATH+1];
	FullPath(fontfile, fullfontpath);
    FILE* fp = fopen(fullfontpath, "rb");
    if(!fp)
    {
        g_log<<"Error loading font "<<fontfile<<endl;
        return;
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    rewind(fp);

    char* file = new char[size];
    fread(file, 1, size, fp);
    fclose(fp);

    //NSLog(@"%s", file);

    string substr;
	//str = file;

	font = id;

        //skip 2 lines
    for(i=0; i<size; i++)
	{
		if(file[i] == '\n')
			break;
	}
	i++;
    for(; i<size; i++)
	{
		if(file[i] == '\n')
			break;
	}
	i++;

	do
	{
		substr = "";

		for(; i<size; i++)
		{
			if(file[i] == '\n')
				break;

			substr += file[i];
		}

		i++;

		if(substr.length() > 9)
			FSub(substr.c_str());
	}while(i<size);

	f->gheight = f->glyph['A'].origsize[1];

    delete [] file;
    g_log<<fontfile<<".fnt"<<endl;
}

void DrawGlyph(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom)
{
    float vertices[] =
    {
        //posx, posy    texx, texy
        left, top,0,          texleft, textop,
        right, top,0,         texright, textop,
        right, bottom,0,      texright, texbottom,

        right, bottom,0,      texright, texbottom,
        left, bottom,0,       texleft, texbottom,
        left, top,0,          texleft, textop
    };

	//g_log<<"draw glyph: "<<texleft<<","<<textop<<","<<texright<<","<<texbottom<<endl;

    //glVertexAttribPointer(g_slots[SHADER_ORTHO][POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);
    //glVertexAttribPointer(g_slots[SHADER_ORTHO][TEXCOORD], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[3]);

	//glVertexPointer(3, GL_FLOAT, sizeof(float)*5, &vertices[0]);
	//glTexCoordPointer(2, GL_FLOAT, sizeof(float)*5, &vertices[3]);

    glVertexAttribPointer(g_shader[SHADER_ORTHO].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);
    glVertexAttribPointer(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[3]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawGlyphF(float left, float top, float right, float bottom, float texleft, float textop, float texright, float texbottom)
{
	float newleft = left;
	float newtop = top;
	float newright = right;
	float newbottom = bottom;
	float newtexleft = texleft;
	float newtextop = textop;
	float newtexright = texright;
	float newtexbottom = texbottom;

	if(newleft < frame[0])
	{
		newtexleft = (frame[0]-newleft)*(texright-texleft)/(right-left);
		newleft = frame[0];
	}
	else if(newleft > frame[2])
	{
		newtexleft = (frame[2]-newleft)*(texright-texleft)/(right-left);
		newleft = frame[2];
	}

	if(newright < frame[0])
	{
		newtexleft = (frame[0]-newright)*(texright-texleft)/(right-left);
		newright = frame[0];
	}
	else if(newright > frame[2])
	{
		newtexright = (frame[2]-newright)*(texright-texleft)/(right-left);
		newright = frame[2];
	}

	if(newtop < frame[1])
	{
		newtextop = (frame[1]-newtop)*(texbottom-textop)/(bottom-top);
		newtop = frame[1];
	}
	else if(newtop > frame[3])
	{
		newtextop = (frame[3]-newtop)*(texbottom-textop)/(bottom-top);
		newtop = frame[3];
	}

	if(newbottom < frame[1])
	{
		newtexbottom = (frame[1]-newbottom)*(texbottom-textop)/(bottom-top);
		newbottom = frame[1];
	}
	else if(newbottom > frame[3])
	{
		newtexbottom = (frame[3]-newbottom)*(texbottom-textop)/(bottom-top);
		newbottom = frame[3];
	}

    float vertices[] =
    {
        //posx, posy    texx, texy
        newleft, newtop,0,          newtexleft, newtextop,
        newright, newtop,0,         newtexright, newtextop,
        newright, newbottom,0,      newtexright, newtexbottom,

        newright, newbottom,0,      newtexright, newtexbottom,
        newleft, newbottom,0,       newtexleft, newtexbottom,
        newleft, newtop,0,          newtexleft, newtextop
    };

    glVertexAttribPointer(g_shader[SHADER_ORTHO].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[0]);
    glVertexAttribPointer(g_shader[SHADER_ORTHO].m_slot[SSLOT_TEXCOORD0], 2, GL_FLOAT, GL_FALSE, sizeof(float)*5, &vertices[3]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void HighlGlyphF(float left, float top, float right, float bottom)
{
	float newleft = left;
	float newtop = top;
	float newright = right;
	float newbottom = bottom;

	if(newleft < frame[0])
		newleft = frame[0];
	else if(newleft > frame[2])
		newleft = frame[2];

	if(newright < frame[0])
		newright = frame[0];
	else if(newright > frame[2])
		newright = frame[2];

	if(newtop < frame[1])
		newtop = frame[1];
	else if(newtop > frame[3])
		newtop = frame[3];

	if(newbottom < frame[1])
		newbottom = frame[1];
	else if(newbottom > frame[3])
		newbottom = frame[3];

    float vertices[] =
    {
        //posx, posy
        newleft, newtop,0,
        newright, newtop,0,
        newright, newbottom,0,

        newright, newbottom,0,
        newleft, newbottom,0,
        newleft, newtop,0,
    };

    glVertexAttribPointer(g_shader[SHADER_ORTHO].m_slot[SSLOT_POSITION], 3, GL_FLOAT, GL_FALSE, 0, &vertices[0]);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawLine(int caret)
{
	if(caret == 0)
		DrawCaret();
	for(; i<size; i++)
    {
		if(caret == i)
			DrawCaret();

		DrawGlyph();
        AdvanceGlyph();
    }
	if(caret == size)
		DrawCaret();
}

void DrawLineF(int caret)
{
	if(caret == 0)
		DrawCaretF();
	for(; i<size; i++)
    {
		if(caret == i)
			DrawCaretF();

		DrawGlyphF();
        AdvanceGlyph();
    }
	if(caret == size)
		DrawCaretF();
}

void DrawLine(int fnt, float startx, float starty, const char* text, const float* color, int caret)
{
	StartText(text, fnt, g_currw*2, g_currh*2, 0, startx);
	UseBitmapFontTex();
	TextLayer(startx, starty);
    DrawLine(caret);
}

void DrawShadowedText(int fnt, float startx, float starty, const char* text, const float* color, int caret)
{
    //glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0, 0, 0, 1);
    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.0f, 0.0f, 0.0f, color != NULL ? color[3] : 1);
	//glColor4f(0, 0, 0, 1);

	StartText(text, fnt, g_currw*2, g_currh*2, 0, startx);
	UseBitmapFontTex();
	TextLayer(startx+1, starty);
    DrawLine(caret);
	TextLayer(startx, starty+1);
    DrawLine(caret);
	TextLayer(startx+1, starty+1);
    DrawLine(caret);

    if(color == NULL)
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		//glColor4f(1, 1, 1, 1);
    else
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		//glColor4f(color[0], color[1], color[2], color[3]);

	TextLayer(startx, starty);
    DrawLine(caret);

    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
	//glColor4f(1, 1, 1, 1);
}


void DrawShadowedTextF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const char* text, const float* color, int caret)
{
    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.0f, 0.0f, 0.0f, color != NULL ? color[3] : 1);

	StartTextF(text, fnt, g_currw*2, g_currh*2, 0, startx, framex1, framey1, framex2, framey2);
	UseBitmapFontTex();
	TextLayer(startx+1, starty);
    DrawLineF(caret);
	TextLayer(startx, starty+1);
    DrawLineF(caret);
	TextLayer(startx+1, starty+1);
    DrawLineF(caret);

    if(color == NULL)
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
		//glColor4f(1, 1, 1, 1);
    else
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);
		//glColor4f(color[0], color[1], color[2], color[3]);

	TextLayer(startx, starty);
    DrawLineF(caret);

    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void HighlightF(int fnt, float startx, float starty, float framex1, float framey1, float framex2, float framey2, const char* text, int starti, int endi)
{
	UseS(SHADER_COLOR2D);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_WIDTH], (float)g_currw);
    glUniform1f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_HEIGHT], (float)g_currh);
	glUniform4f(g_shader[SHADER_COLOR2D].m_slot[SSLOT_COLOR], 1, 1, 1, 0.5f);

	StartTextF(text, fnt, g_currw*2, g_currh*2, 0, startx, framex1, framey1, framex2, framey2);

	TextLayer(startx, starty);

	for(i=0; i<starti; i++)
        AdvanceGlyph();

	for(; i<endi; i++)
    {
		HighlGlyphF();
        AdvanceGlyph();
    }

	Ortho(g_currw, g_currh, 1, 1, 1, 1);
}

void DrawCenterShadText(int fnt, float startx, float starty, const char* text, const float* color, int caret)
{
	float linew = 0;
	int len = strlen(text);
	for(int k=0; k<len; k++)
		linew += g_font[font].glyph[ text[k] ].origsize[0];

	startx -= linew/2;

	float a = 1;
	if(color != NULL)
		a = color[3];

    //glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0, 0, 0, a);
    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.0f, 0.0f, 0.0f, color != NULL ? color[3] : 1);

	StartText(text, fnt, g_currw*2, g_currh*2, 0, startx);
	UseBitmapFontTex();
	TextLayer(startx+1, starty);
    DrawLine(caret);
	TextLayer(startx, starty+1);
    DrawLine(caret);
	TextLayer(startx+1, starty+1);
    for(; i<size; i++)
    DrawLine(caret);

    if(color == NULL)
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
    else
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);

	TextLayer(startx, starty);
    for(; i<size; i++)
    {
		DrawGlyph();
        AdvanceGlyph();
    }

    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
}

void DrawBoxShadText(int fnt, float startx, float starty, float width, float height, const char* text, const float* color, int ln, int caret)
{
    //glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0, 0, 0, 1);
    glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 0.3f, 0.3f, 0.3f, 1);

	StartText(text, fnt, width, height, ln, startx);
	UseBitmapFontTex();
	TextLayer(startx+1, starty);
	if(caret == 0)
		DrawCaret();
    for(; i<size; i++)
    {
        if(i == nextlb)
            NextLineBreak();

		if(caret == i)
			DrawCaret();

        DrawGlyph();
        AdvanceGlyph();
    }
	if(caret == size)
	{
		if(str[size-1] == '\n')
			BreakLine();
		DrawCaret();
	}

	TextLayer(startx, starty+1);
	if(caret == 0)
		DrawCaret();
    for(; i<size; i++)
    {
        if(i == nextlb)
            NextLineBreak();

		if(caret == i)
			DrawCaret();

        DrawGlyph();
        AdvanceGlyph();
    }
	if(caret == size)
	{
		if(str[size-1] == '\n')
			BreakLine();
		DrawCaret();
	}

	TextLayer(startx+1, starty+1);
	if(caret == 0)
		DrawCaret();
    for(; i<size; i++)
    {
        if(i == nextlb)
            NextLineBreak();

		if(caret == i)
			DrawCaret();

        DrawGlyph();
        AdvanceGlyph();
    }
	if(caret == size)
	{
		if(str[size-1] == '\n')
			BreakLine();
		DrawCaret();
	}

    if(color == NULL)
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], 1, 1, 1, 1);
    else
        glUniform4f(g_shader[SHADER_ORTHO].m_slot[SSLOT_COLOR], color[0], color[1], color[2], color[3]);

	TextLayer(startx, starty);
	if(caret == 0)
		DrawCaret();
    for(; i<size; i++)
    {
        if(i == nextlb)
            NextLineBreak();

		if(caret == i)
			DrawCaret();

        DrawGlyph();
        AdvanceGlyph();
    }
	if(caret == size)
	{
		if(str[size-1] == '\n')
			BreakLine();
		DrawCaret();
	}
}

int CountLines(const char* text, int fnt, float startx, float starty, float width, float height)
{
	StartText(text, fnt, width, height, 0, startx);
	TextLayer(startx, starty);

    for(i=0; i<size; i++)
    {
        if(i == nextlb)
            NextLineBreak();

		AdvanceGlyph();
    }
	if(str[size-1] == '\n')
		BreakLine();

	return line+1;
}

int EndX(const char* text, int lastc, int fnt, float startx, float starty)
{
	StartText(text, fnt, g_currw*100, g_currh*100, 0, startx);
	TextLayer(startx, starty);

	//g_log<<"size = "<<size<<endl;
	//g_log<<"lastc = "<<lastc<<endl;

    for(i=0; i<size && i<lastc; i++)
    {
		//g_log<<"str[i] = "<<str[i]<<endl;
		AdvanceGlyph();
    }

	return x;
}

int MatchGlyphF(const char* text, int fnt, int matchx, float startx, float starty, float framex1, float framey1, float framex2, float framey2)
{
	int lastclose = 0;

	StartTextF(text, fnt, g_currw*2, g_currh*2, 0, startx, framex1, framey1, framex2, framey2);
	TextLayer(startx, starty);

	if(x >= matchx)
		return lastclose;

	for(i=0; i<size && x <= framex2; i++)
    {
		AdvanceGlyph();

		lastclose = i;

		if(x >= matchx)
			return lastclose;
    }

	return lastclose+1;
}

void LoadBitmapFonts()
{
    LoadBitmapFont(FONT_CORBEL28, "fonts/corbel28");
    LoadBitmapFont(FONT_EUROSTILE16, "fonts/eurostile16");
    LoadBitmapFont(FONT_EUROSTILE32, "fonts/eurostile32");
    LoadBitmapFont(FONT_GULIM10, "fonts/gulim10");
    LoadBitmapFont(FONT_GULIM16, "fonts/gulim16");
    LoadBitmapFont(FONT_GULIM32, "fonts/gulim32");
    LoadBitmapFont(FONT_MSUIGOTHIC10, "fonts/msuigothic10");
    LoadBitmapFont(FONT_MSUIGOTHIC16, "fonts/msuigothic16");
    LoadBitmapFont(FONT_SMALLFONTS10, "fonts/smallfonts10");
}
