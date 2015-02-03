






#include "../utils.h"
#include "../draw/shader.h"
#include "../platform.h"

PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC glShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC glCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC glAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC glLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB = NULL;
PFNGLUNIFORM1IARBPROC glUniform1iARB = NULL;
PFNGLUNIFORM1FARBPROC glUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC glUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC glUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC glUniform4fARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB = NULL;
PFNGLDETACHOBJECTARBPROC glDetachObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC glDeleteObjectARB = NULL;
PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
PFNGLBINDPROGRAMARBPROC glBindProgramARB;
#ifdef PLATFORM_WIN32
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
#endif
PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
PFNGLGETSHADERIVPROC glGetShaderiv;
PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
PFNGLUNIFORM1IPROC glUniform1i;
PFNGLUNIFORM1IVPROC glUniform1iv;
PFNGLUNIFORM2IVPROC glUniform2iv;
PFNGLUNIFORM3IVPROC glUniform3iv;
PFNGLUNIFORM4IVPROC glUniform4iv;
PFNGLUNIFORM1FPROC glUniform1f;
PFNGLUNIFORM1FVPROC glUniform1fv;
PFNGLUNIFORM2FVPROC glUniform2fv;
PFNGLUNIFORM3FVPROC glUniform3fv;
PFNGLUNIFORM4FVPROC glUniform4fv;
PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
PFNGLUNIFORM4FPROC glUniform4f;
PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
PFNGLBINDBUFFERPROC glBindBuffer;
PFNGLBUFFERDATAPROC glBufferData;

Shader g_shader[SHADERS];
int g_curS = 0;

GLint Shader::GetUniform(const char* strVariable)
{
	if(!m_hProgramObject)
		return -1;

	return glGetUniformLocationARB(m_hProgramObject, strVariable);
}

GLint Shader::GetAttrib(const char* strVariable)
{
	if(!m_hProgramObject)
		return -1;

	return glGetAttribLocation(m_hProgramObject, strVariable);
}

void Shader::MapUniform(int slot, const char* variable)
{
	m_slot[slot] = GetUniform(variable);
	//g_log<<"\tmap uniform "<<variable<<" = "<<(int)m_slot[slot]<<endl;
}

void Shader::MapAttrib(int slot, const char* variable)
{
	m_slot[slot] = GetAttrib(variable);
	//g_log<<"\tmap attrib "<<variable<<" = "<<(int)m_slot[slot]<<endl;
}

void GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if( *major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major); // major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor); // minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

	// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION); // ver = "1.50 NVIDIA via Cg compiler"
}

#ifdef PLATFORM_WIN32
PROC GetGLProcAddr(const char* procname)
#else
void (*GetGLProcAddr(const char* procname))()
#endif
{
#ifdef PLATFORM_WIN32
    return wglGetProcAddress(procname);
#endif

#ifdef PLATFORM_LINUX
    return glXGetProcAddress((const GLubyte*)procname);
#endif
}

