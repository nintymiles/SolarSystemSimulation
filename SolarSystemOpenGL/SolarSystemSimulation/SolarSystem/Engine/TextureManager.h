#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#pragma once
#include "StbImage.h"

using namespace namespaceimage;

//! Draw callback function prototype.
typedef void( PROGRAMDRAWCALLBACK( void * ) );

#define MAX_TEXTURE_SIZE 20
#define MAX_TEXTURE_NAME_LENGTH 256

//GLES program对象是GL API操纵的基础对象。
typedef struct
{
	//! The internal name to use for the shader program.
    //初始化program的对应名称，允许最大字符数目64，为什么要给program起名字？索引的需要？
	char						 name[MAX_TEXTURE_NAME_LENGTH];

	//! The internal GLES program id for the shader program.
	unsigned int				 TexID;
    
    shared_ptr<StbImage>         textureContainerObj;

} TEXTURE;


class TextureManager
{
public:
	TextureManager(void);
	~TextureManager(void);

public:
	TEXTURE *TextureInit( char *name );

	TEXTURE *TextureFree( TEXTURE *texture );

	void TextureDeleteId( TEXTURE *texture );

	void TextureReset( TEXTURE *texture );

    TEXTURE* TextureLoad(char* name, char* textureFilePath);


    TEXTURE* Texture( char* texture );

	unsigned char AddTexture( TEXTURE* texture );

	unsigned char RemoveTexture( TEXTURE* texture );

	inline unsigned char TextureCount() { return textureCounter; }

private:
	//! We need some mechanism to recognize the program for each model.
	TEXTURE* textureList[20];

	//! Keeps track of the number of texture in the renderer.
	unsigned char textureCounter;
};

#endif
