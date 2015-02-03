#include "platform.h"
#include "texture.h"
#include "draw/model.h"
#include "gui/gui.h"
#include "utils.h"

Texture g_texture[TEXTURES];
vector<TextureToLoad> g_texLoad;

int g_texwidth;
int g_texheight;
//int g_texindex;
int g_lastLTex = -1;

char jpegBuffer[JPEG_BUFFER_SIZE];
JPEGSource   jpegSource;
FILE* g_src;
//CFile g_src;
int srcLen;

#ifdef PLATFORM_WIN32  //uses glaux.h
LoadedTex *LoadBMP(const char *fullpath)
{
	AUX_RGBImageRec *pBitbldg = NULL;
	FILE *pFile = NULL;

	if((pFile = fopen(fullpath, "rb")) == NULL)
	{
		MessageBox(g_hWnd, ("Unable to load BMP File!"), ("Error"), MB_OK);
		return NULL;
	}

	// Load the bitbldg using the aux function stored in glaux.lib
	pBitbldg = auxDIBImageLoad(fullpath);

	LoadedTex *pImage = (LoadedTex *)malloc(sizeof(LoadedTex));

	pImage->channels = 3;
	pImage->sizeX = pBitbldg->sizeX;
	pImage->sizeY = pBitbldg->sizeY;
	pImage->data  = pBitbldg->data;

	free(pBitbldg);

	/*
	int stride = pImage->channels * pBitbldg->sizeX;
	int i;
	int y2;
	int temp;

	for(int y = 0; y < pImage->sizeY/2; y++)
	{
		y2 = pImage->sizeY - y;
		// Store a pointer to the current line of pixels
		unsigned char *pLine = &(pImage->data[stride * y]);
		unsigned char *pLine2 = &(pImage->data[stride * y2]);

		// Go through all of the pixels and swap the B and R values since TGA
		// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
		for(i = 0; i < stride; i += pImage->channels)
		{
			temp = pLine[i];
			pLine[i] = pLine2[i];
			pLine2[i] = temp;

			temp = pLine[i+1];
			pLine[i+1] = pLine2[i+1];
			pLine2[i+1] = temp;

			temp = pLine[i+2];
			pLine[i+2] = pLine2[i+2];
			pLine2[i+2] = temp;
		}
	}*/

	return pImage;
}
#endif  //PLATFORM_WIN32

