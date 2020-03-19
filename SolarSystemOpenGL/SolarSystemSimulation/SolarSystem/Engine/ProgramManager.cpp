#include "ProgramManager.h"
#include <string.h>
#include "Cache.h"
#include "GLutils.h"

//在类的默认构造器中，将programList缓存初始化，所有的元素都清空为NULL，计数器置0
ProgramManager::ProgramManager(void) {
    
	for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
		programList[idx] = NULL;
	}
	programCounter = 0;
}

//在destructor函数中，将programList缓存中所有的program都释放，同时计数器置0
ProgramManager::~ProgramManager(void) {
	for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
		free (programList[idx]);
	}
	programCounter = 0;
}

/*!
 Initialize a new PROGRAM structure.

 \param[in] name The internal name to use for the new PROGRAM.

 \return Return a new PROGRAM structure pointer.
 */
//初始化一个新的program结构体，寻址内存，并初始化name和ProgramID为0
PROGRAM* ProgramManager::ProgramInit(char *name) {
	PROGRAM *program = (PROGRAM *) calloc(1, sizeof(PROGRAM));

	strcpy(program->name, name);
	program->ProgramID = 0;

	return program;
}

/*!
 Free a previously initialized PROGRAM structure.

 \param[in,out] program A valid PROGRAM structure pointer.

 \return Return a NULL PROGRAM structure pointer.
 */
//最完整的program资源free函数，将Unifrom缓存，VertexAttrib缓存，programID以及program结构的内存都逐一释放。
PROGRAM* ProgramManager::ProgramFree(PROGRAM *program) {
	if (program->UniformArray)
		free(program->UniformArray);

	if (program->VertexAttribArray)
		free(program->VertexAttribArray);

	if (program->ProgramID)
		ProgramDeleteId(program);

	free(program);
	return NULL;
}

/*!
 Internal function used to add a new uniform into a PROGRAM uniform database.

 \param[in,out] program A valid PROGRAM structure pointer.
 \param[in] name The name of the uniform.
 \param[in] type The variable type for this uniform.

 \return Return the newly created UNIFORM index inside the PROGRAM uniform database.
 */
//为对应program添加unifrom参数，用于将从program中查询到uniform参数管理起来。针对每一个uniform参数，重新寻址program中uniformArray空间，添加一个unifrom结构，将name，type等设置后，同时查找uniformLocation进行设置。设置后，返回其在uniformArray中的索引。
unsigned char ProgramManager::ProgramAddUniform(PROGRAM *program, char *name,
		unsigned int type) {
	unsigned char uniform_index = program->UniformCount;

	++program->UniformCount;

	program->UniformArray = (UNIFORM *) realloc(program->UniformArray,
			program->UniformCount * sizeof(UNIFORM));

	memset(&program->UniformArray[uniform_index], 0, sizeof(UNIFORM));

	strcpy(program->UniformArray[uniform_index].name, name);

	program->UniformArray[uniform_index].type = type;

	program->UniformArray[uniform_index].location = glGetUniformLocation(
			program->ProgramID, name);

	return uniform_index;
}

/*!
 Internal function used to add a new uniform into a PROGRAM vertex attribute database.

 \param[in,out] program A valid PROGRAM structure pointer.
 \param[in] name The name of the vertex attribute.
 \param[in] type The variable type for this attribute.

 \return Return the newly created VERTEX_ATTRIB index inside the PROGRAM vertex attribute database.
 */
