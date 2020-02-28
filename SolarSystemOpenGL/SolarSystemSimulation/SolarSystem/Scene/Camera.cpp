#include "Camera.h"
#include "Scene.h"
#include "quaternion.hpp"
#define DEGREE_TO_RADIAN	M_PI / 180.0f
#define RADIAN_TO_DEGREE	180.0f / M_PI

#define COS(Angle) (float)cos(Angle*DEGREE_TO_RADIAN)

#define SIN(Angle) (float)sin(Angle*DEGREE_TO_RADIAN)

/*!
	Initialize the camera position.
 */
Camera::Camera(std::string name, Scene* parent, CameraType camType) : Object(name, parent)
{
    //Reset的隐含作用是设置了up、right、forward矢量的默认值
    // INITIALIZE PARAMETERS
    Reset();
    
    //SET THE TYPE OF CAMERA
    type            = camType;
    
    if(parent){
        parent->addCamera(this);
    }
    
    MoveBackwards(20.0);
    StrafeUpside(5.0);
    //glEnable(GL_SCISSOR_TEST);
}

//Reset to Center of Screen or Bottom Mid point (based on pixel)
bool Camera::Reset()
{
    // SET THE CAMERA POSITION AND DIRECTIONAL VECTORS
    Position		= glm::vec3 ( 0.0f, 0.0f,  5.0f);
    Target          = glm::vec3 ( 0.0f, 0.0f,  0.0f);
    
    // Directonal vectors initally are set along the x/y/z main axes
    Forward			= glm::vec3 ( 0.0f, 0.0f, -1.0f);
    Right			= glm::vec3 ( 1.0f, 0.0f,  0.0f);
    Up				= glm::vec3 ( 0.0f, 1.0f,  0.0f);
    
    float span = 40.0f;
    cameraViewParameters.left   = -span;
    cameraViewParameters.right  =  span;
    cameraViewParameters.bottom = -span;
    cameraViewParameters.top    =  span;
    cameraViewParameters.front  = -span;
    cameraViewParameters.back   =  span;
    
    // PERSPECTIVE CAMERA VIEW PARAMETERS
    cameraViewParameters.fov        = 60.0f;
    cameraViewParameters.nearPlane  = 1.0f;
    cameraViewParameters.farPlane   = 500.0f;
    clearBitFieldMask               = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
    return true;
}


void Camera::CalculateDirectionAngles()
{
    glm::vec3 lookVector = Target - Position;
    float len = glm::length(lookVector);
    lookVector /= len;
    lookVector = glm::normalize(lookVector);
    //将标准矢量的每一个坐标值当作是对原坐标frame中每个轴的cosine值，这个行为没问题，先转换为针对每个axis的角度
    float cosX = acos(lookVector.x)*RADIAN_TO_DEGREE;
    float cosY = acos(lookVector.y)*RADIAN_TO_DEGREE;
    float cosZ = acos(lookVector.z)*RADIAN_TO_DEGREE;
    AnglesToAxes(glm::vec3(cosX, cosY, cosZ));
}

//计算出eye frame的各个axis vector？？？
void Camera::AnglesToAxes(glm::vec3 angles)
{
    
    float sx, sy, sz, cx, cy, cz, theta;
    
    // X-Axis Rotation => (pitch)
    theta	= angles.x * DEGREE_TO_RADIAN;
    sx		= sinf(theta);
    cx		= cosf(theta);
    
    // Y-Axis Rotation => (yaw)
    theta	= angles.y * DEGREE_TO_RADIAN;
    sy		= sinf(theta);
    cy		= cosf(theta);
    
    // Z-Axis Rotation => (roll)
    theta	= angles.z * DEGREE_TO_RADIAN;
    sz		= sinf(theta);
    cz		= cosf(theta);
    /*
     RxRyRz
     // Calculate the Right Axis
     Right.x	= cy*cz;
     Right.y	= sx*sy*cz + cx*sz;
     Right.z	= -cx*sy*cz + sx*sz;
     
     // Calculate the UP Axis
     Up.x	= -cy*sz;
     Up.y	= -sx*sy*sz + cx*cz;
     Up.z	= cx*sy*sz + sx*cz;
     
     // Calculate the FORWARD Axis
     Forward.x = sy;
     Forward.y = -sx*cy;
     Forward.z = cx*cy;*/
    
    /*
     // RzRxRy
     // Calculate the Right Axis
     Right.x	= cz*cy-sz*sx*sy;
     Right.y	= sz*cy + cz*sx*sy;
     Right.z	= -cx*sy;
     
     // Calculate the UP Axis
     Up.x	= -cz*sx;
     Up.y	= cz*cx;
     Up.z	= sx;
     
     // Calculate the FORWARD Axis
     Forward.x = cz*sy+sz*sx*cy;
     Forward.y = sz*sy-cz*sx*cy;
     Forward.z = cx*cy;*/
    
    // RzRyRx
    // Calculate the Right Axis
    Right.x	= cz*cy;
    Right.y	= sz*cy;
    Right.z	= -sy;
    
    // Calculate the UP Axis
    Up.x	= -sz*cx+cz*sy*sx;
    Up.y	= cz*cx-sz*sy*sx;
    Up.z	= cy*sx;
    
    // Calculate the FORWARD Axis
    Forward.x = sz*sx+cz*sy*cx;
    Forward.y = -cz*sx+sz*sy*cx;
    Forward.z = cy*cx;
    
}