void InitGLSL()
{
	char *szGLExtensions = (char*)glGetString(GL_EXTENSIONS);

	if(!strstr(szGLExtensions, "GL_ARB_shader_objects"))
	{
		//MessageBox(g_hWnd, TEXT("GL_ARB_shader_objects extension not supported!"), TEXT("Error"), MB_OK);
        ErrorMessage("Error", "GL_ARB_shader_objects extension not supported!");
		return;
	}

	if(!strstr(szGLExtensions, "GL_ARB_shading_language_100"))
	{
		//MessageBox(g_hWnd, TEXT("GL_ARB_shading_language_100 extension not supported!"), TEXT("Error"), MB_OK);
        ErrorMessage("Error", "GL_ARB_shading_language_100 extension not supported!");
		return;
	}

	int major, minor;
	GetGLVersion(&major, &minor);

	if(major < 3 || ( major == 3 && minor < 2 ))
	{
		//MessageBox(g_hWnd, TEXT("OpenGL 3.2 is not supported!"), TEXT("Error"), MB_OK);
        ErrorMessage("Error", "OpenGL 3.2 is not supported!");
	}

	glCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)GetGLProcAddr("glCreateShaderObjectARB");
	glShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)GetGLProcAddr("glShaderSourceARB");
	glCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)GetGLProcAddr("glCompileShaderARB");
	glCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)GetGLProcAddr("glCreateProgramObjectARB");
	glAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)GetGLProcAddr("glAttachObjectARB");
	glLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)GetGLProcAddr("glLinkProgramARB");
	glUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)GetGLProcAddr("glUseProgramObjectARB");
	glUniform1iARB = (PFNGLUNIFORM1IARBPROC)GetGLProcAddr("glUniform1iARB");
	glUniform1fARB = (PFNGLUNIFORM1FARBPROC)GetGLProcAddr("glUniform1fARB");
	glUniform2fARB = (PFNGLUNIFORM2FARBPROC)GetGLProcAddr("glUniform2fARB");
	glUniform3fARB = (PFNGLUNIFORM3FARBPROC)GetGLProcAddr("glUniform3fARB");
	glUniform4fARB = (PFNGLUNIFORM4FARBPROC)GetGLProcAddr("glUniform4fARB");
	glGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)GetGLProcAddr("glGetUniformLocationARB");
	glDetachObjectARB = (PFNGLDETACHOBJECTARBPROC)GetGLProcAddr("glDetachObjectARB");
	glDeleteObjectARB  = (PFNGLDELETEOBJECTARBPROC)GetGLProcAddr("glDeleteObjectARB");
	glProgramLocalParameter4fARB = (PFNGLPROGRAMLOCALPARAMETER4FARBPROC)GetGLProcAddr("glProgramLocalParameter4fARB");
	glBindProgramARB = (PFNGLBINDPROGRAMARBPROC)GetGLProcAddr("glBindProgramARB");
	glGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)GetGLProcAddr("glGenFramebuffersEXT");
	glBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)GetGLProcAddr("glBindFramebufferEXT");
	glFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)GetGLProcAddr("glFramebufferTexture2DEXT");
	glBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)GetGLProcAddr("glBindRenderbufferEXT");
	glGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)GetGLProcAddr("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)GetGLProcAddr("glRenderbufferStorageEXT");
	glFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)GetGLProcAddr("glFramebufferRenderbufferEXT");
	glUniformMatrix4fvARB = (PFNGLUNIFORMMATRIX4FVARBPROC)GetGLProcAddr("glUniformMatrix4fvARB");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)GetGLProcAddr("glVertexAttribPointer");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)GetGLProcAddr("glGetAttribLocation");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)GetGLProcAddr("glGetShaderiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)GetGLProcAddr("glGetShaderInfoLog");
	glUniform1i = (PFNGLUNIFORM1IPROC)GetGLProcAddr("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)GetGLProcAddr("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)GetGLProcAddr("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)GetGLProcAddr("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)GetGLProcAddr("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)GetGLProcAddr("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)GetGLProcAddr("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)GetGLProcAddr("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)GetGLProcAddr("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)GetGLProcAddr("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)GetGLProcAddr("glUniformMatrix4fv");
	glUniform4f = (PFNGLUNIFORM4FPROC)GetGLProcAddr("glUniform4f");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)GetGLProcAddr("glEnableVertexAttribArray");
	glBindBuffer = (PFNGLBINDBUFFERPROC)GetGLProcAddr("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)GetGLProcAddr("glBufferData");

	LoadShader(SHADER_ORTHO, "shaders/ortho.vert", "shaders/ortho.frag");
	LoadShader(SHADER_COLOR2D, "shaders/color2d.vert", "shaders/color2d.frag");
	LoadShader(SHADER_MODEL, "shaders/model.vert", "shaders/model.frag");
	LoadShader(SHADER_COLOR3D, "shaders/color3d.vert", "shaders/color3d.frag");
	LoadShader(SHADER_COLOR3DPERSP, "shaders/color3dpersp.vert", "shaders/color3d.frag");
	LoadShader(SHADER_BILLBOARD, "shaders/billboard.vert", "shaders/billboard.frag");
	LoadShader(SHADER_DEPTH, "shaders/depth.vert", "shaders/depth.frag");
	LoadShader(SHADER_SHADOW, "shaders/shadow.vert", "shaders/shadow.frag");
	LoadShader(SHADER_OWNED, "shaders/owned.vert", "shaders/owned.frag");
	LoadShader(SHADER_MAP, "shaders/map.vert", "shaders/map.frag");
	LoadShader(SHADER_WATER, "shaders/water.vert", "shaders/water.frag");
	LoadShader(SHADER_BILLBOARDPERSP, "shaders/billboardpersp.vert", "shaders/billboard.frag");
}

