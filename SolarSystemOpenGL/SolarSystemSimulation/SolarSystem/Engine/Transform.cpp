#include "Transform.h"

///CPP的实现类只需要就具体的原型声明以C语言的方式实现即可，不需要再次重复书写

/*!
 Constructor for Transform class
 \param[in]  void.
 \return     None.
 */
//矩阵变换构造函数，初始将各个矩阵索引设置为0
Transform::Transform(void)
{
	TransformMemData.modelMatrixIndex       = 0;
	TransformMemData.viewMatrixIndex        = 0;
	TransformMemData.projectionMatrixIndex	= 0;
	TransformMemData.textureMatrixIndex		= 0;
}

/*!
    Destructor for Transform class
    \param[in]  void.
    \return     None.
*/
Transform::~Transform(void)
{
}

/*!
	Initialize the global  structure and set all the default OpenGLES machine
	states to their default values as expected by the engine. In addition the matrix
	stacks are created and the first index of the model view, projection and texture
	matrix are set to identity.
    \param[in]  None.
    \return void.
 
*/
//OpenGL ES的各种状态都被放置在变换的初始化函数中，这个实现有些奇怪。
//为什么要在transform相关的类中设置OpenGL pipline states。
void Transform::TransformInit( void )
{
    //memset将TransformMemData的每一位都设置0,实际验证也是这种效果
    //在本类的初始化函数中，将TransformMemData类public成员变量初始化，且每个byte都初始化为0.
    //TransformMemData的初始化方式及内容所决定的？首先在类初始化函数中将成员变量TransformMemData初始化，这可每个类实例都有一个独立的transformmemdata。其次将其中的内存全部填充默认值，这是保证在matrix缓存使用是不需要针对每个缓存再进行初始化动作。
	memset( &TransformMemData, 0, sizeof( TransformData ) );

	glEnable( GL_DEPTH_TEST );
    glDepthFunc( GL_LESS );
    
	glEnable( GL_CULL_FACE  );
	glEnable( GL_DITHER ); //禁用或者启用递色貌似没有看到明显的作用
	
	
    //glDepthRangef()的设置是iOS渲染出现问题的根源
    //为什么要设置depth range？限定OpenGL所接受的深度范围，如果viewport变换后所转化的depth值范围不在此之列，则尝试将其转化为所指定的区间内。只要所限定范围不在默认深度值之外（less时大于深度值，greater时小于深度值）都不影响深度比较
    glDepthRangef( 0.0f, 1.0f ); //远近平面范围？为了让最后被生成的Zw值在特定的区间内，由于n，f会被限制在【0，1】区间，当将n，f都设置为负值时，最终实际被应用的值范围是【0，0】，相当于最后生成的Zw值都一样0，则每个点都会被更新到framebuffer中。类似道理，如果将n，f值设置都大于1，则n,f会被限制为【1，1】，则最后的Zw值范围都为1，由于无法比默认depth值大，故无法更新framebuffer，导致没有显示（非fix function pipeline，只是为了将depth值夹在一个特定区间内）
    //glDepthMask( GL_TRUE );
    
	glClearDepthf( 1.0f ); //设置深度缓存的清除默认值为1。注意default value of depth会被限制在【0，1】，故无法设置这个范围之外的默认值。
	glCullFace ( GL_BACK );//剔除背向的三角形
	glFrontFace( GL_CCW  );//前向的三角形为反时针旋转的三角形
	glClearStencil( 0 ); //清除stencil缓存
	glStencilMask( 0xFFFFFFFF );

    //清理深度缓存、stencil缓存、颜色缓存
	glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT );

    //设置当前矩阵模式，并且将当前矩阵stack push 身份矩阵
	TransformSetMatrixMode( TEXTURE_MATRIX );
	TransformLoadIdentity();

	TransformSetMatrixMode( PROJECTION_MATRIX );
	TransformLoadIdentity();

	TransformSetMatrixMode( MODEL_MATRIX );
	TransformLoadIdentity();

	TransformSetMatrixMode( VIEW_MATRIX );
	TransformLoadIdentity();

    //OpenGL ES如果报错，打印错误信息到相应控制台。
	TransformError();
}

