#include "TextureManager.h"
#include <string.h>
#include "Cache.h"
#include "GLutils.h"

//在类的默认构造器中，将programList缓存初始化，所有的元素都清空为NULL，计数器置0
TextureManager::TextureManager(void) {
	for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
		textureList[idx] = NULL;
	}
	textureCounter = 0;
}

TextureManager::~TextureManager(void) {
	for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
		free(textureList[idx]);
	}
	textureCounter = 0;
}


//初始化一个新的program结构体，寻址内存，并初始化name和ProgramID为0
TEXTURE* TextureManager::TextureInit(char *name) {
	TEXTURE *texture = (TEXTURE *) calloc(1, sizeof(TEXTURE));

	strcpy(texture->name, name);
	texture->TexID = 0;

	return texture;
}


TEXTURE* TextureManager::TextureFree(TEXTURE *texture) {
	if (texture->TexID)
		TextureDeleteId(texture);

	free(texture);
	return NULL;
}


//删除program通过programID,同时将PROGRAM structure中的programId设置为not occupied
void TextureManager::TextureDeleteId(TEXTURE *texture) {
	if (texture->TexID) {
		glDeleteTextures(1, &texture->TexID);

		texture->TexID = 0;
	}
}

TEXTURE* TextureManager::TextureLoad(char* name, char* textureFilePath){
    TEXTURE* texture = NULL;
    //若name所指定的texture存在，则直接返回此texture，这是一种缓存式的使用。
    if (!( texture = Texture(name) ) )
    //如果没有已经存在的program，则初始化一个program，并挂载vertex/fragment shader,并重新编译。
    {
        //生成一个新的program结构对象实例
        texture = TextureInit(name);
        texture->textureContainerObj.reset(new StbImage());
        texture->textureContainerObj->loadImage(textureFilePath);
        texture->TexID = texture->textureContainerObj->getTextureID();
        AddTexture(texture);
    }
    
    return texture;
}

TEXTURE* TextureManager::Texture(char* textureName) {
    //以遍历的方式查找每个texture元素的名字是否匹配，如果匹配则返回此texture的指针。
	for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
		if (textureList[idx] && !strcmp(textureList[idx]->name, textureName)) {
			return textureList[idx];
		}
        
	}
	return NULL;
}


unsigned char TextureManager::AddTexture(TEXTURE* texture) {
	bool isPresent = false;
    //MAX_TEXTURE_SIZE = 20，最大可能的texture数量设置为20
	for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
        //strcmp函数的作用？以字典顺序比较两个byte string，如果没有差异，则返回0.
        //如果programList在当前索引处不为空，且两个name比较相等则说明program已经存在于缓存中。
		if (textureList[idx]
				&& !strcmp(textureList[idx]->name, texture->name)) {
			isPresent = true;
			//Already present in the list
			return 0;
		}
	}

    //两个分支遍历了两遍，这个实现是否有效率？
	if (!isPresent) {
		bool added = false;
        //既然存在programCounter++，为何还要遍历？因为programList缓存中的program结构对象随时可能被delete，也即任何一个缓存位都可能为空，说明programList中的两个元素并不一定相邻。也就是下面的遍历逻辑是没有问题的。
        //再遍历一遍programList，若当前idx处array entry为空，则将program加入此处，同时将programCounter计数器加1
		for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
			if (!textureList[idx]) {
				textureList[idx] = texture;
				textureCounter++;
				return 1; // List added sucessfully
			}
		}
        //若执行到for语句之后，则说明缓存空间已经用尽，无法再加添加入缓存。
		return 2; // No Space in the list to add
	}
    
    return 3;
}


unsigned char TextureManager::RemoveTexture(TEXTURE *texture){
	for (int idx = 0; idx < MAX_TEXTURE_SIZE; idx++) {
        //首先查找program元素是否存在，存在则执行释放逻辑。
		if (textureList[idx]
				&& !strcmp(textureList[idx]->name, texture->name)) {
            //这里有无问题？program对象被直接释放，而没有先行释放shaders，id等
			//free (programList[idx]);
            //改为ProgramFree()函数释放，内存更干净。
            TextureFree(textureList[idx]);
            //释放program strtucture元素后记得将计数器减1
			textureCounter--;
            //将array当前索引处设置为null，以便缓存的充分使用
			textureList[idx] = NULL;
			return 1;
		}
	}
	return 0;
}
