#include "RingModel.h"

#include "Cache.h"
#include "constant.h"
#include "SpaceScene.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>

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

RingModel::RingModel( Scene* parent, Model* model, ModelType type ):Model(parent, model, type)
{
	if (!parent)
		return;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    //初始化根对象的transformation值
    transformation[0][0] = transformation[1][1] = transformation[2][2] = transformation[3][3] = 1.0;
    
    
    LoadMesh();
}

void RingModel::ReleaseMeshResources()
{
    glDeleteVertexArrays(1, &OBJ_VAO_Id);
    glDeleteBuffers(1, &vertexBuffer);
}

void RingModel::LoadMesh()
{
    RingGeometry *ringGeometry=new RingGeometry();
    RingGeometryData ringData = ringGeometry->createRingGeometry();
    
    IndexCount      = ringData.vertices.size();
    
    stride          = sizeof(vec4) + sizeof(vec3)+ sizeof(vec2);
    offset          = ( GLvoid*) ( sizeof(vec4));
    offsetTexCoord  = ( GLvoid*) ( sizeof(vec4) + sizeof(vec3) );
    
    size_t bufferSize = ringData.vertices.size()*(sizeof(vec4)+
            sizeof(vec3)+sizeof(vec2));
    size_t indexBufferSize = ringData.indices.size()*sizeof(GLuint);
    
    GLUtils::checkGlError("before buffer");
    // Create the VBO for our obj model vertices.
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, bufferSize, &(ringData.vertices[0]), GL_STATIC_DRAW);
    // Create the VBO for indices
    glGenBuffers(1, &indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBufferSize, &(ringData.indices[0]), GL_STATIC_DRAW);
    GLUtils::checkGlError("after buffer");
    
//        //通过mapperBuffer的方式验证VertexBuffer中的数据并没有问题
//
//         GLfloat *vtxMappedBuf;
//
//         GLfloat vtxBuf[100] ;
//
//         vtxMappedBuf = (GLfloat*) glMapBufferRange ( GL_ARRAY_BUFFER, 0, (unsigned int)bufferSize , GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
//         if ( vtxMappedBuf == NULL ) {
//         printf( "Error mapping vertex buffer object." );
//         }
//
//         memcpy (  vtxBuf,vtxMappedBuf, bufferSize );
    
    // Create the VAO, this will store the vertex attributes into vectore state.
    glGenVertexArrays(1, &OBJ_VAO_Id);
    glBindVertexArray(OBJ_VAO_Id);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(VERTEX_POSITION);
    glEnableVertexAttribArray(TEX_COORD);
    glEnableVertexAttribArray(NORMAL_POSITION);
    glVertexAttribPointer(VERTEX_POSITION, 4, GL_FLOAT, GL_FALSE, stride, 0);
    // the offset for normal
    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, offset);
    // the offset for uv
    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord);
    
    // bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
    
    glBindVertexArray(0);
    
    
    
}
/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
RingModel::~RingModel()
{
	PROGRAM* program = NULL;
    if ( (program = (ProgramManagerObj->Program( ( char * )"SpaceShader" ))) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void RingModel::InitModel()
{
    //启动子节点的initModel
    Model::InitModel();
    
    program = ProgramManagerObj->ProgramLoad( (char *)"SpaceShader", VERTEX_SHADER_PRG, FRAGMENT_SHADER_PRG);

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
void RingModel::Render()
{
    glDisable(GL_CULL_FACE);
    
    glUseProgram(program->ProgramID);
    ApplyMaterial();
    ApplyLight();
//    Camera* lP = ((CustomScene*)this->SceneHandler)->GetLightPerspective();
//    glm::mat4 shadowCoord = lightBiasMat * lP->GetProjectionMatrix() * lP->GetViewMatrix();
//    glUniformMatrix4fv( SHADOW_MAT, 1, GL_FALSE,( float * )&shadowCoord );
//
//    if ( IS_LIGHT_PASS >= 0 ){
//        isLightPass ? glUniform1i(IS_LIGHT_PASS, 1) : glUniform1i(IS_LIGHT_PASS, 0);
//    }

    // Bind the texture unit 0 to surface texture
    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, surfaceTextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //在S坐标上镜像重复，T坐标上简单重复。实际效果x方向镜像，y方向简单重复
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

    //ScaleLocalUniformly(ringScale);
    
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

        // Draw Element
        glDrawElements(GL_TRIANGLES, (int)IndexCount, GL_UNSIGNED_INT,0);
        //glDrawArrays(GL_TRIANGLES, 0, IndexCount);
        
        glBindVertexArray(0);
        TransformObj->TransformPopMatrix(); // Local Level
    }

    Model::Render();
    TransformObj->TransformPopMatrix();  // Parent Child Level
}

// Apply scenes light on the object
void RingModel::ApplyLight()
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
void RingModel::ApplyMaterial()
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
}


bool RingModel::IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint)
{
    return false;
}

void RingModel::SetLightPass(bool flag)
{
    isLightPass = flag;

    for(int i =0; i<GetChildren()->size(); i++){
        dynamic_cast<RingModel*>(GetChildren()->at(i))->SetLightPass( flag );
    }
}

void RingModel::SetSurfaceTextureId(GLuint surfaceTextureId){
    this->surfaceTextureId = surfaceTextureId;
}

void RingModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void RingModel::TouchEventMove( float x, float y )
{
}

void RingModel::TouchEventRelease( float x, float y )
{
}

void RingModel::AdjustTimeScale( bool x, float y){
    
}