//ProgramManager中使用ProgramAddVertexAttrib函数向VertexAttribArray本地数组中增加一个VertexArrib结构定义。首先分配内存，然后初始化array中的新增VertexAttrib
//ProgramAddVertexAttrib的隐含流程：shader先于管线program运行已经加载至其中，故而在添加vertex attrib时可以同时获取attrib location。也就是用此方法添加到程序中的vertex attrib包含一些运行信息。从而价值更大。
unsigned char ProgramManager::ProgramAddVertexAttrib(PROGRAM *program,
		char *name, unsigned int type) {
    //总数及当前所要添加的新条目的索引，这样的写法有些怪异
	unsigned char vertex_attrib_index = program->VertexAttribCount;

    //oddly但自然的写法，对program结构的VertexAttribCount数量增1
    //vertexattrib数目可以从program上进行查询？但是此处却由程序自身的逻辑维护，是否是多此一举？有些多此一举，GL_ACTIVE_ATTRIBUTES可以查询shader中激活的vertex attributes数目。
    //此处存在的价值是，维护用户设置的vertex attribute，而不管是否在shader中被使用（激活）
	++program->VertexAttribCount;

    //realloc的作用是什么？重新寻址给定区域的内存，这块区域必须是之前被malloc/calloc/realloc之类寻址过的内存，且内存没有被free过。
    //为什么内存的寻址和再寻址，以及free都要专门的函数来对应？
    //realloc新分配的内存是否初始化为0？对于重新分配的新内存（就内存对应内存之外的）并不初始化
    //alloc/realloc/calloc/dealloc都由全局来维持调用次序
    //这么多alloc/dealloc函数其实对应的都是内存这种专业设备存取的具体使用情形的对应工作。
    /**
     Reallocates the given area of memory. It must be previously allocated by malloc(), calloc() or realloc() and not yet freed with a call to free or realloc. Otherwise, the results are undefined.
     The reallocation is done by either:
     a) expanding or contracting the existing area pointed to by ptr, if possible. The contents of the area remain unchanged up to the lesser of the new and old sizes. If the area is expanded, the contents of the new part of the array are undefined.
     b) allocating a new memory block of size new_size bytes, copying memory area with size equal the lesser of the new and the old sizes, and freeing the old block.
     If there is not enough memory, the old memory block is not freed and null pointer is returned.
     If ptr is NULL, the behavior is the same as calling malloc(new_size).
     If new_size is zero, the behavior is implementation defined (null pointer may be returned (in which case the old memory block may or may not be freed), or some non-null pointer may be returned that may not be used to access storage).
     
     realloc is thread-safe: it behaves as though only accessing the memory locations visible through its argument, and not any static storage.
     A previous call to free or realloc that deallocates a region of memory synchronizes-with a call to any allocation function, including realloc that allocates the same or a part of the same region of memory. This synchronization occurs after any access to the memory by the deallocating function and before any access to the memory by realloc. There is a single total order of all allocation and deallocation functions operating on each particular region of memory.
     */
    //扩容VertexAttribArray容器
	program->VertexAttribArray = (VERTEX_ATTRIB *) realloc(
			program->VertexAttribArray,
			program->VertexAttribCount * sizeof(VERTEX_ATTRIB));

    //初始化新添加的vertex_attrib结构的内存
    //直接设置数组对应新增部分的内存
	memset(&program->VertexAttribArray[vertex_attrib_index], 0,
			sizeof(VERTEX_ATTRIB));

    //copy name to VertexAttrib->name fields
    //为什么char[]数组的赋值需要专门的函数strcpy？strcpy是一个比想象的要复杂的问题，一般情况下string（char[])之间的直接复制只是地址的引用，如果想将一个string的内容完全复制一份新的到另一个char[]则需要使用strcpy。
    //提出一个问题，如果一个char[4] a={""},另一char[4] b = "abcd",在a=b后，a原来所处地址的值或者内存如何处理？
    //strcpy_s函数没搞明白？
	strcpy(program->VertexAttribArray[vertex_attrib_index].name, name);

    //对vertexattrib的原生类型直接赋值
	program->VertexAttribArray[vertex_attrib_index].type = type;

    //从shader中获取attribute name对应的attribute index，然后赋值到vertexattrib->location之上
	program->VertexAttribArray[vertex_attrib_index].location =
			glGetAttribLocation(program->ProgramID, name);

	return vertex_attrib_index;
}

/*!
 Link the shader program.

 \param[in] program A valid PROGRAM structure pointer with vertex and fragment shader code loaded.
 \param[in] debug Determine if you need debugging functionalities while processing the linking stage of the shader.

 \return Return 1 if the shader program have succesfully been linked, else return 0.
 */

