#include "ShaderManager.h"
#include <stdio.h>
//在Mac OS X上大小写忽略也可以编译成功，OS X上可以忽略大小写
#include "glutils.h"
#include <stdlib.h>
#include <string.h>

//ShaderManager的实现定义，一般情况下CPP文件中的class实现都是在类定义的外部
//h文件中的类定义虽然只有sketch，但是class body在header文件中存在
/*!
	Default Constructor ShaderManager
	
	\param[in] NONE.
	
	\return NONE.
*/
ShaderManager::ShaderManager(void)
{
}

/*!
	Default Destructor ShaderManager
	
	\param[in] NONE.
	
	\return NONE.
*/
ShaderManager::~ShaderManager(void)
{
}

/*!
	Create a new SHADER structure.
	
	\param[in] name The internal name for the new SHADER structure pointer.
	\param[in] type The type of shader the structure will deal with. (Either GL_VERTEX_SHADER or
	GL_FRAGMENT_SHADER)
	
	\return Return a new SHADER structure pointer.
*/
//包含shader struct的初始化代码，初始化过程，先用calloc为struct分配内存地址，然后将名称copy到name变量char[]中，再设置type的值,最后返回struct地址。
//为什么要用calloc分配地址?calloc用来为多个对象array分配地址，且将所有寻址的字节初始化为0。calloc是线程安全的，此处将其用来初始化单个shader对象，主要是为了使用thread-safe特征。
/**
 Allocates memory for an array of num objects of size size and initializes all bytes in the allocated storage to zero.
 If allocation succeeds, returns a pointer to the lowest (first) byte in the allocated memory block that is suitably aligned for any object type.
 If size is zero, the behavior is implementation defined (null pointer may be returned, or some non-null pointer may be returned that may not be used to access storage)
 calloc is thread-safe: it behaves as though only accessing the memory locations visible through its argument, and not any static storage.
 A previous call to free or realloc that deallocates a region of memory synchronizes-with a call to calloc that allocates the same or a part of the same region of memory. This synchronization occurs after any access to the memory by the deallocating function and before any access to the memory by calloc. There is a single total order of all allocation and deallocation functions operating on each particular region of memory.
 
 */
//用shaderInit方法新生成一个Shader对象，包括寻址和基本属性的初始设置，但是不包含实际shader对象的生成。
//ShaderInit方法的存在价值：标准化自定义Shader结构的初始化，简化与规范Shader结构的使用
//用ShaderManager以ObjC的方式生成一个Shader struct对象。
SHADER *ShaderManager::ShaderInit( char *name, unsigned int type )
{
    //此处calloc的作用就是寻址一个SHADER结构的内存空间，并且初始化为0
	SHADER *shader = ( SHADER * ) calloc( 1, sizeof( SHADER ) );
    //将name拷贝到shader->name地址
	strcpy( shader->name, name );
    //对type（int类型）直接赋值
	shader->type = type;
	
	return shader;
}