LoadedTex *LoadTGA(const char *fullpath)
{
	LoadedTex *pImageData = NULL;			// This stores our important image data
	WORD width = 0, height = 0;			// The dimensions of the image
	byte length = 0;					// The length in bytes to the pixels
	byte imageType = 0;					// The image type (RLE, RGB, Alpha...)
	byte bits = 0;						// The bits per pixel for the image (16, 24, 32)
	FILE *pFile = NULL;					// The file pointer
	int channels = 0;					// The channels of the image (3 = RGA : 4 = RGBA)
	int stride = 0;						// The stride (channels * width)
	int i = 0;							// A counter

	if((pFile = fopen(fullpath, "rb")) == NULL)
	{
		//MessageBox(g_hWnd, ("Unable to load TGA File!"), ("Error"), MB_OK);
		ErrorMessage("Error", "Unable to load TGA file!");
		return NULL;
	}

	// Allocate the structure that will hold our eventual image data (must free it!)
	pImageData = (LoadedTex*)malloc(sizeof(LoadedTex));

	// Read in the length in bytes from the header to the pixel data
	fread(&length, sizeof(byte), 1, pFile);

	// Jump over one byte
	fseek(pFile,1,SEEK_CUR);

	// Read in the imageType (RLE, RGB, etc...)
	fread(&imageType, sizeof(byte), 1, pFile);

	// Skip past general information we don't care about
	fseek(pFile, 9, SEEK_CUR);

	// Read the width, height and bits per pixel (16, 24 or 32)
	fread(&width,  sizeof(WORD), 1, pFile);
	fread(&height, sizeof(WORD), 1, pFile);
	fread(&bits,   sizeof(byte), 1, pFile);

	// Now we move the file pointer to the pixel data
	fseek(pFile, length + 1, SEEK_CUR);

	// Check if the image is RLE compressed or not
	if(imageType != TGA_RLE)
	{
		// Check if the image is a 24 or 32-bit image
		if(bits == 24 || bits == 32)
		{
			// Calculate the channels (3 or 4) - (use bits >> 3 for more speed).
			// Next, we calculate the stride and allocate enough memory for the pixels.
			channels = bits / 8;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			// Load in all the pixel data line by line
			for(int y = 0; y < height; y++)
			{
				// Store a pointer to the current line of pixels
				unsigned char *pLine = &(pImageData->data[stride * y]);

				// Read in the current line of pixels
				fread(pLine, stride, 1, pFile);

				// Go through all of the pixels and swap the B and R values since TGA
				// files are stored as BGR instead of RGB (or use GL_BGR_EXT verses GL_RGB)
				for(i = 0; i < stride; i += channels)
				{
					int temp     = pLine[i];
					pLine[i]     = pLine[i + 2];
					pLine[i + 2] = temp;
				}
			}
		}
		// Check if the image is a 16 bit image (RGB stored in 1 unsigned short)
		else if(bits == 16)
		{
			unsigned short pixels = 0;
			int r=0, g=0, b=0;

			// Since we convert 16-bit images to 24 bit, we hardcode the channels to 3.
			// We then calculate the stride and allocate memory for the pixels.
			channels = 3;
			stride = channels * width;
			pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));

			// Load in all the pixel data pixel by pixel
			for(int i = 0; i < width*height; i++)
			{
				// Read in the current pixel
				fread(&pixels, sizeof(unsigned short), 1, pFile);

				// Convert the 16-bit pixel into an RGB
				b = (pixels & 0x1f) << 3;
				g = ((pixels >> 5) & 0x1f) << 3;
				r = ((pixels >> 10) & 0x1f) << 3;

				// This essentially assigns the color to our array and swaps the
				// B and R values at the same time.
				pImageData->data[i * 3 + 0] = r;
				pImageData->data[i * 3 + 1] = g;
				pImageData->data[i * 3 + 2] = b;
			}
		}
		// Else return a NULL for a bad or unsupported pixel format
		else
			return NULL;
	}
	// Else, it must be Run-Length Encoded (RLE)
	else
	{
		// Create some variables to hold the rleID, current colors read, channels, & stride.
		byte rleID = 0;
		int colorsRead = 0;
		channels = bits / 8;
		stride = channels * width;

		// Next we want to allocate the memory for the pixels and create an array,
		// depending on the channel count, to read in for each pixel.
		pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*stride*height));
		byte *pColors = ((byte*)malloc(sizeof(byte)*channels));

		// Load in all the pixel data
		while(i < width*height)
		{
			// Read in the current color count + 1
			fread(&rleID, sizeof(byte), 1, pFile);

			// Check if we don't have an encoded string of colors
			if(rleID < 128)
			{
				// Increase the count by 1
				rleID++;

				// Go through and read all the unique colors found
				while(rleID)
				{
					// Read in the current color
					fread(pColors, sizeof(byte) * channels, 1, pFile);

					// Store the current pixel in our image array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}
			}
			// Else, let's read in a string of the same character
			else
			{
				// Minus the 128 ID + 1 (127) to get the color count that needs to be read
				rleID -= 127;

				// Read in the current color, which is the same for a while
				fread(pColors, sizeof(byte) * channels, 1, pFile);

				// Go and read as many pixels as are the same
				while(rleID)
				{
					// Assign the current pixel to the current index in our pixel array
					pImageData->data[colorsRead + 0] = pColors[2];
					pImageData->data[colorsRead + 1] = pColors[1];
					pImageData->data[colorsRead + 2] = pColors[0];

					// If we have a 4 channel 32-bit image, assign one more for the alpha
					if(bits == 32)
						pImageData->data[colorsRead + 3] = pColors[3];

					// Increase the current pixels read, decrease the amount
					// of pixels left, and increase the starting index for the next pixel.
					i++;
					rleID--;
					colorsRead += channels;
				}

			}

		}
	}

	// Close the file pointer that opened the file
	fclose(pFile);

	// Flip upside-down
	int x;
	int y2;
	byte temp[4];
	for(int y=0; y<height/2; y++)
	{
		y2 = height - y - 1;

		unsigned char *pLine = &(pImageData->data[stride * y]);
		unsigned char *pLine2 = &(pImageData->data[stride * y2]);

		for(x=0; x<width*channels; x+=channels)
		{
			temp[0] = pLine[x + 0];
			temp[1] = pLine[x + 1];
			temp[2] = pLine[x + 2];
			if(bits == 32)
				temp[3] = pLine[x + 3];

			pLine[x + 0] = pLine2[x + 0];
			pLine[x + 1] = pLine2[x + 1];
			pLine[x + 2] = pLine2[x + 2];
			if(bits == 32)
				pLine[x + 3] = pLine2[x + 3];

			pLine2[x + 0] = temp[0];
			pLine2[x + 1] = temp[1];
			pLine2[x + 2] = temp[2];
			if(bits == 32)
				pLine2[x + 3] = temp[3];
		}
	}

	// Fill in our LoadedTex structure to pass back
	pImageData->channels = channels;
	pImageData->sizeX    = width;
	pImageData->sizeY    = height;

	// Return the TGA data (remember, you must free this data after you are done)
	return pImageData;
}