//在程序链接后将各种属性值获取并记录进入application stage中的数组中，则在程序运行中获取这些程序属性时会十分方便
//程序链接所附加的逻辑，对各种attribs的缓存式管理
//--------------------------------------------------------------------------------
//  ProgramLink方法的逻辑分为6部分：
//  第一部分:gl program object的管理
//  第二部分:program linking过程信息的输出
//  第三部分:program linking status的处理逻辑
//  第四部分:调试状态时，可以对program进行耗时的validate验证，且包含对program信息的再次获取处理
//  第五部分:获取program中active vertex attributes并进行管理缓存化维护管理的逻辑
//  第六部分:获取active unifroms并进行相应缓存化维护管理的逻辑
//--------------------------------------------------------------------------------
//


unsigned char ProgramManager::ProgramLink(PROGRAM *program,
		unsigned char debug) {
    //unsigned意味着在[0..x]范围内，也即i，type都是正整数
	unsigned int i = 0, type;
    //log直接定义为字符指针，这样定义的用意是log地址所指可以是单字符变量，也可以字符数组变量
    //指针类型变量既可以定义这个类型的scalar type，也可以是aggregate type（集合变量）？地址上的这些变量是如何区分的？区分很简单，当获得这个变量的地址，不管其是scalar type，还是aggregate type，当从这个地址引用时，会从这个地址开始一直到有null-terminator结束。这也是内存中的变量之间的区分？
	char *log, name[MAX_CHAR];
    //什么是int?int是基本数据类型，直接使用byte的二进制属性定义，基于2的power。尤其是unsigned，对于signed int，表达分为3个部分，value/padding/sign bits。
    //这些变量的作用，linking status，linking info length
	int status, len, total, size;

    //--------------------------------------------------------------------------------
    //对已经存在的GL program对象的使用所存在的bug分析？
    //   原来对已经存在的program对象的使用场景存在缺失问题，并且原来的program binding attrib callback机制
    //也仅是针对新生成的gl program对象。也就是说未考虑program对象正常的relink处理，比如以application方式改动
    //program中属性的location需要重新链接。
    //--------------------------------------------------------------------------------
    
    //如果programID已经存在，说明program已经被链接过，不再需要链接，直接返回成功标记1
    //如果program需要重新链接怎么办？
//    if (program->ProgramID)
//        return 1;
    //否则，生成gles program对象，并将ID赋给ProgramID
    //此处关于ProgramID的逻辑？先检查Program结构的主体是否已经被生成程序并链接，若已经生成，则直接返回链接成功标记，若没有ProgramID，则生成program，并链接Program结构中的shader，成功后返回1.
	//program->ProgramID = glCreateProgram();
    //这里ProgramID存在问题
    if(!program->ProgramID){
       program->ProgramID = glCreateProgram();
    }
    
    //挂接已经编译好的vertex shader，shaderID从vertexshader结构中获取
	glAttachShader(program->ProgramID, program->VertexShader->sid);
    //挂接已经编译好的fragment shader
	glAttachShader(program->ProgramID, program->FragmentShader->sid);

//	if (program->ProgramBindAttribCallback)
//		program->ProgramBindAttribCallback(program);

    //链接gles程序
	glLinkProgram(program->ProgramID);

    //如果debug开关打开，则输出链接信息
	if (debug) {
		glGetProgramiv(program->ProgramID, GL_INFO_LOG_LENGTH, &len);

		if (len) {
			log = (char *) malloc(len);

			glGetProgramInfoLog(program->ProgramID, len, &len, log);

#ifdef __IPHONE_4_0

			printf("[ %s ]\n%s", program->name, log );
#else
#ifdef _WIN32
			printf("[ %s ]\n%s", program->name, log );
#else
			__android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s",
					program->name, log);
#endif
#endif

			free(log);
		}
	}

    //获得链接状态值，状态值用来表示是否链接成功
    //&操作符是什么？
    //&在C语言中是address operator，用来获取&x用来获取到一个变量的指针，也就是获取一个变量的地址。
    //&和*操作符的区别？*是指针操作符，指得是一个地址变量所对应的值，而&是address操作符，用于获取一个变量的指针（地址）。&操作符的操作对象是普通变量，*操作符的操作对象是指针变量。但都是an unary操作符。
	glGetProgramiv(program->ProgramID, GL_LINK_STATUS, &status);

    //goto语句？起到程序跳转的作用。
    //为什么需要goto语句？碰到异常情形但又不想打断主流程的逻辑，就将异常情形跳转处理，这些形式好像对应一些更高级语言的exception机制。
    //goto语句所代表的语言处理机制？goto导致生成了一个直接到达特定地点的分支，任何语言都有这样一种机制（比如java的异常机制），这种机制是无条件地立即发生在goto语句执行上。
	if (!status) {
		goto delete_program;
	}

    //如果处于调试状态，则进行program验证，为什么正常情况不进行program验证？
	if (debug) {
		glValidateProgram(program->ProgramID);

		glGetProgramiv(program->ProgramID, GL_INFO_LOG_LENGTH, &len);

		if (len) {
			log = (char *) malloc(len);

			glGetProgramInfoLog(program->ProgramID, len, &len, log);

			//Parminder: Need a preprocessor flag for Android case at present #else is being used for Android
#ifdef __IPHONE_4_0
			printf("[ %s ]\n%s", program->name, log );
#else
#ifdef _WIN32
			printf("[ %s ]\n%s", program->name, log );
#else
			__android_log_print(ANDROID_LOG_ERROR, "", "[ %s ]\n%s",
					program->name, log);
#endif
#endif

			free(log);
		}

        //获得Program validating status
        //validating status的获取时机？在glValidateProgram执行之后，是对这个动作的状态的获取。只不过是延迟的执行。
		glGetProgramiv(program->ProgramID, GL_VALIDATE_STATUS, &status);

        //如果gl program验证为无效，则直接跳转到删除程序。
		if (!status)
			goto delete_program;
	}

    //获取激活attributes数目，并写入total值，这个程序为什么有点冗长？其意图是整合program的所有相关知识
	glGetProgramiv(program->ProgramID, GL_ACTIVE_ATTRIBUTES, &total);

    //将从shader中获取的active vertex attribute值加到vertex attrib数组中
    //从active_attributes中获取的type定义在specification有对应类型枚举值。这些类型枚举值是OpenGL支持的类型。
    //问题，OpenGL中GLEnums所定义的active_uniforms/attributes具体枚举值从何处查找和确认？OpenGL中GLEnum类型只是uint32_t(32位unsigned int)，也就是说不是c/cpp中enum类型定义。在OpenGL ES规范中glGetActiveUniform/glGetActiveAttrib函数所定义的类型GL_FLOAT32,GL_FLOAT_VEC2...都是#define定义的macros，每一个enumerant值都被固化指定，不会轻易变化。这样的后果就是这些定义可以被精确的返回，而且可以使用enumeration的语法实现。不好的地方就是可能无法借助一些智能提示，而要注意specification中的指示。
	i = 0; //计数重置
	while (i != total) {
		glGetActiveAttrib(program->ProgramID, i, MAX_CHAR, &len, &size, &type,
				name);

		ProgramAddVertexAttrib(program, name, type);

		++i;
	}

    //获取激活uniform的数目
	glGetProgramiv(program->ProgramID, GL_ACTIVE_UNIFORMS, &total);

    //遍历将active uniforms加入uniform数组中
	i = 0;
	while (i != total) {
		glGetActiveUniform(program->ProgramID, i, MAX_CHAR, &len, &size, &type,
				name);

		ProgramAddUniform(program, name, type);

		++i;
	}

	return 1;

    //delete——program goto point被放置在正常返回之后，被当作一种异常结构
	delete_program:
        ProgramDeleteId(program);

	return 0;
}

