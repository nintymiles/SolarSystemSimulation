#include "SunModel.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

#include "Cache.h"
#include "constant.h"

#include "SpaceScene.h"
#include "RayCaster.h"

//#import <fstream>
using namespace glm;

#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"LavaShaderVert.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"LavaShaderFrag.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/LavaShaderVert.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/LavaShaderFrag.glsl"
#endif

#define VERTEX_POSITION 0
#define TEX_COORD 1

#define GetUniform(X,Y) ProgramManagerObj->ProgramGetUniformLocation(X,Y);

// Namespace used
using std::ifstream;
using std::ostringstream;

float SunModel::timer=0.f;

/*!
	Simple Default Constructor

	\param[in] None.
	\return None

*/
//构造方法的过程：首先开启深度检测，然后设置基本变量，将物体模型的全局变换矩阵设置为同一矩阵，然后加载mesh。
SunModel::SunModel( Scene* parent, Model* model, ModelType type, shared_ptr<Geometry> geometryPtr ):Model(parent, model, type)
{
	if (!parent)
		return;

    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    // Enable blending
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    transformation[0][0] = transformation[1][1] = transformation[2][2] = transformation[3][3] = 1.0;
    
    //default sphere radius
    planetRadius = 1.0f;
    
    geometry = geometryPtr;
        
    LoadMesh();
}

void SunModel::ReleaseMeshResources()
{
    glDeleteVertexArrays(1, &OBJ_VAO_Id);
    glDeleteBuffers(1, &vertexBuffer);
}