void DecodeJPG(jpeg_decompress_struct* cinfo, LoadedTex *pImageData)
{
	// Read in the header of the jpeg file
	jpeg_read_header(cinfo, TRUE);

	// Start to decompress the jpeg file with our compression info
	jpeg_start_decompress(cinfo);

	// Get the image dimensions and channels to read in the pixel data
	pImageData->channels = cinfo->num_components;
	pImageData->sizeX    = cinfo->image_width;
	pImageData->sizeY    = cinfo->image_height;

	// Get the row span in bytes for each row
	int rowSpan = cinfo->image_width * cinfo->num_components;

	// Allocate memory for the pixel buffer
	pImageData->data = ((unsigned char*)malloc(sizeof(unsigned char)*rowSpan*pImageData->sizeY));

	// Create an array of row pointers
	unsigned char** rowPtr = new unsigned char*[pImageData->sizeY];

	for (int i = 0; i < pImageData->sizeY; i++)
		rowPtr[i] = &(pImageData->data[i * rowSpan]);

	// Now comes the juice of our work, here we extract all the pixel data
	int rowsRead = 0;
	while (cinfo->output_scanline < cinfo->output_height)
	{
		// Read in the current row of pixels and increase the rowsRead count
		rowsRead += jpeg_read_scanlines(cinfo,
										&rowPtr[rowsRead], cinfo->output_height - rowsRead);
	}

	// Delete the temporary row pointers
	delete [] rowPtr;

	// Finish decompressing the data
	jpeg_finish_decompress(cinfo);
}

LoadedTex *LoadJPG(const char *fullpath)
{
	struct jpeg_decompress_struct cinfo;
	LoadedTex *pImageData = NULL;
	FILE *pFile;

	// Open a file pointer to the jpeg file and check if it was found and opened
	if((pFile = fopen(fullpath, "rb")) == NULL)
	{
		// Display an error message saying the file was not found, then return NULL
		//MessageBox(g_hWnd, "Unable to load JPG File!", "Error", MB_OK);
		return NULL;
	}

	// Create an error handler
	jpeg_error_mgr jerr;

	// Have our compression info object point to the error handler address
	cinfo.err = jpeg_std_error(&jerr);

	// Initialize the decompression object
	jpeg_create_decompress(&cinfo);

	// Specify the data source (Our file pointer)
	jpeg_stdio_src(&cinfo, pFile);

	// Allocate the structure that will hold our eventual jpeg data (must free it!)
	pImageData = (LoadedTex*)malloc(sizeof(LoadedTex));

	// Decode the jpeg file and fill in the image data structure to pass back
	DecodeJPG(&cinfo, pImageData);

	// This releases all the stored memory for reading and decoding the jpeg
	jpeg_destroy_decompress(&cinfo);

	// Close the file pointer that opened the file
	fclose(pFile);

	// Return the jpeg data (remember, you must free this data after you are done)
	return pImageData;
}