/*!
 Retrieve a vertex attribute location.

 \param[in] program A valid PROGRAM structure pointer.
 \param[in] name The name of the vertex attribute.

 \return Return the vertex attribute location.
 */

//attrib地址从ProgramManager维护的attrib缓存中获取，而attrib缓存则是由ProgramManager在gl program对象被链接后，从prgram中直接获取的有效属性。
char ProgramManager::ProgramGetVertexAttribLocation(PROGRAM *program,
		char *name) {
	unsigned char i = 0;

	while (i != program->VertexAttribCount) {
		if (!strcmp(program->VertexAttribArray[i].name, name)) {
			return program->VertexAttribArray[i].location;
		}

		++i;
	}

	return -1;
}

/*!
 Retrieve a uniform location.

 \param[in] program A valid PROGRAM structure pointer.
 \param[in] name The name of the uniform.

 \return Return the uniform location.
 */
char ProgramManager::ProgramGetUniformLocation(PROGRAM *program, char *name) {
	unsigned char i = 0;

	while (i != program->UniformCount) {
		if (!strcmp(program->UniformArray[i].name, name)) {
			return program->UniformArray[i].location;
		}

		++i;
	}

	return -1;
}

/*!
 Delete the GLSL program id attached to the PROGRAM structure.

 \param[in,out] program A valid PROGRAM structure pointer.
 */