/*!
	Helper function check if the current GLES driver report an error. In case an error
	is generated it will printed on the Console (XCode) or LogChat (Eclispe).
    \param[in]  None.
    \return Return void.
*/
//GL执行矩阵变换时所可能出现错误的封装，对GL API执行所产生的错误应该放到Renderer执行的地方更合适。
void Transform::TransformError( void )
{
	unsigned int error;

	while( ( error = glGetError() ) != GL_NO_ERROR ) //只要获得的错误码不是GL_NO_ERROR则执行循环
	{
		char str[ MAX_CHAR ] = {""};

		switch( error )
		{
			case GL_INVALID_ENUM:
			{
				strcpy( str, "GL_INVALID_ENUM" );
				break;
			}

			case GL_INVALID_VALUE:
			{
				strcpy( str, "GL_INVALID_VALUE" );
				break;
			}

			case GL_INVALID_OPERATION:
			{
				strcpy( str, "GL_INVALID_OPERATION" );
				break;
			}

			case GL_OUT_OF_MEMORY:
			{
				strcpy( str, "GL_OUT_OF_MEMORY" );
				break;
			}
		}

		LOGI( "[ GL_ERROR ]\nERROR: %s\n", str );
	}
}

/*!
	Set the current matrix mode that you want to work with. Only the MODELVIEW_MATRIX,
	PROJECTION_MATRIX and TEXTURE_MATRIX are supported. Take note that contrairly to the default OpenGL behavior
	the TEXTURE_MATRIX is not implemented on a per channel basis, but on a global basis) contain in .h.

	If you wish to tweak the default matrix stack depth you can modify the following values: MAX_MODELVIEW_MATRIX,
	MAX_PROJECTION_MATRIX, MAX_TEXTURE_MATRIX.

	\param[in] mode The matrix mode that will be use as target.
*/
//矩阵模式只设置了3种，MODELVIEW，PROJECTION，TEXTURE。其中的含义就是mdoel和view矩阵是同质的，可以放在一起考虑
void Transform::TransformSetMatrixMode( unsigned int mode )
{
    TransformMemData.matrix_mode = mode;
}

/*!
	Set the current matrix set as target by the _set_matrix_mode to the
	identity matrix.
 \param[in]  None.
 \return None.
 
*/
//Transform的用法是先在TransformMemData中设置全局matrix模式，然后再从全局矩阵数组中获取地址，并初始化，最后赋值为身份矩阵
void Transform::TransformLoadIdentity( void )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			memset(TransformGetModelMatrix(), 0, sizeof(glm::mat4));
            //glmath中mat4的操作遵循对matrix的认知习惯
			glm::mat4* mv = TransformGetModelMatrix();
			(*mv)[0][0] = (*mv)[1][1] = (*mv)[2][2] = (*mv)[3][3] = 1.0;
			break;
		}
            
		case VIEW_MATRIX:
		{
			memset(TransformGetViewMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* mv = TransformGetViewMatrix();
			(*mv)[0][0] = (*mv)[1][1] = (*mv)[2][2] = (*mv)[3][3] = 1.0;
			break;
		}
            
		case PROJECTION_MATRIX:
		{
            //初始化projection matrix指针所指向的内存区域
			memset(TransformGetProjectionMatrix(), 0, sizeof(glm::mat4));
            //继续将projection matrix初始化为identity matrix
			glm::mat4* pm = TransformGetProjectionMatrix();
			(*pm)[0][0] = (*pm)[1][1] = (*pm)[2][2] = (*pm)[3][3] = 1.0;
			break;
		}

		case TEXTURE_MATRIX:
		{
			memset(TransformGetTextureMatrix(), 0, sizeof(glm::mat4));
			glm::mat4* tm = TransformGetTextureMatrix();
			(*tm)[0][0] = (*tm)[1][1] = (*tm)[2][2] = (*tm)[3][3] = 1.0;
			break;
		}
	}
}

/*!
 Return the modelview matrix pointer on the top of the modelview matrix stack.
 \param[in]  None.
 \return Return a 4x4 matrix pointer of the top most modelview matrix.
 */
//获得当前modelMatrixIndex所指向的缓存中的model_matrix的指针
glm::mat4* Transform::TransformGetModelMatrix( void )
{
	return &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex ];
}