void Camera::SetClearColor(glm::vec4 color)
{
    clearColor = color;
}

//rotate eye frame
void Camera::Rotate(glm::vec3 orientation, float angle)
{
    // Rotate along X axis.
    if(orientation.x == 1.0){
        Forward = glm::normalize( Forward * COS(angle) + Up * SIN(angle) );
        Up      = -glm::cross( Forward, Right );
    }
    
    // Rotate along Y axis.
    if(orientation.y == 1.0){
        Forward = glm::normalize( Forward * COS(angle) - Right * SIN(angle));
        Right    = glm::cross( Forward, Up );
    }
    
    // Rotate along Z axis.
    if(orientation.z == 1.0){
        Right    = glm::normalize( Right * COS(angle) + Up * SIN(angle) );
        Up      = -glm::cross(Forward, Right);
    }
}

void Camera::Viewport (int x, int y, int width, int height)
{
    viewPortParam.x         = x;
    viewPortParam.y         = y;
    viewPortParam.width     = width;
    viewPortParam.height    = height;
}


/*!
	Render the current camera metrics.
	Note: Calling glLoadIdentity before using Render is necessary for correct result to be displayed
 */
void Camera::Render()
{
    Scene* scene                = dynamic_cast<Scene*>(this->GetParent());
    Transform*	TransformObj    = scene->SceneTransform();
    GLfloat aspectRatio         = (GLfloat)viewPortParam.width / (GLfloat)viewPortParam.height;
    
    
    ///////////////// SETUP VIEWPORT INFO /////////////////
    glViewport( viewPortParam.x, viewPortParam.y, viewPortParam.width, viewPortParam.height );
    glScissor ( viewPortParam.x, viewPortParam.y, viewPortParam.width, viewPortParam.height );
    Clear();
    ///////////////// SETUP PROJECTION MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( PROJECTION_MATRIX );
    TransformObj->TransformLoadIdentity();
    
    if ( type == perespective ){
        TransformObj->TransformSetPerspective(cameraViewParameters.fov, aspectRatio, cameraViewParameters.nearPlane, cameraViewParameters.farPlane, 0);
    }else{
        //对orthoogonal matrix的细分用法
        if ( viewPortParam.width <= viewPortParam.height ){
            TransformObj->TransformOrtho( cameraViewParameters.left, cameraViewParameters.right, cameraViewParameters.bottom / aspectRatio, cameraViewParameters.top / aspectRatio, cameraViewParameters.front, cameraViewParameters.back);
        }
        else{
            TransformObj->TransformOrtho( cameraViewParameters.left * aspectRatio, cameraViewParameters.right * aspectRatio, cameraViewParameters.bottom, cameraViewParameters.top, cameraViewParameters.front, cameraViewParameters.back);
        }
    }
    
    ///////////////// SETUP VIEW MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( VIEW_MATRIX );
    TransformObj->TransformLoadIdentity();
    //只要设置target参数，则lookat功能就能实现
    glm::vec3 viewPoint = Position + Forward;
    TransformObj->TransformLookAt(&Position, &viewPoint, &Up);
    
    ///////////////// SETUP MODEL MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( MODEL_MATRIX );
    TransformObj->TransformLoadIdentity();
    
    return;
}

