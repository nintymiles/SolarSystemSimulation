#include "PlanetModel.h"

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
#define VERTEX_SHADER_PRG			( char * )"PhongVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"PhongFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/PhongVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/PhongFragment.glsl"
#endif

#define VERTEX_POSITION 0
#define NORMAL_POSITION 1
#define TEX_COORD 2

#define GetUniform(X,Y) ProgramManagerObj->ProgramGetUniformLocation(X,Y);

// Namespace used
using std::ifstream;
using std::ostringstream;

/*!
	Simple Default Constructor

	\param[in] None.
	\return None

*/
//构造方法的过程：首先开启深度检测，然后设置基本变量，将物体模型的全局变换矩阵设置为同一矩阵，然后加载mesh。
PlanetModel::PlanetModel(Scene* parent, Model* model, ModelType type,shared_ptr<Geometry> geometryptr):Model(parent, model, type)
{
	if (!parent)
		return;

    glEnable	( GL_DEPTH_TEST );
    
    glEnable(GL_BLEND);
    // Enable blending
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    
    transformation[0][0] = transformation[1][1] = transformation[2][2] = transformation[3][3] = 1.0;
    
    //default sphere radius
    planetRadius = 1.0f;
    geometry = geometryptr;
        
    LoadMesh();
}

void PlanetModel::ReleaseMeshResources()
{
    glDeleteVertexArrays(1, &OBJ_VAO_Id);
    glDeleteBuffers(1, &vertexBuffer);
}

void PlanetModel::LoadMesh()
{
    stride          = (2 * sizeof(vec3) )+ sizeof(vec2)+ sizeof(vec4);
    offset          = ( GLvoid*) ( sizeof(glm::vec3) + sizeof(vec2) );
    offsetTexCoord  = ( GLvoid*) ( sizeof(glm::vec3) );

    // Create the VAO, this will store the vertex attributes into vectore state.
    glGenVertexArrays(1, &OBJ_VAO_Id);
    glBindVertexArray(OBJ_VAO_Id);
    glBindBuffer(GL_ARRAY_BUFFER, geometry->vbo);
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glEnableVertexAttribArray(NORMAL_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord);
    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glBindVertexArray(0);
    

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    //在S坐标上镜像重复，T坐标上简单重复。实际效果x方向镜像，y方向简单重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    
}
/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
PlanetModel::~PlanetModel()
{
	PROGRAM* program = NULL;
    if ( (program = (ProgramManagerObj->Program( (char *)"PlanetShader" ))) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void PlanetModel::InitModel()
{
    Model::InitModel();
    program = ProgramManagerObj->ProgramLoad((char *)"PlanetShader", VERTEX_SHADER_PRG, FRAGMENT_SHADER_PRG);
    
    this->setShader(program);

    // Use Phong Shade Program
    glUseProgram( program->ProgramID );
    
    // Get Material property uniform variables
    MaterialAmbient  = GetUniform(program,(char*)"MaterialAmbient");
    MaterialSpecular = GetUniform(program,(char*)"MaterialSpecular");
    MaterialDiffuse  = GetUniform(program,(char*)"MaterialDiffuse");
    MaterialShininess= GetUniform(program,(char*)"ShininessFactor");
    MaterialPickingAlpha = GetUniform(program,(char*)"PickingAlpha");
    
    // Get Light property uniform variables
    LightAmbient    = GetUniform(program,(char*)"LightAmbient");
    LightSpecular   = GetUniform(program,(char*)"LightSpecular");
    LightDiffuse    = GetUniform(program,(char*)"LightDiffuse");
    LightPosition   = GetUniform(program, (char*)"LightPosition");
    
    MVP             = GetUniform( program, ( char* )"ModelViewProjectionMatrix" );
    MV              = GetUniform( program, ( char* )"ModelViewMatrix" );
    M               = GetUniform( program, ( char* )"ModelMatrix" );
    NormalMatrix    = GetUniform( program, ( char* )"NormalMatrix");
    TEX      = GetUniform( program, ( char* )"planetTexture" );
    
    glUniform1i(TEX, 0);

    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void PlanetModel::Render()
{
    glUseProgram(program->ProgramID);
    ApplyMaterial();
    ApplyLight();
    
    // Bind the texture unit 0 to surface texture
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, surfaceTextureId);
    
    ScaleLocalUniformly(planetRadius);
    
    
    TransformObj->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();

    if(isVisible){
        TransformObj->TransformPushMatrix(); // Local Level
        ApplyModelsLocalTransformation();
        
        
        // Apply Transformation.
        glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
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

// Apply scenes light on the object
void PlanetModel::ApplyLight()
{
    for(int i =0; i<SceneHandler->getLights().size(); i++){
        Light*& light = SceneHandler->getLights().at(i);
        if ( LightAmbient >= 0 ){
            glUniform3f( LightAmbient, light->material.ambient.r, light->material.ambient.g, light->material.ambient.b );
        }
        
        if ( LightSpecular >=  0 ){
            glUniform3f( LightSpecular, light->material.specular.r, light->material.specular.g, light->material.specular.b );
        }
        
        if ( LightDiffuse >= 0 ){
            glUniform3f(LightDiffuse, light->material.diffuse.r, light->material.diffuse.g, light->material.diffuse.b);
        }
        
        if ( LightPosition >= 0 ){
            glm::vec3 lightPosition(light->position.x,light->position.y,light->position.z);
            glUniform3fv(LightPosition, 1, (float*)&lightPosition);
        }

    }
}

// Apply material on the object
void PlanetModel::ApplyMaterial()
{
    if ( MaterialAmbient >= 0 ){
        glUniform3f(MaterialAmbient, materialObj.ambient.r, materialObj.ambient.g, materialObj.ambient.b);
    }
    
    if ( MaterialSpecular >= 0){
        glUniform3f( MaterialSpecular, materialObj.specular.r, materialObj.specular.g, materialObj.specular.b );
    }
    
    if ( MaterialDiffuse >= 0 ){
        glUniform3f( MaterialDiffuse, materialObj.diffuse.r, materialObj.diffuse.g, materialObj.diffuse.b );
    }
    
    if ( MaterialShininess >= 0 ){
        glUniform1f(MaterialShininess, materialObj.shiness);
    }
    
    if ( MaterialPickingAlpha >= 0 ){
        glUniform1f(MaterialPickingAlpha, materialObj.pickingAlpha);
    }
}

//执行射线投射动作，使用raster发出射线，然后计算相交。
//这个方法是raycast的核心，每个对象都会调用
vector<IntersectionData> PlanetModel::rayCast(RayCaster *rayCaster){
    
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


void PlanetModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void PlanetModel::TouchEventMove( float x, float y )
{
}

void PlanetModel::TouchEventRelease( float x, float y )
{
}

void PlanetModel::AdjustTimeScale( bool x, float y){
    
}

void PlanetModel::SetSurfaceTextureId(GLuint surfaceTextureId){
    this->surfaceTextureId = surfaceTextureId;
}