/*!
 Return the modelview matrix pointer on the top of the modelview matrix stack.
 \param[in]  None.
 \return Return a 4x4 matrix pointer of the top most modelview matrix.
 */
glm::mat4* Transform::TransformGetViewMatrix( void )
{
	return &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex ];
}

/*!
	Return the projection matrix pointer on the top of the projection matrix stack.
    \param[in]  None.
	\return  Return a 4x4 matrix pointer of the top most projection matrix index.
*/
//返回顶部projection matrix stack的matrix指针
glm::mat4* Transform::TransformGetProjectionMatrix( void )
{
	return &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex ];
}

/*!
	Return the texture matrix pointer on the top of the texture matrix stack.
    \param[in]  None.
	\return  Return a 4x4 matrix pointer of the top most texture matrix index.
*/
glm::mat4* Transform::TransformGetTextureMatrix( void )
{
	return &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex ];
}

/*!
 Return the ModelViewProjection matrix. 
 \param[in]  None.
 \return matrix- Model View Projection matrix multiplication result.

 */
glm::mat4* Transform::TransformGetModelViewProjectionMatrix( void )
{
	TransformMemData.modelview_projection_matrix = *TransformGetProjectionMatrix() *
    *TransformGetViewMatrix() * *TransformGetModelMatrix();
	return &TransformMemData.modelview_projection_matrix;
}

/*!
 Return the ModelView matrix.
 \param[in]  None.
 \return matrix- Model View matrix multiplication result.
 
*/
glm::mat4* Transform::TransformGetModelViewMatrix( void )
{
	TransformMemData.modelview_matrix =
    *TransformGetViewMatrix() * *TransformGetModelMatrix();
	return &TransformMemData.modelview_matrix;
}

/*!
 Pushes the current matrix on the Matrix stack as per current active matrix.

 \param[in]  None.
 \return None.
 */
//实现对不同类型的matrix stack进行操作，维护每种matrix stack的matrix index。
void Transform::TransformPushMatrix( void )
{
	switch( TransformMemData.matrix_mode )
	{
        case MODEL_MATRIX:
		{
            //对matrix缓存索引外溢的检测
			if (TransformMemData.modelMatrixIndex >= MAX_MODEL_MATRIX){
				return;
			}
        
            //std::memcpy void* memcpy( void* dest, const void* src, std::size_t count );
            //Copies count bytes from the object pointed to by src to the object pointed to by dest. Both objects are reinterpreted as arrays of unsigned char. 重新解读为多个unsigned char序列？
            //If the objects overlap, the behavior is undefined.
            //If either dest or src is a null pointer, the behavior is undefined, even if count is zero.
            //If the objects are not TriviallyCopyable, the behavior of memcpy is not specified and may be undefined.
            //往matrix stack压栈动作的实现：先将压栈前顶部的元素copy到加1位置的元素位，这么做的目的？符合矩阵栈的构造原理，矩阵栈中的元素，每个上部的元素都是基于下部元素的累加，也即变换的累加？压栈即是将之前已经累加的matrix作为基础，便于在当前栈位置处进行矩阵串接。
			memcpy( &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex + 1 ],
                   &TransformMemData.model_matrix[ TransformMemData.modelMatrixIndex     ],
                   sizeof( glm::mat4 ) );
        
            //压栈后将矩阵index增加1
			++TransformMemData.modelMatrixIndex;
            
			break;
		}

		case VIEW_MATRIX:
		{
			if (TransformMemData.viewMatrixIndex >= MAX_VIEW_MATRIX){
				return;
			}
            
			memcpy( &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex + 1 ],
                   &TransformMemData.view_matrix[ TransformMemData.viewMatrixIndex     ],
                   sizeof( glm::mat4 ) );
            
			++TransformMemData.modelMatrixIndex;
            
			break;
		}

		case PROJECTION_MATRIX:
		{
			if (TransformMemData.projectionMatrixIndex >= MAX_PROJECTION_MATRIX){
				return;
			}

			memcpy( &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex + 1 ],
                    &TransformMemData.projection_matrix[ TransformMemData.projectionMatrixIndex     ],
					sizeof( glm::mat4 ) );

			++TransformMemData.projectionMatrixIndex;

			break;
		}

		case TEXTURE_MATRIX:
		{
			if (TransformMemData.textureMatrixIndex >= MAX_TEXTURE_MATRIX){
				return;
			}

			memcpy( &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex + 1 ],
                    &TransformMemData.texture_matrix[ TransformMemData.textureMatrixIndex     ],
					sizeof( glm::mat4 ) );

			++TransformMemData.textureMatrixIndex;
			break;
		}
	}
}