//删除program通过programID,同时将PROGRAM structure中的programId设置为not occupied
void ProgramManager::ProgramDeleteId(PROGRAM *program) {
	if (program->ProgramID) {
		glDeleteProgram(program->ProgramID);

		program->ProgramID = 0;
	}
}

//为什么对program的回调方法定义进行注释？
//因为这是由program的机制决定的,当program确定一个管线时，管线的任何部分的改动都要求program重新链接，而ProgramLink方法在程序链接之后会主动对attrib/uniform缓存进行更新，故这里不再需要返回当前的program对象，从而实现对其的参数去用。
/*!
 Set a PROGRAM for drawing. If a ProgramDrawCallback is attached to the structure
 the execution pointer will be redirected to it so you can set/upadte the uniforms
 variable for the current PROGRAM.

 \param[in,out] program A valid PROGRAM structure pointer.
 */
//void ProgramManager::ProgramDraw(PROGRAM *program) {
//	glUseProgram(program->ProgramID);
//
//	if (program->ProgramDrawCallback)
//		program->ProgramDrawCallback(program);
//}

/*!
 Helper function to load and compile a  shader file.

 \param[in,out] program A valid PROGRAM structure pointer.
 \param[in] ProgramBindAttribCallback A valid PROGRAMBINDATTRIBCALLBACK function pointer.
 \param[in] ProgramDrawCallback A valid PROGRAMDRAWCALLBACK function pointer.
 \param[in] filename The filename of the  file.
 \param[in] debug_shader Enable or disable debugging functionalities.
 \param[in] relative_path Determine of the filename is a relative (1) or absolute (0) path.

 \return Return 1 if the PROGRAM have successfully loaded and compiled the  shader file.
 */


PROGRAM* ProgramManager::ProgramLoad(char* name, char* vertexShaderPath, char* fragmentShaderPath){
    PROGRAM* program = NULL;
    //若name所指定的program存在，则直接返回此program，这是一种缓存式的使用。
    if (! ( program = Program( name ) ) )
    //如果没有已经存在的program，则初始化一个program，并挂载vertex/fragment shader,并重新编译。
    {
        //生成一个新的program结构对象实例
        program = ProgramInit( name );
        AddProgram( program );
        program->VertexShader	= ShaderManager::ShaderInit( vertexShaderPath,	GL_VERTEX_SHADER	);
        program->FragmentShader	= ShaderManager::ShaderInit( fragmentShaderPath, GL_FRAGMENT_SHADER	);
        
        /////////// Vertex shader //////////////////////////
        //reserveCache的作用是将shader文件路径读取，将文件内容读入CACHE结构中，包含内容buffer，文件大小，文件名。
        CACHE *m = reserveCache( vertexShaderPath, true );
        
        if( m ) {
            //exit()函数及exit_code的含义？当exit_code为0以外的值时，表示非正常退出，1，2，3只是指示不同的异常状态。
            //control is returned to the host environment. If exit_code is 0 or EXIT_SUCCESS, an implementation-defined status indicating successful termination is returned. If exit_code is EXIT_FAILURE, an implementation-defined status indicating unsuccessful termination is returned. In other cases implementation-defined status value is returned.
            if( !ShaderManager::ShaderCompile( program->VertexShader, ( char * )m->buffer, 1 ) ) exit( 1 );
            //用完缓存后直接释放，为什么这么着急释放？放置占用内存资源
            freeCache( m );
        }
        
        /////////// Fragment shader //////////////////////////
        m = reserveCache( fragmentShaderPath, true );
        if( m ) {
            if( !ShaderManager::ShaderCompile( program->FragmentShader, ( char * )m->buffer, 1 ) ) exit( 2 );
            freeCache( m );
        }
    
        //执行program linking动作，如果不成功则异常退出
        if( !ProgramLink( program, 1 ) ) exit( 3 );
    }
    return program;
}
/*!
 \Get the program as per specified program name.

 \param[in,out] program A valid PROGRAM structure pointer.

 \return Return 0 if value already present, 1 if added successfully, 2 not added due to overflow.
 */
