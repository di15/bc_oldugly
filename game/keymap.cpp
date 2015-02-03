




#include "gmain.h"
#include "keymap.h"
#include "../common/math/3dmath.h"
#include "ggui.h"

void MapKeys()
{	
	//AssignKey(VK_ESCAPE, &Escape, NULL);
	//AssignMouseWheel(&MouseWheel);
	//AssignMouseMove(&MouseMove);
	//AssignLButton(&MouseLeftButtonDown, &MouseLeftButtonUp);
	//AssignRButton(NULL, &MouseRightButtonUp);
	
	/*
	int key;
	void (*down)();
	void (*up)();
	ifstream f("keybldg.ini");
	string line;
	char keystr[32];
	char actstr[32];

	while(!f.eof())
	{
		key = -1;
		down = NULL;
		up = NULL;
		strcpy(keystr, "");
		strcpy(actstr, "");

		getline(f, line);
		sscanf(line.c_str(), "%s %s", keystr, actstr);

		if(_stricmp(keystr, "VK_ESCAPE") == 0)			key = VK_ESCAPE;
		else if(_stricmp(keystr, "VK_SHIFT") == 0)		key = VK_SHIFT;
		else if(_stricmp(keystr, "VK_CONTROL") == 0)		key = VK_CONTROL;
		else if(_stricmp(keystr, "VK_SPACE") == 0)		key = VK_SPACE;
		else if(_stricmp(keystr, "MouseLButton") == 0)	key = -2;
		else if(_stricmp(keystr, "F1") == 0)				key = VK_F1;
		else if(_stricmp(keystr, "F2") == 0)				key = VK_F2;
		else if(_stricmp(keystr, "F3") == 0)				key = VK_F3;
		else if(_stricmp(keystr, "F4") == 0)				key = VK_F4;
		else if(_stricmp(keystr, "F5") == 0)				key = VK_F5;
		else if(_stricmp(keystr, "F6") == 0)				key = VK_F6;
		else if(_stricmp(keystr, "F7") == 0)				key = VK_F7;
		else if(_stricmp(keystr, "F8") == 0)				key = VK_F8;
		else if(_stricmp(keystr, "F9") == 0)				key = VK_F9;
		else if(_stricmp(keystr, "F10") == 0)			key = VK_F10;
		else if(_stricmp(keystr, "F11") == 0)			key = VK_F11;
		else if(_stricmp(keystr, "F12") == 0)			key = VK_F12;
		else if(_stricmp(keystr, "'A'") == 0)			key = 'A';
		else if(_stricmp(keystr, "'B'") == 0)			key = 'B';
		else if(_stricmp(keystr, "'C'") == 0)			key = 'C';
		else if(_stricmp(keystr, "'D'") == 0)			key = 'D';
		else if(_stricmp(keystr, "'E'") == 0)			key = 'E';
		else if(_stricmp(keystr, "'F'") == 0)			key = 'F';
		else if(_stricmp(keystr, "'G'") == 0)			key = 'G';
		else if(_stricmp(keystr, "'H'") == 0)			key = 'H';
		else if(_stricmp(keystr, "'I'") == 0)			key = 'I';
		else if(_stricmp(keystr, "'J'") == 0)			key = 'J';
		else if(_stricmp(keystr, "'K'") == 0)			key = 'K';
		else if(_stricmp(keystr, "'L'") == 0)			key = 'L';
		else if(_stricmp(keystr, "'M'") == 0)			key = 'M';
		else if(_stricmp(keystr, "'N'") == 0)			key = 'N';
		else if(_stricmp(keystr, "'O'") == 0)			key = 'O';
		else if(_stricmp(keystr, "'P'") == 0)			key = 'P';
		else if(_stricmp(keystr, "'Q'") == 0)			key = 'Q';
		else if(_stricmp(keystr, "'R'") == 0)			key = 'R';
		else if(_stricmp(keystr, "'S'") == 0)			key = 'S';
		else if(_stricmp(keystr, "'T'") == 0)			key = 'T';
		else if(_stricmp(keystr, "'U'") == 0)			key = 'U';
		else if(_stricmp(keystr, "'V'") == 0)			key = 'V';
		else if(_stricmp(keystr, "'W'") == 0)			key = 'W';
		else if(_stricmp(keystr, "'X'") == 0)			key = 'X';
		else if(_stricmp(keystr, "'Y'") == 0)			key = 'Y';
		else if(_stricmp(keystr, "'Z'") == 0)			key = 'Z';
		else if(_stricmp(keystr, "'0'") == 0)			key = '0';
		else if(_stricmp(keystr, "'1'") == 0)			key = '1';
		else if(_stricmp(keystr, "'2'") == 0)			key = '2';
		else if(_stricmp(keystr, "'3'") == 0)			key = '3';
		else if(_stricmp(keystr, "'4'") == 0)			key = '4';
		else if(_stricmp(keystr, "'5'") == 0)			key = '5';
		else if(_stricmp(keystr, "'6'") == 0)			key = '6';
		else if(_stricmp(keystr, "'7'") == 0)			key = '7';
		else if(_stricmp(keystr, "'8'") == 0)			key = '8';
		else if(_stricmp(keystr, "'9'") == 0)			key = '9';

		if(key == -1)
		{
			g_log<<"Unknown input: "<<keystr<<endl;
			continue;
		}
		
		if(_stricmp(actstr, "Escape();") == 0)				{	down = &Escape;			up = NULL;			}
		else if(_stricmp(actstr, "Forward();") == 0)			{	down = &Forward;		up = NULL;			}
		else if(_stricmp(actstr, "Left();") == 0)			{	down = &Left;			up = NULL;			}
		else if(_stricmp(actstr, "Right();") == 0)			{	down = &Right;			up = NULL;			}
		else if(_stricmp(actstr, "Back();") == 0)			{	down = &Back;			up = NULL;			}

		if(down == NULL)		g_log<<"Unknown action: "<<actstr<<endl;
		else if(key == -2)		AssignLButton(down, up);
		else					AssignKey(key, down, up);
	}
	*/
}