LoadedTex *LoadPNG(const char *fullpath)
{
	LoadedTex *pImageData = NULL;

	png_structp png_ptr;
    png_infop info_ptr;
    unsigned int sig_read = 0;
    //int color_type, interlace_type;
    FILE *fp;

	/*
	if ((fp = fopen(relative, "rb")) == NULL)
        return NULL;
	png_byte header[8];
	fread(header, sizeof(png_byte), 8, fp);
	fclose(fp);

	g_log<<"PNG header "<<relative<<" "
		<<(int)header[0]<<","<<(int)header[1]<<","<<(int)header[2]<<","<<(int)header[3]<<","
		<<(int)header[4]<<","<<(int)header[5]<<","<<(int)header[6]<<","<<(int)header[7]<<endl;
	*/
	if ((fp = fopen(fullpath, "rb")) == NULL)
        return NULL;


	/* Create and initialize the png_struct
     * with the desired error handler
     * functions.  If you want to use the
     * default stderr and longjump method,
     * you can supply NULL for the last
     * three parameters.  We also supply the
     * the compiler header file version, so
     * that we know if the application
     * was compiled with a compatible version
     * of the library.  REQUIRED
     */

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
        fclose(fp);
        return NULL;
    }

	/* Allocate/initialize the memory
     * for image information.  REQUIRED. */
    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL)
	{
        fclose(fp);
        png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
        return NULL;
    }

	/* Set error handling if you are
     * using the setjmp/longjmp method
     * (this is the normal method of
     * doing things with libpng).
     * REQUIRED unless you  set up
     * your own error handlers in
     * the png_create_read_struct()
     * earlier.
     */
    if (setjmp(png_jmpbuf(png_ptr)))
	{
        /* Free all of the memory associated
         * with the png_ptr and info_ptr */
        png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
        fclose(fp);
        /* If we get here, we had a
         * problem reading the file */
        return NULL;
    }

	/* Set up the output control if
     * you are using standard C streams */
    png_init_io(png_ptr, fp);

	/* If we have already
     * read some of the signature */
    png_set_sig_bytes(png_ptr, sig_read);

	/*
     * If you have enough memory to read
     * in the entire image at once, and
     * you need to specify only
     * transforms that can be controlled
     * with one of the PNG_TRANSFORM_*
     * bits (this presently excludes
     * dithering, filling, setting
     * background, and doing gamma
     * adjustment), then you can read the
     * entire image (including pixels)
     * into the info structure with this
     * call
     *
     * PNG_TRANSFORM_STRIP_16 |
     * PNG_TRANSFORM_PACKING  forces 8 bit
     * PNG_TRANSFORM_EXPAND forces to
     *  expand a palette into RGB
     */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, png_voidp_NULL);

	pImageData = (LoadedTex*)malloc(sizeof(LoadedTex));

	pImageData->sizeX = png_get_image_width(png_ptr, info_ptr); //info_ptr->width;
    pImageData->sizeY = png_get_image_height(png_ptr, info_ptr); //info_ptr->height;
    //switch (info_ptr->color_type)
	switch( png_get_color_type(png_ptr, info_ptr) )
	{
        case PNG_COLOR_TYPE_RGBA:
            pImageData->channels = 4;
            break;
        case PNG_COLOR_TYPE_RGB:
            pImageData->channels = 3;
            break;
        default:
			g_log<<fullpath<<" color type "<<png_get_color_type(png_ptr, info_ptr)<<" not supported"<<endl;
            //std::cout << "Color type " << info_ptr->color_type << " not supported" << std::endl;
            png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
            fclose(fp);
			free(pImageData);
            return NULL;
    }

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	pImageData->data = (unsigned char*) malloc(row_bytes * pImageData->sizeY);

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for (int i = 0; i < pImageData->sizeY; i++)
	{
        // note that png is ordered top to
        // bottom, but OpenGL expect it bottom to top
        // so the order or swapped

        memcpy((void*)(pImageData->data+(row_bytes * i)), row_pointers[i], row_bytes);
        //memcpy((void*)(pImageData->data+(row_bytes * (pImageData->sizeY-1-i))), row_pointers[i], row_bytes);
    }

	/* Clean up after the read,
     * and free any memory allocated */
    png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
	fclose(fp);

	return pImageData;
}

bool FindTexture(unsigned int &textureidx, const char* relative)
{
	char corrected[1024];
	strcpy(corrected, relative);
	CorrectSlashes(corrected);

	for(int i=0; i<TEXTURES; i++)
	{
		Texture* t = &g_texture[i];

		if(t->loaded && _stricmp(t->filepath, corrected) == 0)
		{
			//g_texindex = i;
			//texture = t->texname;
			textureidx = i;
			g_texwidth = t->width;
			g_texheight = t->height;
			return true;
		}
	}

	return false;
}

