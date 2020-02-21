#ifdef _WIN32
#endif

#include "Cache.h"

#include <string.h>
#include <fstream>
using std::ifstream;
#include <sstream>
using std::ostringstream;
#include "GLutils.h"

/*!
	Open/Extract a file from disk and load it in cachePtr.

	\param[in] filename The file to load in cachePtr.
	\param[in] relative_path Determine if the filename is an absolute or relative path.

	\return Return a CACHE structure pointer if the file is found and loaded, instead will return
	NULL.
*/
CACHE *reserveCache( char *filename, unsigned char relative_path )
{

    //文件操作，iPhone和Android以条件编译方式独自处理
	#ifdef __IPHONE_4_0

		FILE *f;

        //MAX_PATH is defined as maximum paths number
		char fname[ MAX_PATH ] = {""};

    
		 //此处逻辑组合文件绝对路径名
		if( relative_path )
		{
            GLUtils::extractPath( getenv( "FILESYSTEM" ), fname );

			strcat( fname, filename );
		}
		else strcpy( fname, filename );

        //用fopen打开文件，执行文件操作
        //为什么要用fopen？作用是打开一个文件并且返回指向文件流的指针。
        //C语言操作的特点，就是指针化操作
		f = fopen( fname, "rb" );

        //如果filestream指针f不存在，则说明没有打开文件
		if( !f ) return NULL;

        //CACHE结构变量的定义及内存初始化动作。
    //什么时候的局部变量需要使用用户自己寻址并管理内存？应该说是什么类型的变量，目前可知就是struct结构的变量需要用户自己管理内存。
		CACHE *cachePtr = ( CACHE * ) calloc( 1, sizeof( CACHE ) );

		strcpy( cachePtr->filename, fname );


    //fseek的作用？设置文件指示符的偏离数量，从指定的文件origin，SEEK_END代表从文件结尾，SEEK_SET代表文件开头，SEEK_CUR代表当前位置
		fseek( f, 0, SEEK_END );
    //ftell的作用？返回当前文件流指示符的位置
		cachePtr->size = (unsigned int)ftell( f );
		fseek( f, 0, SEEK_SET );

    //为CACHE结构变量分配文件缓存，缓存的大小为文件的大小（size+1），实际大小比所读取的大小加1，为什么？因为文件读取到结束符时结束，而缓存文件最后需要手工设置一个结束符。
		cachePtr->buffer = ( unsigned char * ) calloc( 1, cachePtr->size + 1 );
    //将文件内容读入缓存，也就是说缓存在存了文件的copy。fread第二个参数为每个对象的尺寸，此处就是缓存文件的尺寸，将缓存文件当做一个对象。第三个参数为对象的数目，这里就是1。同时fread读取文件流f是从当前position indicator开始。
		fread( cachePtr->buffer, cachePtr->size, 1, f );
    //将缓存的结尾设置结束标记
		cachePtr->buffer[ cachePtr->size ] = 0;

    //关闭文件流
		fclose( f );
    
    //iPhone上文件操作的完整过程？首先fopen(absoluteFileName),然后fseek指定文件流位置指示符，再其次使用ftell获得文件指示符的具体值，再次fread读取内容(从哪个文件流中读取多少内容到目的文件流），最后关闭。
    
		return cachePtr;


    #else
    #ifdef _WIN32

        //Parminder: added handling for Win32 platform
        #ifdef _WIN32
        GLchar * shaderCode;




#else
		unsigned char * shaderCode;
		#endif
		// Load contents of file into shaderCode here�
		ifstream inFile( filename, ifstream::in );
		if( !inFile ) {
			fprintf(stderr, "Error opening file: %s\n", filename );
			exit(1);
		}
		CACHE *cachePtr = ( CACHE * ) calloc( 1, sizeof( CACHE ) );
		#ifdef _WIN32
			cachePtr->buffer = (GLchar *)malloc(10000);
			memset(cachePtr->buffer, 0, 10000);
		#else
			cachePtr->buffer = (unsigned char *)malloc(10000);
		#endif
		int i = 0;
		while( inFile.good() ) {
			int c = inFile.get();
			cachePtr->buffer[i++] = c;
		}
		inFile.close();
		cachePtr->buffer[--i] = '\0';

		strcpy( cachePtr->filename, filename );
		cachePtr->position = 0;
		cachePtr->size	 = 10000;
		cachePtr->buffer[ 10000-1 ] = 0;
		return cachePtr;
	#else
		char fpath[ MAX_PATH ] = {""},
			 fname[ MAX_PATH ] = {""};

		unzFile		    uf;
		unz_file_info   fi;
		unz_file_pos    fp;
		strcpy( fpath, getenv( "FILESYSTEM" ) );
		uf = unzOpen( fpath );

		if( !uf ) return NULL;

		if( relative_path ) sprintf( fname, "assets/%s", filename );
		else strcpy( fname, filename );

		unzGoToFirstFile( uf );

		CACHE *cachePtr = ( CACHE * ) calloc( 1, sizeof( CACHE ) );

		unzGetFilePos( uf, &fp );

		if( unzLocateFile( uf, fname, 1 ) == UNZ_OK )
		{
			unzGetCurrentFileInfo(  uf,
								   &fi,
									cachePtr->filename,
									MAX_PATH,
									NULL, 0,
									NULL, 0 );

			if( unzOpenCurrentFilePassword( uf, NULL ) == UNZ_OK )
			{
				cachePtr->position = 0;
				cachePtr->size	 = fi.uncompressed_size;
				cachePtr->buffer   = ( unsigned char * ) realloc( cachePtr->buffer, fi.uncompressed_size + 1 );
				cachePtr->buffer[ fi.uncompressed_size ] = 0;

				while( unzReadCurrentFile( uf, cachePtr->buffer, fi.uncompressed_size ) > 0 ){}

				unzCloseCurrentFile( uf );

				unzClose( uf );
				return cachePtr;
			}
		}

		unzClose( uf );

		LOGI("Error: as");
		return NULL;

	#endif
	#endif
}

/*!
	Delete initialized CACHE.

	\param[in,out] cachePtr A valid CACHE structure object.

	\return Return a NULL CACHE pointer.
*/
CACHE *freeCache( CACHE *cachePtr )
{
    //先释放缓存数组，先行释放的理由是，缓存是程序自己寻址的内存，必须由程序主动释放，否则就内存泄漏
	if( cachePtr->buffer ) free( cachePtr->buffer );

    //再释放整个CACHE结构体
	free( cachePtr );
	return NULL;
}
