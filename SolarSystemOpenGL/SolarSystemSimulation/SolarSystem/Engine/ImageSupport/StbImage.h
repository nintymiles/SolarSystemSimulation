#pragma once
#include "Image.h"
#include "GLutils.h"

namespace namespaceimage {
class StbImage : public Image
{
	unsigned int byte;
	unsigned int format;
    unsigned int channels;
    
//    png_byte colorType;
public:
	StbImage(void);
	~StbImage(void);

	//! Load the bitmap file helper.
	virtual bool loadImage(char* fileName, bool generateTexID = true, GLenum target = GL_TEXTURE_2D);
	unsigned char* read_image_file(char* file_name);//, png_bytep * row_pointers, int* width, int* height);
};

}