/*!
 Pop's the current matrix on the Matrix stack as per current active matrix.

 \param[in]  None.
 \return None.

 */
//矩阵stack的pop只是将matrix index的索引减1
void Transform::TransformPopMatrix( void )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			if (TransformMemData.modelMatrixIndex == 0){
				return;
			}
			--TransformMemData.modelMatrixIndex;
            
			break;
		}

		case VIEW_MATRIX:
		{
			if (TransformMemData.viewMatrixIndex == 0){
				return;
			}
			--TransformMemData.viewMatrixIndex;
            
			break;
		}

		case PROJECTION_MATRIX:
		{
			if (TransformMemData.projectionMatrixIndex == 0){
				return;
			}
			--TransformMemData.projectionMatrixIndex;

			break;
		}

		case TEXTURE_MATRIX:
		{
			if (TransformMemData.textureMatrixIndex == 0){
				return;
			}
			--TransformMemData.textureMatrixIndex;

			break;
		}
	}
}

/*!
 Set the matrix sent as the current matrix.
 
 \param[in] m Specify matrix which needs to set current.
 
 */
//将参数指定的矩阵设置为matrix stack中的顶部矩阵
void Transform::TransformLoadMatrix( glm::mat4 *m )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
            //将参数matrix中的内存值copy到栈顶部矩阵内存中
			memcpy(TransformGetModelMatrix(), m, sizeof(glm::mat4));
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			memcpy(TransformGetViewMatrix(), m, sizeof(glm::mat4));
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			memcpy(TransformGetProjectionMatrix(), m, sizeof(glm::mat4));

			break;
		}

		case TEXTURE_MATRIX:
		{
			memcpy(TransformGetTextureMatrix(), m, sizeof(glm::mat4));

			break;
		}
	}
}

/*!
 Multiple the matrix with the current matrix.
 
 \param[in] m Specify matrix which needs to multiply.
 
 */
//矩阵乘法的执行将当前栈顶部的矩阵累加当前参数中的矩阵变换，矩阵乘法的语意
void Transform::TransformMultiplyMatrix( glm::mat4 *m )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() *=  (*m);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() *=  (*m);
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*TransformGetProjectionMatrix() *=  (*m);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() *=  (*m);

			break;
		}
	}
}

/*!
	Translate the current matrix pointed for the current mode set
	as target by the _set_matrix_mode function.

	\param[in] x Specify the x coordinate of a translation vector.
	\param[in] y Specify the y coordinate of a translation vector.
	\param[in] z Specify the z coordinate of a translation vector.

*/
//WRT当前matrix stack的顶部矩阵执行平移操作
void Transform::TransformTranslate( float x, float y, float z )
{
	glm::vec3 v( x, y, z );

	switch( TransformMemData.matrix_mode )
	{            
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() = glm::translate( *TransformGetModelMatrix(), v);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() = glm::translate( *TransformGetViewMatrix(), v);
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*TransformGetProjectionMatrix() = glm::translate( *TransformGetProjectionMatrix(), v);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() = glm::translate( *TransformGetTextureMatrix(), v);

			break;
		}
	}
}

/*!
	Rotate the current matrix pointed for the current mode.

	\param[in] angle Specifies the angle of rotation, in degrees.
	\param[in] x Specify the x coordinate of a vector.
	\param[in] y Specify the y coordinate of a vector.
	\param[in] z Specify the z coordinate of a vector.

*/
//关联于当前stack顶部matrix，执行euler角变换
void Transform::TransformRotate( float angle, float x, float y, float z )
{
	if( !angle ) return;

	glm::vec3 v( x, y, z);

	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() = glm::rotate( *TransformGetModelMatrix(), angle, v );
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() = glm::rotate( *TransformGetViewMatrix(), angle, v );
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*TransformGetProjectionMatrix() = glm::rotate( *TransformGetProjectionMatrix(), angle, v );

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() = glm::rotate( *TransformGetTextureMatrix(), angle, v );

			break;
		}
	}
}

