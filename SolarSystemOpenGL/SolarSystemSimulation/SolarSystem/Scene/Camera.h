#ifndef CAMERA_H
#define CAMERA_H
#include "ModelEx.h"
#include "ProgramManager.h"
#include "Transform.h"
#include <vector>

struct ViewPort
{
    int x;
    int y;
    int width;
    int height;
};

struct CameraViewParams
{
    // ORTHOGRAPHIC CAMERA VIEW PARAMETERS
    float left;
    float right;
    float bottom;
    float top;
    float front;
    float back;
    
    // PERSPECTIVE CAMERA VIEW PARAMETERS
    float fov;
    float nearPlane;
    float farPlane;
};

class Camera : public Object
{
private:
    // Directional unit vectors for forward, up and left.
    glm::vec3 Forward, Up, Right;
    
    // Camera Position with Target at which it is looking.
    glm::vec3 Position, Target;
    
    // Type of camera
    CameraType type;
    
protected:
    int                 viewport_matrix[4];
    ViewPort            viewPortParam;
    CameraViewParams    cameraViewParameters;
    glm::vec4           clearColor;
    GLbitfield          clearBitFieldMask;
public:
    Camera(std::string name, Scene* parent = NULL, CameraType camType = perespective);
    void Viewport (int x, int y, int width, int height);
    
    virtual void Render ();
    
    void Rotate(glm::vec3 orientation, float angle);
    
    void Resize();
    
    void MoveForwards	 ( GLfloat Distance );
    
    void MoveBackwards	 ( GLfloat Distance );
    
    void StrafeRightSide ( GLfloat Distance );
    
    void StrafeLeftSide ( GLfloat Distance );
    
    void StrafeUpside	 ( GLfloat Distance );
    
    void StrafeDownside	 ( GLfloat Distance );
    
    bool Reset();
    
    void SetClearColor(glm::vec4 color = glm::vec4(0.0, 0.0, 0.0, 1.0));
    
    void SetClearBitFieldMask(GLbitfield mask){ clearBitFieldMask = mask; }
    
    glm::vec3 PositionCamera() { return Position + Forward; };
    void SetPosition(glm::vec3 pos) { Position = pos; }
    void SetTarget(glm::vec3 pos) { Target = pos; Forward = glm::normalize(Target-Position);}
    
    // SETTER METHODS FOR ORTHOGRAPHIC CAMERA VIEW
    void SetLeft(float LeftVal)     { cameraViewParameters.left     = LeftVal;  }
    void SetRight(float RightVal)   { cameraViewParameters.right    = RightVal; }
    void SetBottom(float BottomVal) { cameraViewParameters.bottom   = BottomVal;}
    void SetTop(float TopVal)       { cameraViewParameters.top      = TopVal;   }
    void SetFront(float FrontVal)   { cameraViewParameters.front    = FrontVal; }
    void SetBack(float BackVal)     { cameraViewParameters.back     = BackVal;  }
    
    // SETTER METHODS FOR PERSPECTIVE CAMERA VIEW
    void SetFov(float FovVal)       { cameraViewParameters.fov      = FovVal;   }
    void SetNearPlane(float NearVal){ cameraViewParameters.nearPlane= NearVal;  }
    void SetFarPlane(float FarVal)  { cameraViewParameters.farPlane = FarVal;   }
    
    // GETTER METHODS FOR ORTHOGRAPHIC CAMERA VIEW
    float GetLeft()                 { return cameraViewParameters.left;     }
    float GetRight()                { return cameraViewParameters.right;    }
    float GetBottom()               { return cameraViewParameters.bottom;   }
    float GetTop()                  { return cameraViewParameters.top;      }
    float GetFront()                { return cameraViewParameters.front;    }
    float GetBack()                 { return cameraViewParameters.back;     }
    
    // GETTER METHODS FOR PERSPECTIVE CAMERA VIEW
    float GetFov() const            { return cameraViewParameters.fov;      }
    float GetNearPlane()            { return cameraViewParameters.nearPlane;}
    float GetFarPlane()             { return cameraViewParameters.farPlane; }
    
    // CALCULATE DIRECTION ANGLES
    void CalculateDirectionAngles();
    
    void AnglesToAxes(glm::vec3 angles);
    
    glm::mat4 GetViewMatrix() const;
    glm::mat4 GetProjectionMatrix() const;
    
    void Clear();
};

class CameraHUD : public Camera{
public:
    CameraHUD(std::string name, Scene* parent = NULL);
    void Render();
    virtual ~CameraHUD();
};

#endif