string LoadTextFile(char* strFile)
{
	ifstream fin(strFile);

	if(!fin)
		return "";

	string strLine = "";
	string strText = "";

	while(getline(fin, strLine))
		strText = strText + "\n" + strLine;

	fin.close();

	return strText;
}

void LoadShader(int shader, char* strVertex, char* strFragment)
{
	Shader* s = &g_shader[shader];
	string strVShader, strFShader;

	if(s->m_hVertexShader || s->m_hFragmentShader || s->m_hProgramObject)
		s->release();

	s->m_hVertexShader = glCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	s->m_hFragmentShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);

	strVShader = LoadTextFile(strVertex);
	strFShader = LoadTextFile(strFragment);

	const char *szVShader = strVShader.c_str();
	const char *szFShader = strFShader.c_str();

	glShaderSourceARB(s->m_hVertexShader, 1, &szVShader, NULL);
	glShaderSourceARB(s->m_hFragmentShader, 1, &szFShader, NULL);

	glCompileShaderARB(s->m_hVertexShader);
	GLint logLength;
	glGetShaderiv(s->m_hVertexShader, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(s->m_hVertexShader, logLength, &logLength, log);
        g_log<<"Shader "<<strVertex<<" compile log: "<<log;
        free(log);
    }

	glCompileShaderARB(s->m_hFragmentShader);
	glGetShaderiv(s->m_hFragmentShader, GL_INFO_LOG_LENGTH, &logLength);
    if(logLength > 0)
    {
        GLchar *log = (GLchar *)malloc(logLength);
        glGetShaderInfoLog(s->m_hFragmentShader, logLength, &logLength, log);
        g_log<<"Shader "<<strFragment<<" compile log: "<<log;
        free(log);
    }

	s->m_hProgramObject = glCreateProgramObjectARB();
	glAttachObjectARB(s->m_hProgramObject, s->m_hVertexShader);
	glAttachObjectARB(s->m_hProgramObject, s->m_hFragmentShader);
	glLinkProgramARB(s->m_hProgramObject);

	//glUseProgramObjectARB(s->m_hProgramObject);

	//g_log<<"shader "<<strVertex<<","<<strFragment<<endl;

    s->MapAttrib(SSLOT_POSITION, "position");
    s->MapAttrib(SSLOT_NORMAL, "normalIn");
    s->MapAttrib(SSLOT_TEXCOORD0, "texCoordIn0");
    s->MapAttrib(SSLOT_TEXCOORD1, "texCoordIn1");
    //s->MapAttrib(SSLOT_TANGENT, "tangent");
	s->MapUniform(SSLOT_SHADOWMAP, "shadowmap");
	s->MapUniform(SSLOT_LIGHTMATRIX, "lightMatrix");
	s->MapUniform(SSLOT_LIGHTPOS, "lightPos");
	//s->MapUniform(SSLOT_LIGHTDIR, "lightDir");
	s->MapUniform(SSLOT_TEXTURE0, "texture0");
	s->MapUniform(SSLOT_TEXTURE1, "texture1");
	s->MapUniform(SSLOT_TEXTURE2, "texture2");
	s->MapUniform(SSLOT_TEXTURE3, "texture3");
	s->MapUniform(SSLOT_NORMALMAP, "normalmap");
	s->MapUniform(SSLOT_SPECULARMAP, "specularmap");
    s->MapUniform(SSLOT_PROJECTION, "projection");
    s->MapUniform(SSLOT_MODELMAT, "model");
    s->MapUniform(SSLOT_VIEWMAT, "view");
    s->MapUniform(SSLOT_MVPMAT, "mvpmat");
	//s->MapUniform(SSLOT_NORMALMAT, "normalMat");
	//s->MapUniform(SSLOT_INVMODLVIEWMAT, "invModelView");
    s->MapUniform(SSLOT_COLOR, "color");
    s->MapUniform(SSLOT_OWNCOLOR, "owncolor");
    s->MapUniform(SSLOT_WIDTH, "width");
    s->MapUniform(SSLOT_HEIGHT, "height");
    s->MapUniform(SSLOT_MIND, "mind");
    s->MapUniform(SSLOT_MAXD, "maxd");
    s->MapUniform(SSLOT_CAMERAPOS, "cameraPos");
    s->MapUniform(SSLOT_SCALE, "scale");
	s->MapUniform(SSLOT_MAXELEV, "maxelev");
	s->MapUniform(SSLOT_SANDONLYMAXY, "sandonlymaxy");
	s->MapUniform(SSLOT_SANDGRASSMAXY, "sandgrassmaxy");
	s->MapUniform(SSLOT_GRASSONLYMAXY, "grassonlymaxy");
	s->MapUniform(SSLOT_GRASSDIRTMAXY, "grassdirtmaxy");
	s->MapUniform(SSLOT_DIRTONLYMAXY, "dirtonlymaxy");
	s->MapUniform(SSLOT_DIRTROCKMAXY, "dirtrockmaxy");
	s->MapUniform(SSLOT_SANDGRAD, "sandgrad");
	s->MapUniform(SSLOT_SANDDET, "sanddet");
	s->MapUniform(SSLOT_GRASSGRAD, "grassgrad");
	s->MapUniform(SSLOT_GRASSDET, "grassdet");
	s->MapUniform(SSLOT_DIRTGRAD, "dirtgrad");
	s->MapUniform(SSLOT_DIRTDET, "dirtdet");
	s->MapUniform(SSLOT_ROCKGRAD, "rockgrad");
	s->MapUniform(SSLOT_ROCKGRAD, "rockdet");
	s->MapUniform(SSLOT_SUNDIRECTION, "sundirection");
	s->MapUniform(SSLOT_GRADIENTTEX, "gradienttex");
	s->MapUniform(SSLOT_DETAILTEX, "detailtex");
	s->MapUniform(SSLOT_OWNERMAP, "ownermap");
}

void UseS(int shader)
{
	g_curS = shader;
	glUseProgramObjectARB(g_shader[shader].m_hProgramObject);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Shader::release()
{
	if(m_hVertexShader)
	{
		glDetachObjectARB(m_hProgramObject, m_hVertexShader);
		glDeleteObjectARB(m_hVertexShader);
		m_hVertexShader = NULL;
	}

	if(m_hFragmentShader)
	{
		glDetachObjectARB(m_hProgramObject, m_hFragmentShader);
		glDeleteObjectARB(m_hFragmentShader);
		m_hFragmentShader = NULL;
	}

	if(m_hProgramObject)
	{
		glDeleteObjectARB(m_hProgramObject);
		m_hProgramObject = NULL;
	}
}

void TurnOffShader()
{
	glUseProgramObjectARB(0);
}

void ReleaseShaders()
{
	for(int i=0; i<SHADERS; i++)
		g_shader[i].release();
}