void SunModel::LoadMesh()
{
    char fname[MAX_PATH]= {""};
#ifdef __IPHONE_4_0
    GLUtils::extractPath(getenv( "FILESYSTEM" ), fname);
#else
    strcpy( fname, "/sdcard/Models/" );
#endif
    
    char modelName[]={"Sphere.obj"};
    string modName = string(fname) + string(modelName);
    
    
    
    stride          = (2 * sizeof(vec3) )+ sizeof(vec2)+ sizeof(vec4);
    offset          = ( GLvoid*) ( sizeof(glm::vec3) + sizeof(vec2) );
    offsetTexCoord  = ( GLvoid*) ( sizeof(glm::vec3) );
    
    // Create the VAO, this will store the vertex attributes into vectore state.
    glGenVertexArrays(1, &OBJ_VAO_Id);
    glBindVertexArray(OBJ_VAO_Id);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    //    glEnableVertexAttribArray(NORMAL_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord);
    //    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glBindVertexArray(0);
    
    
    string cloudTexName = string(fname) + "cloud.png";
    string lavaTileTexName = string(fname) + "lavatile.jpg";
    
    cloudImage=new StbImage();
    lavaImage=new StbImage();
    cloudImage->loadImage((char *)cloudTexName.c_str());
    lavaImage->loadImage((char *)lavaTileTexName.c_str());
}
/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
SunModel::~SunModel()
{
	PROGRAM* program = NULL;
    if ( (program = (ProgramManagerObj->Program( (char *)"LavaShader" ))) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SunModel::InitModel()
{
    Model::InitModel();
    program = ProgramManagerObj->ProgramLoad((char *)"LavaShader", VERTEX_SHADER_PRG, FRAGMENT_SHADER_PRG);
    
    this->setShader(program);

    // Use Phong Shade Program
    glUseProgram( program->ProgramID );
    
//    // Get Material property uniform variables
//    MaterialAmbient  = GetUniform(program,(char*)"MaterialAmbient");
//    MaterialSpecular = GetUniform(program,(char*)"MaterialSpecular");
//    MaterialDiffuse  = GetUniform(program,(char*)"MaterialDiffuse");
//    MaterialShininess= GetUniform(program,(char*)"ShininessFactor");
//    MaterialPickingAlpha = GetUniform(program,(char*)"PickingAlpha");
//    
//    // Get Light property uniform variables
//    LightAmbient    = GetUniform(program,(char*)"LightAmbient");
//    LightSpecular   = GetUniform(program,(char*)"LightSpecular");
//    LightDiffuse    = GetUniform(program,(char*)"LightDiffuse");
//    LightPosition   = GetUniform(program, (char*)"LightPosition");
    
//    MVP             = GetUniform( program, ( char* )"ModelViewProjectionMatrix" );
    M               = GetUniform( program, ( char* )"ModelMatrix" );
    
    Time            = GetUniform( program, ( char* )"time");
    FogColor        = GetUniform( program, ( char* )"fogColor");
    FogDensity      = GetUniform( program, ( char* )"fogDensity");
    MV              = GetUniform( program, ( char* )"ModelViewMatrix" );
    P               = GetUniform( program, ( char* )"ProjectionMatrix" );
    NormalMatrix    = GetUniform( program, ( char* )"NormalMatrix");
    
    UVScale   = GetUniform( program, ( char* )"uvScale");
    TEX1      = GetUniform( program, ( char* )"texture1" );
    TEX2      = GetUniform( program, ( char* )"texture2" );
    
    glUniform1i(TEX1, 2);
    glUniform1i(TEX2, 3);
    
    glUniform1f(Time, 1.0f);
    glUniform3f(FogColor,0,0,0);
    glUniform1f(FogDensity, 0.45);
    
    glUniform2f(UVScale, 6.0, 2.0);
    
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

//    texture1: { value: textureLoader.load( 'textures/lava/cloud.png' ) },
//    texture2: { value: textureLoader.load( 'textures/lava/lavatile.jpg' ) }

    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SunModel::Render()
{
    glUseProgram(program->ProgramID);
//    ApplyMaterial();
//    ApplyLight();
    
    timer+=0.25f;
    if(timer>100.f)
        timer=1.0f;
    
    glUniform1f(Time, timer);
    
    // Bind the texture unit 0 to surface texture
    glActiveTexture (GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, cloudImage->getTextureID());

    glActiveTexture (GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, lavaImage->getTextureID());
    
    ScaleLocalUniformly(planetRadius);
    
    TransformObj->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();

    if(isVisible){
        TransformObj->TransformPushMatrix(); // Local Level
        ApplyModelsLocalTransformation();
        
        
        // Apply Transformation.
        glUniformMatrix4fv( P, 1, GL_FALSE,( float * )TransformObj->TransformGetProjectionMatrix());
        glUniformMatrix4fv( MV, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewMatrix() );
        glUniformMatrix4fv( M, 1, GL_FALSE,( float * )TransformObj->TransformGetModelMatrix() );
        glm::mat4 matrix    = *(TransformObj->TransformGetModelViewMatrix());
        //对RBT应用的normal矩阵省略计算，直接应用linear transformation部分
        glm::mat3 normalMat = glm::mat3( glm::vec3(matrix[0]), glm::vec3(matrix[1]), glm::vec3(matrix[2]) );
        glUniformMatrix3fv( NormalMatrix, 1, GL_FALSE, (float*)&normalMat );
        
        // Bind with Vertex Array Object for OBJ
        glBindVertexArray(OBJ_VAO_Id);
        
        // Draw Geometry
        glDrawArrays(GL_TRIANGLES, 0, geometry->vboLen);
        glBindVertexArray(0);
        TransformObj->TransformPopMatrix(); // Local Level
    }
    
    Model::Render();
    TransformObj->TransformPopMatrix();  // Parent Child Level
}

//执行射线投射动作，使用raster发出射线，然后计算相交。
//这个方法是raycast的核心，每个对象都会调用
vector<IntersectionData> SunModel::rayCast(RayCaster *rayCaster){
    
    if(!isPickingEnabled()) return{};
    
    TransformObj->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();

    TransformObj->TransformPushMatrix(); // Local Level
    ApplyModelsLocalTransformation();
    //Material material = material;
    mat4 matrixWorld = *TransformObj->TransformGetModelMatrix();
    TransformObj->TransformPopMatrix(); // Local Level
    TransformObj->TransformPopMatrix(); // Parent Level
        
    //if ( material == undefined ) return;
        
    // Checking boundingSphere distance to ray
//    if(this->boundingSphere == NULL){
    this->boundingSphere = new Sphere();
    this->boundingSphere->setFromPoints(geometry->points);
    //由于每个行星model都是从标准球体建立，从几何数据无法计算出真实的半径，所以需要从半径信息计算
    this->boundingSphere->radius *= planetRadius;
//    }
    
    //将BV副本转换为world coordinate
    Sphere *sphere = new Sphere();
    sphere->center = boundingSphere->center;
    sphere->radius = boundingSphere->radius;

    sphere->applyMatrix4(matrixWorld);
    
    // 先进行了ray-sphere相交检测
    Ray *ray = rayCaster->ray;
    vector<IntersectionData> intersectPoints = ray->intersectSphere(sphere);
    if (intersectPoints.size()==0) return {};
    
    IntersectionData idata=intersectPoints[0];
    idata.object = this;

//    //先计算object matrix的反转，用于将射线转换为object coordinate
//    mat4 inverseMatrix = glm::inverse(matrixWorld);
//    Ray *rayObj = new Ray();
//    rayObj->origin = ray->origin;
//    rayObj->direction = ray->direction;
//
//    rayObj->applyMatrix4(inverseMatrix);
//
//    // Check boundingBox before continuing
//    // 计算ray-triangle前先检查ray-box相交作保守型检测，用于优化
////    if(this->boundingBox == NULL){
//        this->boundingBox = new Box();
//        this->boundingBox->setFromPoints(points);
////    }
//    intersectPoints = ray->intersectBox(boundingBox);
//    if (intersectPoints.size()==0) return {};

           
    return {idata};
}


void SunModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void SunModel::TouchEventMove( float x, float y )
{
}

void SunModel::TouchEventRelease( float x, float y )
{
}

void SunModel::AdjustTimeScale( bool x, float y){
    
}

void SunModel::SetSurfaceTextureId(GLuint surfaceTextureId){
    this->surfaceTextureId = surfaceTextureId;
}

