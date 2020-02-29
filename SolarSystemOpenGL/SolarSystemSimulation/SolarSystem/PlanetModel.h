#ifndef PLANETMODEL_H
#define PLANETMODEL_H

#include "ModelEx.h"
#include "GLutils.h"
#include "RendererEx.h"
#include <string>
#include "WaveFrontOBJ.h"
#include "Light.h"

using namespace std;

#define EARTH_SCALE (1/510)*0.00002

//enum MeshType{
//    SEMI_HOLLOW_CYLINDER = 0,
//    SPHERE,
//    TORUS,
//    SUZZANE,
//    ISO_SPHERE,
//    CONE,
//    CUBE
//};

//一个PLANETMODEL只是从一个mesh中加载一种几何物体模型
class PlanetModel : public Model
{
    
public:
	void TouchEventDown( float x, float y );
	void TouchEventMove( float x, float y );
	void TouchEventRelease( float x, float y );
    void AdjustTimeScale( bool x, float y);

public:
    // Constructor for PLANETMODEL
    PlanetModel( Scene* parent, Model* model, ModelType type);

    // Destructor for PLANETMODEL
    virtual ~PlanetModel();
    
    virtual bool isPickingEnabled(){
        return true;
    }
    
    virtual vector<IntersectionData> rayCast(RayCaster *rayCaster);

    // Initialize our Model class
    void InitModel();

    // Render the Model class
    void Render();

    // Apply scenes light on the object
    void ApplyLight();

    // Apply material on the object
    void ApplyMaterial();

    // set raidus
    void SetPlanetRadius(float radius=1.0){planetRadius = radius;}
    
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
    char MVP, MV, M, SHADOW_MAT, SHADOW_TEX;
//    char IS_LIGHT_PASS;
    GLint NormalMatrix;
    GLuint vertexBuffer;
//    bool isLightPass;
    
    GLuint surfaceTextureId;
    
    float planetRadius;
    
    // Char variables is used to store uniform location values
    // Material variables
    char MaterialAmbient, MaterialSpecular, MaterialDiffuse, MaterialShininess, MaterialPickingAlpha;
    
    // Light variables
    char LightAmbient,LightSpecular, LightDiffuse, LightPosition;
    glm::mat4 lightBiasMat;
};


#endif // PLANETMODEL_H
