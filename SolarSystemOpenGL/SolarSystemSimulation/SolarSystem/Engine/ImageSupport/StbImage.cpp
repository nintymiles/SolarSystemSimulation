#include "StbImage.h"

#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"



using namespace namespaceimage;

//todo:???
StbImage::StbImage(void)
{
	//memData.extension = PNG;
}

StbImage::~StbImage(void)
{
	//LOGI("destructor: StbImage");
	if ( memData.bitsraw )
		delete memData.bitsraw;
	memData.bitsraw = NULL;
	if (memData.texID){
		glDeleteTextures( 1,&memData.texID );
		memData.texID	= 0;
	}
}

void loadJPGImage(char* fileName){
    int width, height, channels;
    unsigned char *img = stbi_load(fileName, &width, &height, &channels, 0);
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
}

unsigned char* StbImage::read_image_file(char* file_name)
{
    int width, height, channels;
    unsigned char *img = stbi_load(file_name, &width, &height, &channels, 0);
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);
    
    /**# stb_image的代码阅读所获的信息
     * stbi__context包含了图像文件的各种信息，其中图像缓存信息位于
     * stbi__context.img_buffer = 0x00007ffeebb78cd8 "\x89PNG\r\n\x1a\n"
     * stbi_info(file_name, &width, &height, &channels)用于确认支持的图像格式
     * 其实现内部包含有stbi__jpeg_info(),stbi__png_info()函数，用于每种具体图像格式的判断
     */
    //int stbi_I= stbi_info(file_name, &width, &height, &channels);
    //stbi__context s;
    
    memData.width = width;
    memData.height = height;
    this->channels = channels;
    
    //colorType属性的作用是为了确定图像的RGBA通道
    
    return img;

}

//! Load the bitmap file helper.
bool StbImage::loadImage(char* fileName, bool generateTexID, GLenum target )
{
    // Get the image bits from the png file.
	memData.bitsraw = read_image_file( fileName);

	if (!memData.bitsraw){
		LOGI("Fail to read image file: %s", fileName);
		return false;
	}
    
    // Generate the texture ID if it is not produced before
    if ( generateTexID ){
        GLuint texID;
        glGenTextures	( 1,&texID );
        memData.texID = texID;

        // Depending upon the target type bind the
        // texture using generated texture ID handler
        if (target == GL_TEXTURE_2D){
            glBindTexture	( GL_TEXTURE_2D,texID );
        }
        else if (target == GL_TEXTURE_3D){
            glBindTexture	( GL_TEXTURE_3D,texID );
        }
        else if (target == GL_TEXTURE_2D_ARRAY){
            glBindTexture	( GL_TEXTURE_2D_ARRAY,texID );
        }
        else{
            #ifdef __ANDROID__
			glBindTexture	( 0x8513/*GL_TEXTURE_CUBE_MAP*/,texID );
            #else
            glBindTexture	( GL_TEXTURE_CUBE_MAP,texID );
            #endif
        }
    }
    
    // according to channels,to prepare the texture
    switch (channels) {
        case 1:
        {
            glTexImage2D    ( target,  0, GL_LUMINANCE,  memData.width, memData.height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, memData.bitsraw );
            break;
        }
        case 2:
        {
            glTexImage2D    ( target,  0, GL_ALPHA,  memData.width, memData.height, 0, GL_ALPHA, GL_UNSIGNED_BYTE, memData.bitsraw );
            break;
        }
        case 3:
        {
            glTexImage2D    ( target,  0, GL_RGB,  memData.width, memData.height, 0, GL_RGB, GL_UNSIGNED_BYTE, memData.bitsraw );
            break;
        }
        case 4:
        {
            glTexImage2D    ( target,  0, GL_RGBA,  memData.width, memData.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, memData.bitsraw );
            break;
        }
    }
    
    // Release the allocate memory for image bits.
    free (memData.bitsraw);
    memData.bitsraw = NULL;
	return true;
}
