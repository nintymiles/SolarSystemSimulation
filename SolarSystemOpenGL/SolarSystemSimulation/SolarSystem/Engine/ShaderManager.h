#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#pragma once

//! Header File inclusion
#include "Constant.h"

//! Structure to handle shading language shader object.
//再封装shader对象，包括shader名称，类型和object handle
//为什么这样再封装？在shader数目较多时可以轻易区分出每个shader，让每个shader自身拥有足够多的辨识度
typedef struct
{
	//! Name assigned to shader.
    //vertex shader的名字
	char			name[ MAX_CHAR ];

	//! Shader type. [GL_VERTEX_SHADER or GL_FRAGMENT_SHADER]
    //每个shader的类型，将shader类型封装在shader对象中，维护类型信息以便随时查阅
	unsigned int	type;

	//! The Shader's unique ID.
    //由OpenGL ES返回的Shader Object ID，对应内存地址？a handle to a shader object
	unsigned int	sid;

} SHADER;


class ShaderManager
{

public:
    //! Constructor Shader class.
	ShaderManager(void);

    //! Destructor Shader class.
	~ShaderManager(void);

    //! Create a new shader of type mentioned.
	static SHADER*			ShaderInit( char *name, unsigned int type );

    //! Compilation process for Shader.
	static unsigned char	ShaderCompile( SHADER *shader, const char *code, unsigned char debug );

    //! Free the Shader.
	static void			    ShaderDelete( SHADER *shader );
};

#endif