void FreeTexture(const char* relative)
{
	char corrected[MAX_PATH+1];
	strcpy(corrected, relative);
	CorrectSlashes(corrected);

	for(int i=0; i<TEXTURES; i++)
	{
		Texture* t = &g_texture[i];

		if(t->loaded && _stricmp(t->filepath, corrected) == 0)
		{
			t->loaded = false;
			glDeleteTextures(1, &t->texname);
			//g_log<<"Found texture "<<filepath<<" ("<<texture<<")"<<endl;
			return;
		}
	}
}

void FreeTexture(int i)
{
	Texture* t = &g_texture[i];

	if(t->loaded)
	{
		t->loaded = false;
		glDeleteTextures(1, &t->texname);
	}
}

int NewTexture()
{
	for(int i=0; i<TEXTURES; i++)
		if(!g_texture[i].loaded)
			return i;

	return -1;
}

bool TextureLoaded(unsigned int texture, const char* relative, bool transp, unsigned int& texindex, bool reload)
{
	char corrected[1024];
	strcpy(corrected, relative);
	CorrectSlashes(corrected);

	if(!reload)
	{
		texindex = NewTexture();

		if((int)texindex < 0)
		{
			texindex = 0;	// Give a harmless texture index
			return false;
		}
	}

	//g_texindex = texindex;
	Texture* t = &g_texture[texindex];
	t->loaded = true;
	strcpy(t->filepath, corrected);
	t->texname = texture;
	t->width = g_texwidth;
	t->height = g_texheight;
	t->transp = transp;

	t->sky = false;
	t->breakable = false;
	t->passthru = false;
	t->grate = false;
	t->ladder = false;
	t->water = false;
	t->fabric = false;

	if(strstr(corrected, "^"))
		t->sky = true;
	if(strstr(corrected, "!"))
		t->breakable = true;
	if(strstr(corrected, "~"))
		t->passthru = true;
	if(strstr(corrected, "`"))
		t->grate = true;
	if(strstr(corrected, "#"))
		t->ladder = true;
	if(strstr(corrected, "$"))
		t->water = true;
	if(strstr(corrected, "@"))
		t->fabric = true;

	return true;
}

void FreeTextures()
{
	for(int i=0; i<TEXTURES; i++)
	{
		if(!g_texture[i].loaded)
			continue;

		glDeleteTextures(1, &g_texture[i].texname);
		//g_texture[i].loaded = false;	// Needed to reload textures
	}
}

void FindTextureExtension(char *relative)
{
	char strJPGPath[MAX_PATH] = {0};
	char strPNGPath[MAX_PATH] = {0};
	char strTGAPath[MAX_PATH]    = {0};
	char strBMPPath[MAX_PATH]    = {0};
	FILE *fp = NULL;

	//GetCurrentDirectory(MAX_PATH, strJPGPath);

	//strcat(strJPGPath, "\\");
	FullPath("", strJPGPath);

	strcat(strJPGPath, relative);
	strcpy(strTGAPath, strJPGPath);
	strcpy(strBMPPath, strTGAPath);
	strcpy(strPNGPath, strBMPPath);

	strcat(strJPGPath, ".jpg");
	strcat(strTGAPath, ".tga");
	strcat(strBMPPath, ".bmp");
	strcat(strPNGPath, ".png");

	if((fp = fopen(strJPGPath, "rb")) != NULL)
	{
		fclose(fp);
		strcat(relative, ".jpg");
		return;
	}

	if((fp = fopen(strPNGPath, "rb")) != NULL)
	{
		fclose(fp);
		strcat(relative, ".png");
		return;
	}

	if((fp = fopen(strTGAPath, "rb")) != NULL)
	{
		fclose(fp);
		strcat(relative, ".tga");
		return;
	}

	if((fp = fopen(strBMPPath, "rb")) != NULL)
	{
		fclose(fp);
		strcat(relative, ".bmp");
		return;
	}
}

bool Load1Texture()
{
	if(g_lastLTex+1 < g_texLoad.size())
		UpdateStatus(g_texLoad[g_lastLTex+1].relative);

	if(g_lastLTex >= 0)
	{
		TextureToLoad* t = &g_texLoad[g_lastLTex];
		if(t->reload)
			CreateTexture(t->texindex, t->relative, t->clamp, t->reload);
		else
			CreateTexture(*t->ptexindex, t->relative, t->clamp, t->reload);
	}

	g_lastLTex ++;

	if(g_lastLTex >= g_texLoad.size())
	{
		g_texLoad.clear();
		return false;	// Done loading all textures
	}

	return true;	// Not finished loading textures
}

