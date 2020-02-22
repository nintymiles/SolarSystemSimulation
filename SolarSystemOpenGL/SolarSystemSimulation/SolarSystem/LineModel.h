#ifndef LINEMODEL_H
#define LINEMODEL_H

#include "PlanetModel.h"
#include "GLutils.h"
#include "RendererEx.h"
#include "WaveFrontOBJ.h"
#include "Light.h"

#include <string>

using namespace std;


//一个LINEMODEL只是从一个mesh中加载一种几何物体模型
class LineModel : public Model
{
    
public:
    void TouchEventDown( float x, float y );
    void TouchEventMove( float x, float y );
    void TouchEventRelease( float x, float y );
    void AdjustTimeScale( bool x, float y);

public:
    // Constructor for PLANETMODEL
    LineModel( Scene* parent, Model* model, ModelType type);

    // Destructor for PLANETMODEL
    virtual ~LineModel();

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
    
private:
    // Load the mesh model
    void LoadMesh();
    
    // Switch the mesh model
    void SwitchMesh();
    
    // Release the mesh model resources
    void ReleaseMeshResources();
    
    // Object for wavefront model parser
    OBJMesh waveFrontObjectModel;

    // Mesh structure object for storing parsed information.
    Mesh* objMeshModel;
    
    // Parse the wavefront OBJ mesh
    unsigned char RenderPrimitive;
    
    // Number of vertex Indices
    int IndexCount;
    
    // Vertex Array Object ID
    GLuint OBJ_VAO_Id;

    // VBO Stride
    int stride;
    
    // Vertex offset
    GLvoid* offset;

    // Vertex Texture offset in interleaved array.
    GLvoid* offsetTexCoord;
    char MVP, UCOLOR;
    
    GLint NormalMatrix;
    GLuint vertexBuffer;
    bool isLightPass;
    
    GLuint surfaceTextureId;
    
    // Char variables is used to store uniform location values
    // Material variables
    char MaterialAmbient, MaterialSpecular, MaterialDiffuse, MaterialShininess;
    
    // Light variables
    char LightAmbient,LightSpecular, LightDiffuse, LightPosition;
    glm::mat4 lightBiasMat;
    
    glm::vec3 lineColor;
    GLint lineWidth;
};


#endif // LINEMODEL_H
