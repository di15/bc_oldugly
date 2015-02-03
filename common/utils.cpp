

#include "utils.h"
#include "platform.h"

#ifndef PLATFORM_WIN32
static long long g_starttick = 0;
#endif

ofstream g_log;

const string DateTime()
{
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://www.cplusplus.com/reference/clibrary/ctime/strftime/
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}

void OpenLog(const char* filename, float version)
{
	char fullpath[MAX_PATH+1];
	FullPath(filename, fullpath);
	g_log.open(fullpath, ios_base::out);
	g_log<<DateTime()<<endl;
	g_log<<"Version "<<version<<endl<<endl;
	g_log.flush();
}

string MakePathRelative(const char* full)
{
	char full2c[MAX_PATH+1];
	strcpy(full2c, full);
	CorrectSlashes(full2c);
	string full2(full2c);
	char exepath[MAX_PATH+1];
	ExePath(exepath);
	CorrectSlashes(exepath);

	//g_log<<"exepath: "<<exepath<<endl;
	//g_log<<"fulpath: "<<full<<endl;

	string::size_type pos = full2.find(exepath, 0);

	if(pos == string::npos)
	{
		return full2;
	}

	//g_log<<"posposp: "<<pos<<endl;

	string sub = string( full2 ).substr(strlen(exepath)+1, strlen(full)-strlen(exepath)-1);

	//g_log<<"subpath: "<<sub<<endl;

    return sub;
}

void ExePath(char* exepath)
{
#ifdef PLATFORM_WIN32
    char buffer[MAX_PATH+1];
    GetModuleFileName(NULL, buffer, MAX_PATH);
#else
    char pidbuffer[MAX_PATH+1];
    char buffer[MAX_PATH+1];
    //struct stat info;
    pid_t pid = getpid();
    sprintf(pidbuffer, "/proc/%d/exe", pid);
    if (readlink(pidbuffer, buffer, MAX_PATH) == -1)
        ErrorMessage("Error", "Error getting exe path");
#endif
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    string strexepath = string( buffer ).substr( 0, pos);
	strcpy(exepath, strexepath.c_str());
}

string StripFile(string filepath)
{
	int lastof = filepath.find_last_of("/\\");
	if(lastof < 0)
		lastof = strlen(filepath.c_str());
	else
		lastof += 1;

	string stripped = filepath.substr(0, lastof);
	return stripped;
}

void StripPath(char* filepath)
{
	string s0(filepath);
	size_t sep = s0.find_last_of("\\/");
	string s1;

    if (sep != std::string::npos)
        s1 = s0.substr(sep + 1, s0.size() - sep - 1);
	else
		s1 = s0;

	strcpy(filepath, s1.c_str());
}

void StripExtension(char* filepath)
{
	string s1(filepath);

	size_t dot = s1.find_last_of(".");
	string s2;

	if (dot != std::string::npos)
		s2 = s1.substr(0, dot);
	else
		s2 = s1;

	strcpy(filepath, s2.c_str());
}

void StripPathExtension(const char* n, char* o)
{
	string s0(n);
	size_t sep = s0.find_last_of("\\/");
	string s1;

    if (sep != std::string::npos)
        s1 = s0.substr(sep + 1, s0.size() - sep - 1);
	else
		s1 = s0;

	size_t dot = s1.find_last_of(".");
	string s2;

	if (dot != std::string::npos)
		s2 = s1.substr(0, dot);
	else
		s2 = s1;

	strcpy(o, s2.c_str());
}

void FullPath(const char* filename, char* full)
{
	//char exepath[MAX_PATH+1];
	//GetModuleFileName(NULL, exepath, MAX_PATH);
	//string path = StripFile(exepath);

	//char full[MAX_PATH+1];
	//sprintf(full, "%s", path.c_str());

	ExePath(full);

	char c = full[ strlen(full)-1 ];
	if(c != '\\' && c != '/')
		strcat(full, "\\");
		//strcat(full, "/");

	strcat(full, filename);
	CorrectSlashes(full);
}

