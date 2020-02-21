#ifndef GRAPHICS_CONSTANT_H
#define GRAPHICS_CONSTANT_H


#ifndef M_PI
#define M_PI           3.14159265358979323846
#endif
#define LOG_TAG "Main"

#define LOG_TAGQI    __FILE__ ":" STR(__LINE__)
#define STR(x) STRINGIFY(x)
#define STRINGIFY(x) # x

#define MAX_CHAR 64

//! The depth of the modelview matrix stack.
//#define MAX_MODELVIEW_MATRIX	8

//! The depth of the projection matrix stack.
//投射矩阵stack的深度定义为2，因为一般只使用两种投射矩阵
#define MAX_PROJECTION_MATRIX	2

//! The depth of the model matrix stack.
//最大的模型矩阵stack的深度设置为8
#define MAX_MODEL_MATRIX	108

//! The depth of the view matrix stack.
#define MAX_VIEW_MATRIX	108

//! The depth of the texture matrix stack.
//最大texture matrix stack的深度也只设置为2？
#define MAX_TEXTURE_MATRIX		2

//定义路径的最大字符长度
#ifdef _WIN32
#else
#define MAX_PATH	256
#endif

//从Degree到Radius半径角的转换macro
#define DEG_TO_RAD	M_PI / 180.0f

#define RAD_TO_DEG	90.0f / M_PI

//缓存偏离的macro定义，为何使用NULL进行偏离？
#define BUFFER_OFFSET( x ) ( ( char * )NULL + x )

//限定数值区间的macro定义x belogs [low..high]
#define CLAMP( x, low, high ) ( ( x > high ) ? high : ( ( x < low ) ? low : x ) )

//! Maximum number of program possible.
#define MAX_PROGRAM_SIZE 20

//矩阵类型枚举，为什么定义Matrix_Type?
enum
{
	//! The modelview matrix identifier.
	//MODELVIEW_MATRIX = 0,
	
    //! The model matrix identifier.
	MODEL_MATRIX = 0,

	//! The view matrix identifier.
	VIEW_MATRIX = 1,

	//! The projection matrix identifier.
	PROJECTION_MATRIX = 2,

	//! The texture matrix identifier.
	TEXTURE_MATRIX	  = 3
};

//场景类型，为什么定义SceneType？
enum SceneType{
    //! The Scene identifier.
    SceneNone = -1,

    //! The Scene identifier.
    SceneMesh = 0,
    
    //! The Scene identifier.
    SceneButtons = 1,
};

//模型类型定义
enum ModelType{
	//! The Triangle Model identifier.
    None = -1,
    TriangleType = 0,
	CubeType = 1,
	TeapotType = 2,
	ObjFileType = 3,
	ThreeDSFileType = 4,
	TransformFeedbackType = 5,
	GridType = 6,
	ImageDemoType = 7,
    FontEnglish = 8,
    FontPunjabi = 9,
    FontThai = 10,
    FontArabic = 11,
    FontTamil = 12,
};

// Language Type for Localizaion support
enum LanguageType
{
    English = 0,
    Arabic  = 1,
    Thai    = 2,
    Punjabi = 3,
    Tamil = 4,
};

//投射矩阵类型枚举，支持两种：平行投射，透视投射
enum CameraType{
	//! The Ortho Camera type identifier.
	ortho = 0,

	//! The Perespective Camera type identifier.
	perespective
};

//图像格式枚举
enum ImageTypeEnum{
    BITMAP,
    PNG,
    JPEG,
    NO_IMAGE
};

#endif