/*!
	Scale the current matrix pointed for the current mode.

	\param[in] x Specify scale factor along the x axis.
	\param[in] y Specify scale factor along the y axis.
	\param[in] z Specify scale factor along the z axis.

*/
//关联于顶部矩阵执行缩放操作
void Transform::TransformScale( float x, float y, float z )
{
	static glm::vec3 scale( 1.0f, 1.0f, 1.0f );

	glm::vec3 v( x, y, z );

	if( !memcmp( &v, &scale, sizeof( glm::vec3 ) ) ) return;


	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() = glm::scale(*TransformGetModelMatrix(), v);
            
			break;
		}
            
		case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() = glm::scale(*TransformGetViewMatrix(), v);
            
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			*TransformGetProjectionMatrix() = glm::scale(*TransformGetProjectionMatrix(), v);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() = glm::scale(*TransformGetTextureMatrix(), v);

			break;
		}
	}
}

/*!
	Return the result of the inverse and transposed operation of the top most modelview matrix applied
	on the rotation part of the matrix.

	\return Return the 3x3 matrix pointer that represent the invert and transpose
	result of the top most model view matrix.
*/
//warning-The origin implementation is incorrect, some fix is done here
void Transform::TransformGetNormalMatrix( glm::mat3* mat3Obj )
{
	glm::mat4 mat4Obj;
    //glm的反转操作
	mat4Obj = glm::inverse(*TransformGetModelViewMatrix());
    
    //glm的调换顺序操作,error code here，transpose should be just only applied on inversed MVMatrix
    //mat4Obj = glm::transpose(*TransformGetModelViewMatrix());
    //mat4Obj = glm::transpose(mat4Obj);

    //这个内存copy操作是否正确？
	memcpy(&(*mat3Obj)[0], &mat4Obj[0], sizeof(glm::mat3));
	memcpy(&(*mat3Obj)[1], &mat4Obj[1], sizeof(glm::mat3));
	memcpy(&(*mat3Obj)[2], &mat4Obj[2], sizeof(glm::mat3));
}


/*!
	Multiply the current matrix with an orthographic matrix.

	\param[in] left Specify the coordinates for the left vertical clipping planes.
	\param[in] right Specify the coordinates for the right vertical clipping planes.
	\param[in] bottom Specify the coordinates for the bottom horizontal clipping planes.
	\param[in] top Specify the coordinates for the top horizontal clipping planes.
	\param[in] clip_start Specify the distance to the nearer depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] clip_end Specify the distance to the farther depth clipping planes. This value is negative if the plane is to be behind the viewer.

*/
void Transform::TransformOrtho( float left, float right, float bottom, float top, float clip_start, float clip_end )
{
	switch( TransformMemData.matrix_mode )
	{
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}
		
        case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			break;
		}

		case PROJECTION_MATRIX:
		{
			glm::mat4	*matProjection = TransformGetProjectionMatrix();
			*TransformGetProjectionMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);
			*matProjection = glm::ortho(left, right, bottom, top, clip_start, clip_end);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() = glm::ortho(left, right, bottom, top, clip_start, clip_end);

			break;
		}
	}
}

/*!
	Multiply the current matrix with a scaled orthographic matrix that respect the current
	screen and and aspect ratio.

	\param[in] screen_ratio Specifies the screen ratio that determines the field of view in the Y direction. The screen ratio is the ratio of y (height) to y (width).
	\param[in] scale Determine that scale value that should be applied to the matrix.
	\param[in] aspect_ratio Specifies the aspect ratio that determines the field of view in the X direction. The screen ratio is the ratio of x (width) to y (height).
	\param[in] clip_start Specify the distance to the nearer depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] clip_end Specify the distance to the farther depth clipping planes. This value is negative if the plane is to be behind the viewer.
	\param[in] screen_orientation A value in degree that rotate the matrix. Should be synchronized with your device orientation.
*/
void Transform::TransformOrthoGrahpic( float screen_ratio, float scale, float aspect_ratio, float clip_start, float clip_end, float orientation )
{
	scale = ( scale * 0.5f ) * aspect_ratio;

	TransformOrtho( -1.0f, 1.0f, -screen_ratio, screen_ratio, clip_start, clip_end );
	TransformScale( 1.0f / scale, 1.0f / scale, 1.0f );

	if( orientation ) {
        TransformRotate( orientation, 0.0f, 0.0f, 1.0f );
    }
}

