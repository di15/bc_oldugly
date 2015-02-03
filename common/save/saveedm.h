


#include "../platform.h"

#define EDMAP_VERSION		2.0f

#define TAG_EDMAP		{'D', 'M', 'D', 'B', 'M'}	//I forgot what the B stands for

class EdMap;
class TexRef;
class Brush;

void ReadBrush(FILE* fp, TexRef* texrefs, Brush* b);
void SaveBrush(FILE* fp, int* texrefs, Brush* b);
void SaveEdMap(const char* fullpath, EdMap* map);
bool LoadEdMap(const char* fullpath, EdMap* map);
void FreeEdMap(EdMap* map);