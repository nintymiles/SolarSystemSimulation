#ifndef PROGRAM_MANA_H
#define PROGRAM_MANA_H

#pragma once
#include "ShaderManager.h"

//! Structure to deal with GLSL uniform variables.
typedef struct
{
	//! The name of the uniform.
	char			name[ MAX_CHAR ];

	//! The variable type for this uniform.
    // uniform可使用的数据类型有哪些，float,int,bool,vector,matrix,collection???所有基础数据类型
	unsigned int	type;

	//! The location id maintained by GLSL for this uniform.
    //uniform index
	int				location;

	//! Determine if the uniform is constant or shoud be updated every frame.
	unsigned char	constant;

} UNIFORM;


//! Structure to deal with vertex attribute variables.
typedef struct
{
	//! The name of the vertex attribute.
	char			name[ MAX_CHAR ];

	//! The variable type for this vertex attribute.
	unsigned int	type;

	//! The location of the id maintained GLSL for this vertex attribute.
	int				location;

} VERTEX_ATTRIB;


//! Draw callback function prototype.
typedef void( PROGRAMDRAWCALLBACK( void * ) );

//! Callback function prototypes for binding vertex attribute before compilation.
typedef void( PROGRAMBINDATTRIBCALLBACK( void * ) );


//! Structure to easily handle GLSL programs.
//program结构的封装所包含对象的理解，programId，unifrom/count/array，shaders，vertex/count/array program名字？为什么包含这么多对象？这是从program对象的实际操作范围及关联内容出发的，program代表的是一条pipeline，而这个pipeline上，vertex/fragment shaders是组成部分，uniform/vertex 是两种不同的输入数据对象。programID是这条pipeline在内存中的handle，而名称name[]则是我们对所封装的这条pipeline的命名。从这个封装上我们也看到了其上聚集了GL program的所有主要概念。
//这个structure可以轻松的操纵program，凭什么？因为这个structure封装了program的几乎所有概念对象，通过这个program对整体pipelien可以做到操作。

//GLES program对象是GL API操纵的基础对象。
typedef struct
{
	//! The internal name to use for the shader program.
    //初始化program的对应名称，允许最大字符数目64，为什么要给program起名字？索引的需要？
	char						 name[ MAX_CHAR ];

	//! The vertex shader. Shader封装，为什么封装？
	SHADER						 *VertexShader;

	//! The fragment shader.
	SHADER						 *FragmentShader;

	//! The internal GLES program id for the shader program.
	unsigned int				 ProgramID;

	//! The number of uniforms.
	unsigned char				 UniformCount;

	//! Array of UNIFORM variables.
	UNIFORM						 *UniformArray;

	//! The number of vertex attributes.
	unsigned char				 VertexAttribCount;

	//! Array of vertex attributes.
	VERTEX_ATTRIB				 *VertexAttribArray;

} PROGRAM;


class ProgramManager
{
public:
	ProgramManager(void);
	~ProgramManager(void);

public:
	PROGRAM *ProgramInit( char *name );

	PROGRAM *ProgramFree( PROGRAM *program );

	unsigned char ProgramLink( PROGRAM *program, unsigned char debug );

	char ProgramGetVertexAttribLocation( PROGRAM *program, char *name );

	char ProgramGetUniformLocation( PROGRAM *program, char *name );

	void ProgramDeleteId( PROGRAM *program );

	void ProgramReset( PROGRAM *program );

    PROGRAM* ProgramLoad(char* name, char* vertexShaderPath, char* fragmentShaderPath);
	unsigned char ProgramLoad( PROGRAM *program, PROGRAMBINDATTRIBCALLBACK	*ProgramBindAttribCallback, PROGRAMDRAWCALLBACK	*ProgramDrawCallback, char *filename, unsigned char	debug_shader, unsigned char relative_path );

	unsigned char ProgramAddUniform( PROGRAM *program, char *name, unsigned int type );

	unsigned char ProgramAddVertexAttrib( PROGRAM *program, char *name, unsigned int type );

    PROGRAM* Program( char* program );

    PROGRAM* Program( int shaderID );

	unsigned char AddProgram( PROGRAM* program );

	unsigned char RemoveProgram( PROGRAM* program );

	inline unsigned char ProgramCount() { return programCounter; }

private:
	//! We need some mechanism to recognize the program for each model.
	//! Parminder temporarily using a array, must use a list instead.
	PROGRAM* programList[20]; 

	//! Keeps track of the number of program in the renderer.
    //program的数量是通过程序自身逻辑进行维护的，没有借助外部程序。原因：program运行的时候很少和别的program交互，每个管线都是独立运行，而此处自己维护则是从总体上对使用program也即管线条目的追踪。
	unsigned char programCounter;
};

#endif
