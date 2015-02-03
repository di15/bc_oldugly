



#ifndef SHADER_H
#define SHADER_H

#include "../platform.h"

#define APIENTRYP APIENTRY *
#define GL_VERTEX_SHADER_ARB				0x8B31
#define GL_FRAGMENT_SHADER_ARB				0x8B30
#define GL_VERTEX_PROGRAM_ARB				0x8620
#define GL_FRAGMENT_PROGRAM_ARB				0x8804
#define GL_FOG_COORDINATE_SOURCE_EXT		0x8450
#define GL_FOG_COORDINATE_EXT				0x8451
#define GL_TEXTURE0_ARB                     0x84C0
#define GL_TEXTURE1_ARB                     0x84C1
#define GL_TEXTURE2_ARB                     0x84C2
#define GL_TEXTURE3_ARB                     0x84C3
#define GL_TEXTURE4_ARB                     0x84C4
#define GL_COMBINE_ARB						0x8570
#define GL_RGB_SCALE_ARB					0x8573

typedef unsigned int GLhandleARB;
typedef char GLcharARB;

typedef void (APIENTRY * PFNGLMULTITEXCOORD2FARBPROC) (GLenum target, GLfloat s, GLfloat t);
typedef void (APIENTRY * PFNGLACTIVETEXTUREARBPROC) (GLenum target);
typedef GLhandleARB (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC) (GLenum shaderType);
typedef void (APIENTRYP PFNGLSHADERSOURCEARBPROC) (GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
typedef void (APIENTRYP PFNGLCOMPILESHADERARBPROC) (GLhandleARB shaderObj);
typedef GLhandleARB (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC) (void);
typedef void (APIENTRYP PFNGLATTACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB obj);
typedef void (APIENTRYP PFNGLLINKPROGRAMARBPROC) (GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC) (GLhandleARB programObj);
typedef void (APIENTRYP PFNGLUNIFORM1IARBPROC) (GLint location, GLint v0);
typedef void (APIENTRYP PFNGLUNIFORM1FARBPROC) (GLint location, GLfloat v0);
typedef void (APIENTRYP PFNGLUNIFORM2FARBPROC) (GLint location, GLfloat v0, GLfloat v1);
typedef void (APIENTRYP PFNGLUNIFORM3FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
typedef void (APIENTRYP PFNGLUNIFORM4FARBPROC) (GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
typedef GLint (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC) (GLhandleARB programObj, const GLcharARB *name);
typedef void (APIENTRYP PFNGLDETACHOBJECTARBPROC) (GLhandleARB containerObj, GLhandleARB attachedObj);
typedef void (APIENTRYP PFNGLDELETEOBJECTARBPROC) (GLhandleARB obj);
typedef void (APIENTRYP PFNGLPROGRAMLOCALPARAMETER4FARBPROC) (GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
typedef void (APIENTRYP PFNGLBINDPROGRAMARBPROC) (GLenum target, GLuint program);

extern PFNGLCREATESHADEROBJECTARBPROC glCreateShaderObjectARB;
extern PFNGLSHADERSOURCEARBPROC glShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC glCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC glCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC glAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC glLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC glUseProgramObjectARB;
extern PFNGLUNIFORM1IARBPROC glUniform1iARB;
extern PFNGLUNIFORM1FARBPROC glUniform1fARB;
extern PFNGLUNIFORM2FARBPROC glUniform2fARB;
extern PFNGLUNIFORM3FARBPROC glUniform3fARB;
extern PFNGLUNIFORM4FARBPROC glUniform4fARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC glGetUniformLocationARB;
extern PFNGLDETACHOBJECTARBPROC glDetachObjectARB;
extern PFNGLDELETEOBJECTARBPROC glDeleteObjectARB;
extern PFNGLPROGRAMLOCALPARAMETER4FARBPROC glProgramLocalParameter4fARB;
extern PFNGLBINDPROGRAMARBPROC glBindProgramARB;
#ifdef PLATFORM_WIN32
extern PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
extern PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
extern PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;
#endif
extern PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffersEXT;
extern PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebufferEXT;
extern PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2DEXT;
extern PFNGLBINDRENDERBUFFEREXTPROC glBindRenderbufferEXT;
extern PFNGLGENRENDERBUFFERSEXTPROC glGenRenderbuffersEXT;
extern PFNGLRENDERBUFFERSTORAGEEXTPROC glRenderbufferStorageEXT;
extern PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC glFramebufferRenderbufferEXT;
extern PFNGLUNIFORMMATRIX4FVARBPROC glUniformMatrix4fvARB;
extern PFNGLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
extern PFNGLGETATTRIBLOCATIONPROC glGetAttribLocation;
extern PFNGLGETSHADERIVPROC glGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
extern PFNGLUNIFORM1IPROC glUniform1i;
extern PFNGLUNIFORM1IVPROC glUniform1iv;
extern PFNGLUNIFORM2IVPROC glUniform2iv;
extern PFNGLUNIFORM3IVPROC glUniform3iv;
extern PFNGLUNIFORM4IVPROC glUniform4iv;
extern PFNGLUNIFORM1FPROC glUniform1f;
extern PFNGLUNIFORM1FVPROC glUniform1fv;
extern PFNGLUNIFORM2FVPROC glUniform2fv;
extern PFNGLUNIFORM3FVPROC glUniform3fv;
extern PFNGLUNIFORM4FVPROC glUniform4fv;
extern PFNGLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
extern PFNGLUNIFORM4FPROC glUniform4f;
extern PFNGLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
extern PFNGLBINDBUFFERPROC glBindBuffer;
extern PFNGLBUFFERDATAPROC glBufferData;

#define SSLOT_SHADOWMAP			0
#define SSLOT_LIGHTMATRIX		1
#define SSLOT_LIGHTPOS			2
//#define SSLOT_LIGHTDIR			3
#define SSLOT_TEXTURE0			4
#define SSLOT_TEXTURE1			5
#define SSLOT_TEXTURE2			6
#define SSLOT_TEXTURE3			7
#define SSLOT_POSITION			8
#define SSLOT_NORMAL			9
#define SSLOT_TEXCOORD0			10
#define SSLOT_TEXCOORD1			11
#define SSLOT_PROJECTION		12
#define SSLOT_MODELMAT			13
#define SSLOT_VIEWMAT			14
#define SSLOT_NORMALMAT			15
#define SSLOT_INVMODLVIEWMAT	16
#define SSLOT_MVPMAT			17
#define SSLOT_COLOR				18
#define SSLOT_OWNCOLOR			19
#define SSLOT_WIDTH				20
#define SSLOT_HEIGHT			21
#define SSLOT_CAMERAPOS			22
#define SSLOT_SCALE				23
#define SSLOT_MIND				24
#define SSLOT_MAXD				25
#define SSLOT_NORMALMAP			26
#define SSLOT_SPECULARMAP		27
//#define SSLOT_TANGENT			28
#define SSLOT_MAXELEV			28
#define SSLOT_SANDONLYMAXY		29
#define SSLOT_SANDGRASSMAXY		30
#define SSLOT_GRASSONLYMAXY		31
#define SSLOT_GRASSDIRTMAXY		32
#define SSLOT_DIRTONLYMAXY		33
#define SSLOT_DIRTROCKMAXY		34
#define SSLOT_SANDGRAD			35
#define SSLOT_SANDDET			36
#define SSLOT_GRASSGRAD			37
#define SSLOT_GRASSDET			38
#define SSLOT_DIRTGRAD			39
#define SSLOT_DIRTDET			40
#define SSLOT_ROCKGRAD			41
#define SSLOT_ROCKDET			42
#define SSLOT_SUNDIRECTION		43
#define SSLOT_GRADIENTTEX		44
#define SSLOT_DETAILTEX			45
#define SSLOT_OWNERMAP			46
#define SSLOTS					47

class Shader
{
public:
	Shader()	{			 }
	~Shader()	{ release(); }

	GLint GetUniform(const char* strVariable);
	GLint GetAttrib(const char* strVariable);

	GLhandleARB GetProgram()	{	return m_hProgramObject; }
	GLhandleARB GetVertexS()	{	return m_hVertexShader; }
	GLhandleARB GetFragmentS()	{	return m_hFragmentShader; }
	void MapUniform(int slot, const char* variable);
	void MapAttrib(int slot, const char* variable);

	void release();

	GLint m_slot[SSLOTS];

	GLhandleARB m_hVertexShader;
	GLhandleARB m_hFragmentShader;
	GLhandleARB m_hProgramObject;
};

#define SHADER_DEPTH		0
#define SHADER_SHADOW		1
#define SHADER_OWNED		2
#define SHADER_ORTHO		3
#define SHADER_COLOR2D		4
#define SHADER_MODEL		5
#define SHADER_MAP		6
#define SHADER_COLOR3D		7
#define SHADER_BILLBOARD	8
#define SHADER_WATER		9
#define SHADER_COLOR3DPERSP	10
#define SHADER_BILLBOARDPERSP	11
#define SHADERS				12

extern Shader g_shader[SHADERS];
extern int g_curS;

void UseS(int shader);
void InitGLSL();
void TurnOffShader();
void ReleaseShaders();
string LoadTextFile(char* strFile);
void LoadShader(int shader, char* strVertex, char* strFragment);

#ifdef PLATFORM_WIN32
PROC GetGLProcAddr(const char* procname);
#else
void (*GetGLProcAddr(const char* procname))();
#endif

#endif