/*!
	Compile a VERTEX or FRAGMENT shader code.
	
	\param[in,out] shader A valid SHADER structure pointer.
	\param[in] code The code to compile for the current SHADER.
	\param[in] debug Enable (1) or disable (0) debugging functionalities while compiling the shader.
	
	\return Return 1 if the shader code compile successfully, else return 0.
*/
//ShaderManager的工作：封装了shader管理的整个过程，对shader creation,shader source,shader compilation,shader info retrieving,shader delete等整个过程进行了封装。尤其对shader 错误日志的输出进行了友好的封装，可以将shader type配合shader log一并打印出来，ShaderManager还封装了对编译错误的shader的直接delete。
//ShaderCompile方法的评价：几乎封装了OpenGL shader对象管理的整个周期，感觉是一种好的封装。用debug控制debug信息的输出。
//用debug标记控制Debug信息的方式的用意？可以方便的快速切换debug信息的输出
//Shader自定义对象以一种inout的参数方式使用
unsigned char ShaderManager::ShaderCompile( SHADER *shader, const char *code, unsigned char debug )
{
    //用{}(bracket)进行初始化，这已是现代编译器的标配
	char type[ MAX_CHAR ] = {""};
	
    //直接用int定义status，status当作bool使用
    //cpp中的bool类型？When we assign one of the nonbool arithmetic types to a bool object, the result is false if the value is 0 and true otherwise.
    //也就是说cpp中bool类型几乎是integer类型的另一种角度用法，用int完全可以代替bool，以0为false，而其它为true。完全等价于bool
	int loglen,
		status;
	
    //如果shader中包含真正的shader对象，则返回编译失败，暗含的意思是已存在的shader不能再编译
	if( shader->sid ) return 0;
	
	shader->sid = glCreateShader( shader->type );

    //&符号在参数中的作用：
    glShaderSource( shader->sid, 1, &code, NULL );

    glCompileShader( shader->sid );

	if( debug )
	{
        //为什么对char[]类型要用strcpy来赋值name string,strcpy的作用是将src参数指定的byte string copy to dest指定的character array中，也就是strcpy针对的就是char array的操作。
        //Copies the null-terminated byte string pointed to by src, including the null terminator, to the character array whose first element is pointed to by dest.
        //The behavior is undefined if the dest array is not large enough. The behavior is undefined if the strings overlap. The behavior is undefined if either dest is not a pointer to a character array or src is not a pointer to a null-terminated byte string.
        //为什么进行char[]的赋值就要动这么大的周章？因为C语言系统的数组间的数据copy操作的特征决定的，同时没有针对char[]或者“String”类型进行特别优化。
		if( shader->type == GL_VERTEX_SHADER ) strcpy( type, "GL_VERTEX_SHADER" );
		else strcpy( type, "GL_FRAGMENT_SHADER" );
		
		glGetShaderiv( shader->sid, GL_INFO_LOG_LENGTH, &loglen );
		
		if( loglen )
		{
			char *log = ( char * ) malloc( loglen );

			glGetShaderInfoLog( shader->sid, loglen, &loglen, log );
			
        //针对不同的操作系统进行日志输出
        //在iOS平台，针对每个OS版本基本都有宏定义，从__IPHONE_2_0一直到__IPHONE_12_0，TVOS，WATCHOS都有单独的版本定义，这意味着只要是针对iPhone/iPad的应用都可以确认这个宏定义，而不必担心真正的版本号。
        //为什么宏定义会以不断添加新宏的方式定义？一方面可以使用具体版本的宏定义进行精确的版本定位，另一方面也可以保证兼容性。
        //此处针对iOS的宏所起到的作用是，只要是iOS 4及之上的版本都可以被确认，从而应用定义的输出功能。
        //输出具体信息时，shader对象中保存的shader name和type可以发挥出用处。
			#ifdef __IPHONE_4_0			
				printf("[ %s:%s ]\n%s", shader->name, type, log );
			#else
				#ifdef _WIN32
					printf("[ %s:%s ]\n%s", shader->name, type, log );
				#else
					__android_log_print( ANDROID_LOG_ERROR, "", "[ %s:%s ]\n%s", shader->name, type, log );
				#endif
			#endif
			
			free( log );
		}
	}
    
    //获取真正shader编译状态，从编译状态决定shaderCompiler的返回值
    //编译状态决定了shader是否看起来可以工作，如果不具备基本可运行状态，则当然没有存在的价值，可以直接删除掉这个对象。
    glGetShaderiv( shader->sid, GL_COMPILE_STATUS, &status );

	if( !status )
	{
		ShaderDelete( shader );
		return 0;
	}

	return 1;
}

/*!
	Delete the shader by internal id maintained by GLES.
	
	\param[in,out] shader A valid SHADER structure pointer.
*/
//删除Shader结构实例对象所维护ID对应的GLES shader对象
//为何用ShaderDelete来命名？
//::操作符的含义？scope operator,在此处代表class scope。
//::scope operator的作用？当在class外部定义函数时，由于位于class定义外部，故无法访问类的成员。这时借助scope operator就可以如在class定义内部一样访问其他类成员。 scope operator在类上的作用相当于提供了在外部进行成员定义时提供了与类内部成员进行直接沟通的机制。
//在本函数处::scope operator的含义是在类外部定义类的静态成员函数ShaderDelete()
void ShaderManager::ShaderDelete( SHADER *shader )
{
    //只要GLES shader对象ID存在，就执行GLES shader的删除动作，并将shader结构的sid置为0
	if( shader->sid )
	{
		glDeleteShader( shader->sid );
		shader->sid = 0;
	}
}