/*!
	Move in the backward direction.
 */
void Camera::MoveBackwards	 ( GLfloat Distance )
{
    MoveForwards( -Distance );
}

/*!
	Move in the forward direction.
 */
//移动forward vector只是对其的缩放操作（scalar multiplication）
void Camera::MoveForwards( GLfloat Distance )
{
    glm::vec3 MoveVector;
    MoveVector.x = Forward.x * Distance;
    MoveVector.y = Forward.y * Distance;
    MoveVector.z = Forward.z * Distance;
    Position += MoveVector;
}


/*!
	Move in the Right side direction.
 */
void Camera::StrafeRightSide ( GLfloat Distance )
{
    StrafeLeftSide( -Distance );
}

/*!
	Move in the left side direction.
 */
void Camera::StrafeLeftSide ( GLfloat Distance )
{
    Position += Right * Distance;
}

/*!
	Move in the downside direction.
 */
void Camera::StrafeDownside( GLfloat Distance )
{
    StrafeUpside ( -Distance );
}

/*!
	Move in the Upside direction.
 */
void Camera::StrafeUpside ( GLfloat Distance )
{
    Position += Up * Distance;
}

void Camera::Resize()
{
    glGetIntegerv( GL_VIEWPORT, viewport_matrix );
}

glm::mat4 Camera::GetViewMatrix() const
{
    glm::vec3 ViewPoint = Position + Forward;
    return glm::lookAt( Position, ViewPoint, Up );
}

glm::mat4 Camera::GetProjectionMatrix() const
{
    if( type == perespective ){
        GLfloat aspectRatio         = (GLfloat)viewPortParam.width / (GLfloat)viewPortParam.height;
        return glm::perspective(cameraViewParameters.fov, aspectRatio, cameraViewParameters.nearPlane, cameraViewParameters.farPlane);
    }
    else{
        return glm::ortho( (float)viewport_matrix[0], (float)viewport_matrix[2], (float)viewport_matrix[3], (float)viewport_matrix[1] , -1.0f, 1.0f);
    }
}

CameraType Camera::GetCameraType() const{
    return type;
}

ViewPort Camera::GetViewPortParam() const{
    return viewPortParam;
}

void Camera::Clear()
{
    glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w );
    glClear(clearBitFieldMask);
}


// Derive versions of camera
CameraHUD::CameraHUD(std::string name, Scene* parent) : Camera(name, parent, ortho)
{
    
}

void CameraHUD::Render()
{
    //    return;
    Scene* scene = dynamic_cast<Scene*>(this->GetParent());
    //glViewport( 0, 0, screenWidth, screenHeight );
    //glViewport( viewPortParam.x, viewPortParam.y, viewPortParam.width, viewPortParam.height );
    
    Transform*	TransformObj			= scene->SceneTransform();
    
    // This HUD therefore do not clear the color buffer
    ///////////////// SETUP VIEWPORT INFO /////////////////
    glViewport( viewPortParam.x, viewPortParam.y, viewPortParam.width, viewPortParam.height );
    glScissor ( viewPortParam.x, viewPortParam.y, viewPortParam.width, viewPortParam.height );
    //glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w );
    //glClear(clearBitFieldMask);

    ///////////////// SETUP PROJECTION MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( PROJECTION_MATRIX );
    TransformObj->TransformLoadIdentity();
    glGetIntegerv( GL_VIEWPORT, viewport_matrix );
    
    //HUD camera is set defaultly as doing orthographic projection
    // Left ,Right ,Bottom , Top, Near, Far
    TransformObj->TransformOrtho( viewport_matrix[0], viewport_matrix[2], viewport_matrix[3], viewport_matrix[1] , -1, 1);
    
    ///////////////// SETUP VIEW MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( VIEW_MATRIX );
    TransformObj->TransformLoadIdentity();
    
    ///////////////// SETUP MODEL MATRIX /////////////////
    TransformObj->TransformSetMatrixMode( MODEL_MATRIX );
    TransformObj->TransformLoadIdentity();
    return;
}

CameraHUD::~CameraHUD()
{
    
}
