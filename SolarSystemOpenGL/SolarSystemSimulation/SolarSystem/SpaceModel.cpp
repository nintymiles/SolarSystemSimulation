#include "SpaceModel.h"

#include "Cache.h"
#include "constant.h"
#include "SpaceScene.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
//#import <fstream>

using namespace glm;

#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"PhongVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"TwoSidedPhongFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/PhongVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/TwoSidedPhongFragment.glsl"
#endif

#define VERTEX_POSITION 0
#define NORMAL_POSITION 1
#define TEX_COORD 2

#define GetUniform(X,Y) ProgramManagerObj->ProgramGetUniformLocation(X,Y);

// Namespace used
using std::ifstream;
using std::ostringstream;

SpaceModel::SpaceModel( Scene* parent, Model* model, ModelType type ):Model(parent, model, type)
{
	if (!parent)
		return;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    //初始化根对象的transformation值
    transformation[0][0] = transformation[1][1] = transformation[2][2] = transformation[3][3] = 1.0;
    
    LoadMesh();
}

void SpaceModel::ReleaseMeshResources()
{
    glDeleteVertexArrays(1, &OBJ_VAO_Id);
    glDeleteBuffers(1, &vertexBuffer);
}

void SpaceModel::LoadMesh()
{
    char modelName[]={"Sphere.obj"};
    char fname[MAX_PATH]= {""};
#ifdef __IPHONE_4_0
    GLUtils::extractPath( getenv( "FILESYSTEM" ), fname );
#else
    strcpy( fname, "/sdcard/Models/" );
#endif

    strcat( fname, modelName);
    
    objMeshModel    = waveFrontObjectModel.ParseObjModel(fname);
    IndexCount      = waveFrontObjectModel.IndexTotal();
    stride          = (2 * sizeof(vec3) )+ sizeof(vec2)+ sizeof(vec4);
    offset          = ( GLvoid*) ( sizeof(glm::vec3) + sizeof(vec2) );
    offsetTexCoord  = ( GLvoid*) ( sizeof(glm::vec3) );
    
    
    // Create the VBO for our obj model vertices.
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, objMeshModel->vertices.size() * sizeof(objMeshModel->vertices[0]), &objMeshModel->vertices[0], GL_STATIC_DRAW);
    
    
    // Create the VAO, this will store the vertex attributes into vectore state.
    glGenVertexArrays(1, &OBJ_VAO_Id);
    glBindVertexArray(OBJ_VAO_Id);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glEnableVertexAttribArray(NORMAL_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord);
    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, offset);
    glBindVertexArray(0);
    
    //objMeshModel->vertices.clear();
}
/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
SpaceModel::~SpaceModel()
{
	PROGRAM* program = NULL;
    if ( (program = (ProgramManagerObj->Program( ( char * )"PhongShader" ))) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SpaceModel::InitModel()
{
    //启动子节点的initModel
    Model::InitModel();
    
    program = ProgramManagerObj->ProgramLoad("PhongShader", VERTEX_SHADER_PRG, FRAGMENT_SHADER_PRG);

    this->setShader(program);

    // Use Phong Shade Program
    glUseProgram( program->ProgramID );

    // Get Material property uniform variables
    MaterialAmbient  = GetUniform(program,(char*)"MaterialAmbient");
    MaterialSpecular = GetUniform(program,(char*)"MaterialSpecular");
    MaterialDiffuse  = GetUniform(program,(char*)"MaterialDiffuse");
    MaterialShininess= GetUniform(program,(char*)"ShininessFactor");

    // Get Light property uniform variables
    LightAmbient    = GetUniform(program,(char*)"LightAmbient");
    LightSpecular   = GetUniform(program,(char*)"LightSpecular");
    LightDiffuse    = GetUniform(program,(char*)"LightDiffuse");
    LightPosition   = GetUniform(program, (char*)"LightPosition");

    MVP             = GetUniform( program, ( char* )"ModelViewProjectionMatrix" );
    MV              = GetUniform( program, ( char* )"ModelViewMatrix" );
    M               = GetUniform( program, ( char* )"ModelMatrix" );
    NormalMatrix    = GetUniform( program, ( char* )"NormalMatrix");
    SHADOW_TEX      = GetUniform( program, ( char* )"planetTexture" );
    IS_LIGHT_PASS   = GetUniform( program, ( char* )"isLightPerspectivePass" );


    glUniform1i(SHADOW_TEX, 0);

    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SpaceModel::Render()
{
    glDisable(GL_CULL_FACE);
    
    glUseProgram(program->ProgramID);
    ApplyMaterial();
    ApplyLight();
//    Camera* lP = ((CustomScene*)this->SceneHandler)->GetLightPerspective();
//    glm::mat4 shadowCoord = lightBiasMat * lP->GetProjectionMatrix() * lP->GetViewMatrix();
//    glUniformMatrix4fv( SHADOW_MAT, 1, GL_FALSE,( float * )&shadowCoord );

    if ( IS_LIGHT_PASS >= 0 ){
        isLightPass ? glUniform1i(IS_LIGHT_PASS, 1) : glUniform1i(IS_LIGHT_PASS, 0);
    }

    // Bind the texture unit 0 to surface texture
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, surfaceTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //在S坐标上镜像重复，T坐标上简单重复。实际效果x方向镜像，y方向简单重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);


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
        glDrawArrays(GL_TRIANGLES, 0, IndexCount );
        glBindVertexArray(0);
        TransformObj->TransformPopMatrix(); // Local Level
    }

    Model::Render();
    TransformObj->TransformPopMatrix();  // Parent Child Level
}

// Apply scenes light on the object
void SpaceModel::ApplyLight()
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
void SpaceModel::ApplyMaterial()
{
    if ( MaterialAmbient >= 0 ){
        glUniform3f(MaterialAmbient, materialObj.ambient.r, materialObj.ambient.g, materialObj.ambient.b);
    }
    
    if ( MaterialSpecular >= 0){
        glUniform3f( MaterialSpecular, materialObj.specular.r, materialObj.specular.g, materialObj.specular.b );
    }
    
    if ( MaterialDiffuse >= 0 ){
        glm::vec3 color = glm::vec3(1.0, 1.0, 1.0);
        glUniform3f( MaterialDiffuse, materialObj.diffuse.r, materialObj.diffuse.g, materialObj.diffuse.b );
    }
    
    if ( MaterialShininess >= 0 ){
        glUniform1f(MaterialShininess, materialObj.shiness);
    }
}


bool SpaceModel::IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint)
{
    return false;
}

void SpaceModel::SetLightPass(bool flag)
{
    isLightPass = flag;

    for(int i =0; i<GetChildren()->size(); i++){
        dynamic_cast<SpaceModel*>(GetChildren()->at(i))->SetLightPass( flag );
    }
}

void SpaceModel::SetSurfaceTextureId(GLuint surfaceTextureId){
    this->surfaceTextureId = surfaceTextureId;
}

void SpaceModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void SpaceModel::TouchEventMove( float x, float y )
{
}

void SpaceModel::TouchEventRelease( float x, float y )
{
}

void SpaceModel::AdjustTimeScale( bool x, float y){
    
}

