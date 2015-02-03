



#include "debug.h"
#include "utils.h"

void LastNum(const char* l)
{
	return;

	ofstream last;
	char filepath[MAX_PATH+1];
	FullPath("last.txt", filepath);
	last.open(filepath, ios_base::out);
	last<<l;
	last.flush();
}