void QueueTexture(unsigned int* texindex, const char* relative, bool clamp)
{
	TextureToLoad toLoad;
	toLoad.ptexindex = texindex;
	strcpy(toLoad.relative, relative);
	toLoad.clamp = clamp;
	toLoad.reload = false;

	g_texLoad.push_back(toLoad);
}

void RequeueTexture(unsigned int texindex, const char* relative, bool clamp)
{
	TextureToLoad toLoad;
	toLoad.texindex = texindex;
	strcpy(toLoad.relative, relative);
	toLoad.clamp = clamp;
	toLoad.reload = true;

	g_texLoad.push_back(toLoad);
}

LoadedTex* LoadTexture(const char* full)
{
	if(strstr(full, ".jpg"))
	{
		return LoadJPG(full);
		//return LoadJPG2(relative);
	}
	else if(strstr(full, ".png"))
	{
		return LoadPNG(full);
	}
	else if(strstr(full, ".tga"))
	{
		return LoadTGA(full);
	}
#ifdef PLATFORM_WIN32   //uses glaux.h
	else if(strstr(full, ".bmp"))
	{
		return LoadBMP(full);
	}
#endif

	return NULL;
}

bool CreateTexture(unsigned int &texindex, const char* relative, bool clamp, bool reload)
{
	if(!relative)
		return false;

	if(!reload)
		if(FindTexture(texindex, relative))
			return true;

	// Define a pointer to a LoadedTex
	LoadedTex *pImage = NULL;

	char full[1024];
	FullPath(relative, full);

	pImage = LoadTexture(full);

	// Make sure valid image data was given to pImage, otherwise return false
	if(pImage == NULL)
	{
		g_log<<"Failed to load "<<relative<<endl;
		g_log.flush();

		if(!reload)
			texindex = 0;	// Give a harmless texture index

		return false;
	}

	unsigned int texname;
	// Generate a texture with the associative texture ID stored in the array
	glGenTextures(1, &texname);

	// This sets the alignment requirements for the start of each pixel row in memory.
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

	// Bind the texture to the texture arrays index and init the texture
	glBindTexture(GL_TEXTURE_2D, texname);

	// Assume that the texture is a 24 bit RGB texture (We convert 16-bit ones to 24-bit)
	int textureType = GL_RGB;
	bool transp = false;

	// If the image is 32-bit (4 channels), then we need to specify GL_RGBA for an alpha
	if(pImage->channels == 4)
	{
		textureType = GL_RGBA;
		transp = true;
	}

	// Option 1: with mipbldgs
	gluBuild2DMipmaps(GL_TEXTURE_2D, pImage->channels, pImage->sizeX, pImage->sizeY, textureType, GL_UNSIGNED_BYTE, pImage->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(clamp)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	// Option 2: without mipbldgs
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexImage2D(GL_TEXTURE_2D, 0, textureType, pImage->sizeX, pImage->sizeY, 0, textureType, GL_UNSIGNED_BYTE, pImage->data);

	if(pImage)
	{
		g_texwidth = pImage->sizeX;
		g_texheight = pImage->sizeY;

		if (pImage->data)							// If there is texture data
		{
			free(pImage->data);						// Free the texture data, we don't need it anymore
		}

		free(pImage);								// Free the image structure

		g_log<<relative<<"\n\r";
		g_log.flush();
	}

	TextureLoaded(texname, relative, transp, texindex, reload);

	// Return a success
	return true;
}

void RequeueTextures()
{
	FreeTextures();

	for(int i=0; i<TEXTURES; i++)
	{
		if(g_texture[i].loaded)
			RequeueTexture(i, g_texture[i].filepath, i);
	}

	//LoadParticles();
	//LoadProjectiles();
	//LoadTerrainTextures();
	//LoadUnitSprites();
	//BSprites();
}

void DiffPath(const char* basepath, char* diffpath)
{
	strcpy(diffpath, basepath);
	//StripExtension(diffpath);
	strcat(diffpath, ".jpg");
}

void SpecPath(const char* basepath, char* specpath)
{
	strcpy(specpath, basepath);
	//StripExtension(specpath);
	strcat(specpath, ".spec.jpg");
}

void NormPath(const char* basepath, char* normpath)
{
	strcpy(normpath, basepath);
	//StripExtension(normpath);
	strcat(normpath, ".norm.jpg");
}