/*!
	Set up a perspective projection matrix.

	\param[in] fovy Specifies the field of view angle, in degrees, in the y direction.
	\param[in] aspect_ratio Specifies the aspect ratio that determines the field of view in the x direction. The aspect ratio is the ratio of x (width) to y (height).
	\param[in] clip_start Specifies the distance from the viewer to the near clipping plane (always positive).
	\param[in] clip_end Specifies the distance from the viewer to the far clipping plane (always positive).
	\param[in] screen_orientation A value in degree that rotate the matrix. Should be synchronized with your device orientation.

*/
//screen_orientation是一个考虑device orientation（设备有角度问题）的图像较偏设置，一般没用。但是确实对应了一种 特殊场景。
void Transform::TransformSetPerspective( float fovy, float aspect_ratio, float clip_start, float clip_end, float screen_orientation )
{
	glm::mat4 mat;

	switch( TransformMemData.matrix_mode )
	{
        //这种代码方式意味着，可以给MODEL_MATRIX stack中压入proejction matrix，以供特殊使用。
		case MODEL_MATRIX:
		{
			*TransformGetModelMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
			break;
		}
            
		case VIEW_MATRIX:
		{
			*TransformGetViewMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
			break;
		}
            
		case PROJECTION_MATRIX:
		{
			glm::mat4	*matProjection = TransformGetProjectionMatrix();
			*TransformGetProjectionMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);
			*matProjection = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);

			break;
		}

		case TEXTURE_MATRIX:
		{
			*TransformGetTextureMatrix() = glm::perspective(fovy, aspect_ratio, clip_start, clip_end);

			break;
		}
	}

}

void Transform::TransformSetView(glm::mat4 mat)
{
	//TransformMemData.view_matrix = mat;
}

/*!
	Define a viewing transformation.

	\param[in] eye Specifies the position of the eye point.
	\param[in] center Specifies the position of the reference point.
	\param[in] up Specifies the direction of the up vector.

*/
void Transform::TransformLookAt( glm::vec3 *eye, glm::vec3 *center, glm::vec3 *up )
{
    //此处3个vec3变量的定义是否多余？
	glm::vec3 f, s, u;
	glm::mat4 mat = glm::lookAt(*eye, *center, *up);
	//TransformSetView(mat);
	TransformMultiplyMatrix( &mat );
}

/*!
	Map object coordinates to window coordinates.

	\param[in] objextx Specify the object X coordinate.
	\param[in] objecty Specify the object Y coordinate.
	\param[in] objectz Specify the object Z coordinate.
	\param[in] modelview_matrix Specifies the current modelview matrix.
	\param[in] projection_matrix Specifies the current projection matrix.
	\param[in] viewport_matrix Specifies the current viewport matrix.
	\param[in] windowx Return the computed X window coordinate.
	\param[in] windowy Return the computed Y window coordinate.
	\param[in] windowz Return the computed Z window coordinate.

*/
//投射变换，此函数的作用是将对object coordinate变换为窗口坐标。此函数模拟了vertex shader以及viewport transform的处理。
int Transform::TransformProject( float objextx, float objecty, float objectz, glm::mat4 *modelview_matrix, glm::mat4 *projection_matrix, int *viewport_matrix, float *windowx, float *windowy, float *windowz )
{    
	glm::vec4 vin,
		 vout;

	vin.x = objextx;
	vin.y = objecty;
	vin.z = objectz;
	vin.w = 1.0f;

	Vec4MultiplyMat4( &vout, &vin, modelview_matrix );

	Vec4MultiplyMat4( &vin, &vout, projection_matrix );

	if( !vin.w ) return 0;

    //simulating perspective division
	vin.x /= vin.w;
	vin.y /= vin.w;
	vin.z /= vin.w;

    //将canonical square转换为unit square
	vin.x = vin.x * 0.5f + 0.5f;
	vin.y = vin.y * 0.5f + 0.5f;
	vin.z = vin.z * 0.5f + 0.5f;

    //在unit square上直接应用viewport矩阵
	vin.x = vin.x * viewport_matrix[ 2 ] + viewport_matrix[ 0 ];
	vin.y = vin.y * viewport_matrix[ 3 ] + viewport_matrix[ 1 ];

	*windowx = vin.x;
	*windowy = vin.y;
	*windowz = vin.z;

	return 1;
}