float StrToFloat(const char *s)
{
	if(s[0] == '\0')
		return 1.0f;

    float x;
    istringstream iss(s);
    iss >> x;

	if(_isnan(x))
		x = 1.0f;

    return x;
}

int HexToInt(const char* s)
{
    int x;
    stringstream ss;
    ss << std::hex << s;
    ss >> x;
    return x;
}

int StrToInt(const char *s)
{
    int x;
    istringstream iss(s);
    iss >> x;
    return x;
}

void CorrectSlashes(char* corrected)
{
	int strl = strlen(corrected);
	for(int i=0; i<strl; i++)
		if(corrected[i] == '/')
			corrected[i] = '\\';
}

void BackSlashes(char* corrected)
{
	int strl = strlen(corrected);
	for(int i=0; i<strl; i++)
		if(corrected[i] == '/')
			corrected[i] = '\\';
}

#ifndef PLATFORM_WIN32
long long timeGetTime()
{
        struct timeval now;
        gettimeofday(&now, NULL);
        return (long long)now.tv_sec * 1000 + now.tv_usec/1000;
}

long long GetTickCount64()
{
        return timeGetTime();
}

unsigned int GetTickCount()
{
    if(g_starttick == 0LL)
        g_starttick = GetTickCount64();

    return (unsigned int)(GetTickCount64() - g_starttick);
}

void Sleep(int ms)
{
    usleep(ms*1000);
}

bool ListFiles(const char* full, list<string> &files)
{
    DIR *dir;

    dir = opendir (full);

    if(!dir)
        return false;

    struct dirent* entity = readdir(dir);

    if(entity == NULL)
        return false;

    while(entity != NULL)
    {
        if(entity->d_type == DT_REG)        //regular file
                {
                        files.push_back(string(entity->d_name));
                }

        entity = readdir(dir);
    }

    closedir(dir);

    return true;
}

bool ListDirs(const char* full, list<string> &subdirs)
{
        DIR *dir;

        dir = opendir (full);

        if(!dir)
                return false;

        struct dirent* entity = readdir(dir);

        if(entity == NULL)
                return false;

        while(entity != NULL)
    {
        if(entity->d_type == DT_DIR)        //it's an direcotry
                {
                        //don't process the '..' and the '.' directories
                        if(entity->d_name[0] != '.')
                        {
                                subdirs.push_back(string(entity->d_name));
                        }
                }

        entity = readdir(dir);
    }

        closedir(dir);

        return true;
}

void Hash(char* in, char* out)
{
        int msg_len = strlen(in);
        int hash_len = gcry_md_get_algo_dlen(GCRY_MD_SHA1);
        unsigned char hash[ hash_len ];

        // output sha1 hash - converted to hex representation
        // 2 hex digits for every byte + 1 for trailing \0
        //char* out = (char *) malloc( sizeof(char) * ((hash_len*2)+1) );
        char* p = out;

        gcry_md_hash_buffer(GCRY_MD_SHA1, hash, in, msg_len);

        for(int i=0; i<hash_len; i++, p+=2)
                snprintf(p, 3, "%02x", hash[i]);
}
#endif

void ErrorMessage(const char* title, const char* message)
{
#ifdef PLATFORM_WIN32
    MessageBox(g_hWnd, message, title, MB_OK);
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, title, message, NULL);
#endif
}

void InfoMessage(const char* title, const char* message)
{
#ifdef PLATFORM_WIN32
		MessageBox(g_hWnd, message, title, MB_OK);
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, title, message, NULL);
#endif
}

void WarningMessage(const char* title, const char* message)
{
#ifdef PLATFORM_WIN32
		MessageBox(g_hWnd, message, title, MB_OK);
#else
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, title, message, NULL);
#endif
}
