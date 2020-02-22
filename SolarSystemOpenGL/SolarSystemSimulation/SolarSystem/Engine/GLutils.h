#ifndef GLUTILS_H
#define GLUTILS_H
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cstdint>

#include "Constant.h"

#ifdef __APPLE__
    //! Apple iOS Header files for OpenGLES 3.0
    #include <OpenGLES/ES3/gl.h>
    #include <OpenGLES/ES3/glext.h>
    #define LOGI printf
    #define LOGE printf
#else
    #ifdef __ANDROID__
    //! Android Header files for NDK and OpenGLES 3.0
    #include <jni.h>
    #include <android/log.h>
    #include <GLES3/gl3.h>
    #include <GLES3/gl3ext.h>

    //! JNI Information and Error Log
    #define  LOG_TAG    "glOpenGLES3Native"
    #define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG,__VA_ARGS__)
    #define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
    #endif // End of __ANDROID__ scope
#endif

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>

using namespace std;

class GLUtils
{
public:
    GLUtils();
    
    static void checkGlError(const char* op);

    static int checkForOpenGLError(const char *, int);
    static void printGLInfo(bool printExtensions = false);
	
	//!	Replace the \ for / in a file path.
	static void modifyPath( char *path );

	//!
    static void extractPath( char *path, char *modifiedPath );


	//! Extract the file name from given file path.
	static void extractFileName( char *filepath, char *filename );
    
    // Get next next immediate power of 2. 
    static int nextPowerOf2 (int a);
    
    // HEX String to RGB
    static glm::vec3 hexStringToRGB(string hexString);
    
    
    //! Byte swap unsigned short
    static uint16_t swap_uint16( uint16_t val );
    
    //! Byte swap short
    static int16_t swap_int16( int16_t val );
    
    //! Byte swap unsigned int
    static uint32_t swap_uint32( uint32_t val );
    
    //! Byte swap int
    static int32_t swap_int32( int32_t val );

    //! Added 64bit byte swapping
    static int64_t swap_int64( int64_t val );
    static uint64_t swap_uint64( uint64_t val );
};
#endif // GLUTILS_H