//根据指定的program name从缓存中取出program对象structure pointer
PROGRAM* ProgramManager::Program(char* programName) {
    //以遍历的方式查找每个program元素的名字是否匹配，如果匹配则返回此program的指针。
	for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
		if (programList[idx] && !strcmp(programList[idx]->name, programName)) {
			return programList[idx];
		}
	}
	return NULL;
}

/*!
 \Get the program as per specified shader ID.
 
 \param[in,out] program A valid PROGRAM structure pointer.
 
 \return Return 0 if value already present, 1 if added successfully, 2 not added due to overflow.
 */
PROGRAM* ProgramManager::Program(int shaderID) {
    for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
        if (programList[idx] && programList[idx]->ProgramID == shaderID) {
            return programList[idx];
        }
    }
    return NULL;
}

/*!
 \Add the Program into buffer.

 \用param[in,out]表示参数类型是输入输出类型
 \param[in,out] program A valid PROGRAM structure pointer.

 \描述返回值的具体意义
 \return Return 0 if value already present, 1 if added successfully, 2 not added due to overflow.
 */

//为什么返回值定义为unsigned char？因为char类型是8bit，对于有限数量的返回值来说不浪费空间。
//为何program要专门添加到缓存array中管理？方便随时索引已经存在的program
unsigned char ProgramManager::AddProgram(PROGRAM* program) {
	bool isPresent = false;
    //MAX_PROGRAM_SIZE = 20，最大可能的程序pipline数量设置为20
	for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
        //strcmp函数的作用？以字典顺序比较两个byte string，如果没有差异，则返回0.
        //如果programList在当前索引处不为空，且两个name比较相等则说明program已经存在于缓存中。
		if (programList[idx]
				&& !strcmp(programList[idx]->name, program->name)) {
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
		for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
			if (!programList[idx]) {
				programList[idx] = program;
				programCounter++;
				return 1; // List added sucessfully
			}
		}
        //若执行到for语句之后，则说明缓存空间已经用尽，无法再加添加入缓存。
		return 2; // No Space in the list to add
	}
}

/*!
 \Remove the program from buffer.

 \param[in,out] program A valid PROGRAM structure pointer.

 \return Return 1 if the PROGRAM have successfully removed else 0 for no program found in the list.
 */
unsigned char ProgramManager::RemoveProgram(PROGRAM* program) {
	for (int idx = 0; idx < MAX_PROGRAM_SIZE; idx++) {
        //首先查找program元素是否存在，存在则执行释放逻辑。
		if (programList[idx]
				&& !strcmp(programList[idx]->name, program->name)) {
            //这里有无问题？program对象被直接释放，而没有先行释放shaders，id等
			//free (programList[idx]);
            //改为ProgramFree()函数释放，内存更干净。
            ProgramFree(programList[idx]);
            //释放program strtucture元素后记得将计数器减1
			programCounter--;
            //将array当前索引处设置为null，以便缓存的充分使用
			programList[idx] = NULL;
			return 1;
		}
	}
	return 0;
}
