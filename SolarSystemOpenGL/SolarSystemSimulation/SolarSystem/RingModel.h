#ifndef RINGMODEL_H
#define RINGMODEL_H

#include "PlanetModel.h"
#include "GLutils.h"
#include "RendererEx.h"
#include "WaveFrontOBJ.h"
#include "Light.h"

#include "RingGeometry.h"


//一个SPACEMODEL只是从一个mesh中加载一种几何物体模型
class RingModel : public Model
{
    
public:
    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
    void AdjustTimeScale( bool x, float y);

public:
    // Constructor for PLANETMODEL
    RingModel( Scene* parent, Model* model, ModelType type);

    // Destructor for PLANETMODEL
    virtual ~RingModel();

    // Initialize our Model class
    void InitModel();

    // Render the Model class
    void Render();

    // Apply scenes light on the object
    void ApplyLight();

    // Apply material on the object
    void ApplyMaterial();

    bool IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint);

    // Set the light pass
    void SetLightPass(bool flag);
    
    void SetSurfaceTextureId(GLuint surfaceTextureId);
    
    void SetRingScale(float scale=1.0){ringScale = scale;}
    
private:
    // Load the mesh model
    void LoadMesh();
    
    // Switch the mesh model
    void SwitchMesh();
    
    // Release the mesh model resources
    void ReleaseMeshResources();
    
    
    // Parse the wavefront OBJ mesh
    unsigned char RenderPrimitive;
    
    // Number of vertex Indices
    size_t IndexCount;
    
    // Vertex Array Object ID
    GLuint OBJ_VAO_Id;

    // VBO Stride
    int stride;
    
    // Vertex offset
    GLvoid* offset;

    // Vertex Texture offset in interleaved array.
    GLvoid* offsetTexCoord;
    char MVP, MV, M, SHADOW_MAT, SHADOW_TEX;
    char IS_LIGHT_PASS;
    GLint NormalMatrix;
    
    GLuint vertexBuffer;
    GLuint indexBuffer;
    
    
    bool isLightPass;
    
    GLuint surfaceTextureId;
    GLfloat ringScale;
    
    // Char variables is used to store uniform location values
    // Material variables
    char MaterialAmbient, MaterialSpecular, MaterialDiffuse, MaterialShininess;
    
    // Light variables
    char LightAmbient,LightSpecular, LightDiffuse, LightPosition;
    glm::mat4 lightBiasMat;
};


#endif // RINGMODEL_H