/*!
	Multiply a glm::vec4 by a glm::4x4 matrix.

	\param[in,out] dst A valid glm::vec4 pointer that will be used as the destination variable where the result of the operation will be stored.
	\param[in] v A valid glm::vec4 pointer.
	\param[in] m A valid glm::4x4 matrix pointer.
*/
//应该为M*c的格式，M为4x4矩阵，c为row vector。强调格式是因为矩阵乘法不可交换。
void Transform::Vec4MultiplyMat4( glm::vec4 *dst, glm::vec4 *v, glm::mat4 *m )
{
	dst->x = ( v->x * (*m)[ 0 ].x ) +
			 ( v->y * (*m)[ 1 ].x ) +
			 ( v->z * (*m)[ 2 ].x ) +
			 ( v->w * (*m)[ 3 ].x );

	dst->y = ( v->x * (*m)[ 0 ].y ) +
			 ( v->y * (*m)[ 1 ].y ) +
			 ( v->z * (*m)[ 2 ].y ) +
			 ( v->w * (*m)[ 3 ].y );

	dst->z = ( v->x * (*m)[ 0 ].z ) +
			 ( v->y * (*m)[ 1 ].z ) +
			 ( v->z * (*m)[ 2 ].z ) +
			 ( v->w * (*m)[ 3 ].z );

	dst->w = ( v->x * (*m)[ 0 ].w ) +
			 ( v->y * (*m)[ 1 ].w ) +
			 ( v->z * (*m)[ 2 ].w ) +
			 ( v->w * (*m)[ 3 ].w );
}

/*!
	Map window coordinates to object coordinates.

	\param[in] windowx Specify the window X coordinate.
	\param[in] windowy Specify the window Y coordinate.
	\param[in] windowz Specify the window Z coordinate.
	\param[in] modelview_matrix Specifies the current modelview matrix.
	\param[in] projection_matrix Specifies the current projection matrix.
	\param[in] viewport_matrix Specifies the current viewport matrix.
	\param[in] objextx Return the computed X object coordinate.
	\param[in] objecty Return the computed Y object coordinate.
	\param[in] objectz Return the computed Z object coordinate.

*/
//viewport矩阵实质只是传入了glViewPort()的四个参数？
//把窗口坐标映射为object frame中的标准化坐标。为vertex shader即viewport变换处理的逆向过程。
int Transform::TransformUnproject( float windowx, float windowy, float windowz, glm::mat4 *modelview_matrix, glm::mat4 *projection_matrix, int *viewport_matrix, float *objextx, float *objecty, float *objectz )
{
	glm::mat4 final;

	glm::vec4 vin,
		 vout;

	final = *projection_matrix**modelview_matrix;

	final = glm::inverse(final);
	vin.x = windowx;
	vin.y = windowy;
	vin.z = windowz;
	vin.w = 1.0f;

    //先讲坐标变换为viewport映射之前的状态
	vin.x = ( vin.x - viewport_matrix[ 0 ] ) / viewport_matrix[ 2 ];
	vin.y = ( vin.y - viewport_matrix[ 1 ] ) / viewport_matrix[ 3 ];

	vin.x = vin.x * 2.0f - 1.0f;
	vin.y = vin.y * 2.0f - 1.0f;
	vin.z = vin.z * 2.0f - 1.0f;

    //再应用mvp矩阵的反转
	Vec4MultiplyMat4( &vout, &vin, &final );

	if( !vout.w ) return 0;

    //为什么还要除w？因为缺乏viewport transform之前获得ndc的w坐标，故此处在此除以w，用以获得投射之前object frame的normalized coordinates。
	vout.x /= vout.w;
	vout.y /= vout.w;
	vout.z /= vout.w;

	*objextx = vout.x;
	*objecty = vout.y;
	*objectz = vout.z;

	return 1;
}
