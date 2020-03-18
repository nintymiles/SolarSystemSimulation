#ifndef SUNMODEL_H
#define SUNMODEL_H

#include "ModelEx.h"
#include "GLutils.h"
#include "RendererEx.h"
#include <string>
#include "WaveFrontOBJ.h"
#include "Light.h"
#include "StbImage.h"
#include "Geometry.h"

using namespace std;
using namespace namespaceimage;

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
class SunModel : public Model
{
    
public:
	void TouchEventDown( float x, float y );
	void TouchEventMove( float x, float y );
	void TouchEventRelease( float x, float y );
    void AdjustTimeScale( bool x, float y);

public:
    // Constructor for PLANETMODEL
    SunModel( Scene* parent, Model* model, ModelType type, shared_ptr<Geometry> geometryPtr);

    // Destructor for PLANETMODEL
    virtual ~SunModel();
    
    virtual bool isPickingEnabled(){
        return true;
    }
    
    virtual vector<IntersectionData> rayCast(RayCaster *rayCaster);

    // Initialize our Model class
    void InitModel();

    // Render the Model class
    void Render();


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
    
    char MV, M, P, TEX1, TEX2, Time, FogColor, FogDensity,UVScale;
    GLint NormalMatrix;
    GLuint vertexBuffer;
    GLuint vertexCount;
    
    GLuint surfaceTextureId;
    
    shared_ptr<Geometry> geometry;
    
    float planetRadius;
    
    StbImage *cloudImage;
    StbImage *lavaImage;
    
    static float timer;
    

};


#endif // SUNMODEL_H
