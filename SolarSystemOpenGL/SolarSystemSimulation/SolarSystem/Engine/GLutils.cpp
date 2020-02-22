#include "GLutils.h"

#ifdef _WIN32
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

GLUtils::GLUtils() {}

void GLUtils::checkGlError(const char* op) {
    //将GL error stack中的error逐一取出，直至stack中没有error
    for (GLint error = glGetError(); error; error= glGetError()) {
        printf("Error Occurs, %s() glError (0x%x)\n", op, error);
    }
}

int GLUtils::checkForOpenGLError(const char * file, int line) {
    //
    // Returns 1 if an OpenGL error occurred, 0 otherwise.
    //
    int    retCode = 0;
	#ifdef _WIN32
    GLenum glErr;

    glErr = glGetError();
    while (glErr != GL_NO_ERROR)
    {
        LOGI("glError in file %s @ line %d: %s\n", file, line, gluErrorString(glErr));
        retCode = 1;
        glErr = glGetError();
    }
	#endif
    return retCode;

}

void GLUtils::printGLInfo(bool printExtensions) {
	LOGI("\nGL_VENDOR:      %s\n", ( char * )glGetString( GL_VENDOR     ) );
	LOGI("\nGL_RENDERER:    %s\n"  , ( char * )glGetString( GL_RENDERER   ) );
	LOGI("\nGL_VERSION:     %s\n"  , ( char * )glGetString( GL_VERSION    ) );
	LOGI("\nGL_SHADING_LANGUAGE_VERSION:  %s\n"  , ( char * )glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	if (printExtensions){
		LOGI("\nGL_EXTENSIONS:  %s\n"  , ( char * )glGetString( GL_EXTENSIONS ) );
	}
}

//modifyPath函数将一般路径中可能包含的windows分割符'\\'替换为‘/’。
void GLUtils::modifyPath( char *path )
{
    
    unsigned int i = 0;
    //strlen函数获取path的char array长度
    //为何i，l用unsigned int？因为i表示索引，l表示字符的长度，都只可能为0或者正值，用unsigned int极为合适。
    unsigned int l = (unsigned int)strlen( path );
    
    //为什么此处使用while语句来做简单循环？首先此处的目的是遍历完整个path char array，找出其中的特殊字符并进行替换，只需要将其按照索引从头至尾遍历，其中根据索引取用值对比即可。而while用来遍历索引十分完美简单。用while避免了for的while转化。
    while( i != l )
    {
    //'\\'是一个字符，此处if语句的作用是将‘\\'替换为‘/’
        if( path[ i ] == '\\' ) {
            path[ i ] = '/';
        }
        ++i;
    }
}


/*!
 Extract path from a file path.
 
 \param[in] filepath The file path to use to extract the path.
 \param[in,out] path The return path.
 */
//提取qualified file name的标准（unix风格）绝对文件路径
//本函数关注重点，C语言利用内部指针确定memory size。
//将路径中最后一个“/”之前的路径copy到modifiedPaht变量中
void GLUtils::extractPath( char *path, char *modifiedPath )
{
    //将指针f的初始值定位NULL pointer
    char *f			= NULL;
    unsigned int p	= 0;
    
    modifyPath( path );
    
    //Finds the first occurrence of ch (after conversion to char as if by (char)ch) in the null-terminated byte string pointed to by str (each character interpreted as unsigned char). The terminating null character is considered to be a part of the string and can be found when searching for '\0'.
    //c函数strchr的作用？从string（byte string）中找到第一个‘ch'字符的出现位置指针
    //occurence指针的特殊性？这个pointer可以直接当做char array此pointer之后sub arr的指针。
    //这个语句没问题！所使用函数为strrchar反向字符查找函数，具体语意如下：
    //Finds the last occurrence of ch (after conversion to char as if by (char)ch) in the null-terminated byte string pointed to by str (each character interpreted as unsigned char). The terminating null character is considered to be a part of the string and can be found if searching for '\0'.
    f = strrchr( path, '/' );
    
    if( f )
    {
    //此处为何敢直接用pointers相减？使用两个pointers相减，意味着这两个pointer在一块连续的内存上，否则没有意义。f为path变量内存空间中的地址，故f-path(path为变量内存空间的开头地址）所得为f之前的空间数量，加上1包含“/”符号。
        p = (unsigned int)(f - path) + 1;
    //char *strncpy( char *dest, const char *src, std::size_t count )函数的作用？copies最多count个src的字符(byte string,包括null-terminator）到dest。
        strncpy( modifiedPath, path, p );
    //为何将path中“/”后的字符设置为0，则path字符就终结了，为何单独的0起到了null-terminator的作用？
        path[ p ] = 0;
    }
}


/*!
 Extract the file name from given file path.
 
 \param[in] path complete path contain file name.
 \param[in,out] filename extracted.
 
 */
//如何利用C语言函数从文件路径中获取文件名？利用strrchr获取被查找字符所处char array反向的第一个出现位置的字符指针，然后将指针加1，这个新的指针就是文件名的指针了。虽然这个内部指针并没有定义变量，这也暗示了指针变量的本质--内存的地址。
void GLUtils::extractFileName( char *filepath, char *filename )
{
    char *t = NULL;
    
    modifyPath( filepath );
    
    t = strrchr( filepath, '/' );
    
    //如果查找的字符存在，则从其指针顺延一个，用这个指针作为真实文件名的内存地址，直接用strcpy将其copy到filename变量即可。
    if( t ) strcpy( filename, t + 1 );
    else strcpy( filename, filepath );
}

/*!
 Get next best fit from the specified current 'number'.
 
 \param[in] number, input number.
 \param[in,out] next immediate power of two.
 
 */

//计算一个数字的2的冥的值，也即是这个数字是2的多少次冥。
int GLUtils::nextPowerOf2 (int number)
{
    int rval = 1;
    while (rval<number) rval*=2;
    return rval;
}

glm::vec3 GLUtils::hexStringToRGB(string hexString){
    int num = stoi(hexString,NULL,16);
    int red = num>>16&0xFF;
    int green = num>>8&0xFF;
    int blue = num&0xFF;
    return glm::vec3(red/255.0,green/255.0,blue/255.0);
}

//! Byte swap unsigned short
uint16_t GLUtils::swap_uint16( uint16_t val )
{
    return (val << 8) | (val >> 8 );
}

//! Byte swap short
//Byte Swap交换的目的是什么？移动端在与Android交互过程中，因为Android是从低位往高位读，iOS是从高位往低位读，所以交互数据需要进行转换。
int16_t GLUtils::swap_int16( int16_t val )
{
    //left/right shifting operator
    //为什么将int类型的byte前半部分和后半部分交换，然后将其值为0xFF mask提取？用0xFF提取是为了避免不同的系统对right operator在signed type上的不同而做，兼容性决定。
    //‘|’和‘&’优先级相同？
    //shifting operator的特征？原来的值并不改变，而是产生新的值。
    return (val << 8) | ((val >> 8) & 0xFF);
}

//! Byte swap unsigned int
uint32_t GLUtils::swap_uint32( uint32_t val )
{
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | (val >> 16);
}

//! Byte swap int
int32_t GLUtils::swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

int64_t GLUtils::swap_int64( int64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | ((val >> 32) & 0xFFFFFFFFULL);
}

uint64_t GLUtils::swap_uint64( uint64_t val )
{
    val = ((val << 8) & 0xFF00FF00FF00FF00ULL ) | ((val >> 8) & 0x00FF00FF00FF00FFULL );
    val = ((val << 16) & 0xFFFF0000FFFF0000ULL ) | ((val >> 16) & 0x0000FFFF0000FFFFULL );
    return (val << 32) | (val >> 32);
}
