


#ifndef TEXTURE_H
#define TEXTURE_H

#include "platform.h"

class Texture
{
public:
	unsigned int texname;
	char filepath[MAX_PATH];
	bool loaded;
	int width, height;
	bool transp;
	bool sky;
	bool breakable;
	bool passthru;
	bool grate;
	bool ladder;
	bool water;
	bool fabric;

	Texture()
	{
		loaded = false;
	}
};

#define TEXTURES	2048
extern Texture g_texture[TEXTURES];

#define TGA_RGB		 2		// This tells us it's a normal RGB (really BGR) file
#define TGA_A		 3		// This tells us it's an ALPHA file
#define TGA_RLE		10		// This tells us that the targa is Run-Length Encoded (RLE)

#ifdef PLATFORM_WIN32
#define png_infopp_NULL (png_infopp)NULL
#define int_p_NULL (int*)NULL
#define png_voidp_NULL	(png_voidp)NULL
#endif

#define JPEG_BUFFER_SIZE (8 << 10)

typedef struct
{
    struct jpeg_source_mgr  pub;
} JPEGSource;

struct LoadedTex
{
	int channels;			// The channels in the image (3 = RGB : 4 = RGBA)
	int sizeX;				// The width of the image in pixels
	int sizeY;				// The height of the image in pixels
	unsigned char *data;	// The image pixel data
};

struct TextureToLoad
{
	unsigned int* ptexindex;
	unsigned int texindex;
	char relative[MAX_PATH+1];
	bool clamp;
	bool reload;
};

extern int g_texwidth;
extern int g_texheight;
//extern int g_texindex;
extern int g_lastLTex;

#ifdef PLATFORM_WIN32  //uses glaux.h
LoadedTex *LoadBMP(const char *fullpath);
#endif

LoadedTex *LoadTGA(const char *fullpath);
void DecodeJPG(jpeg_decompress_struct* cinfo, LoadedTex *pImageData);
LoadedTex *LoadJPG(const char *fullpath);
LoadedTex *LoadPNG(const char *fullpath);
bool FindTexture(unsigned int &texture, const char* relative);
int NewTexture();
bool TextureLoaded(unsigned int texture, const char* relative, bool transp, unsigned int& texindex);
void FindTextureExtension(char *relative);
void FreeTextures();
bool Load1Texture();
void QueueTexture(unsigned int* texindex, const char* relative, bool clamp);
void RequeueTexture(unsigned int texindex, const char* relative, bool clamp);
LoadedTex* LoadTexture(const char* full);
bool CreateTexture(unsigned int &texindex, const char* relative, bool clamp, bool reload=false);
void ReloadTextures();
void FreeTexture(const char* relative);
void FreeTexture(int i);
void DiffPath(const char* basepath, char* diffpath);
void SpecPath(const char* basepath, char* specpath);
void NormPath(const char* basepath, char* normpath);

#endif
