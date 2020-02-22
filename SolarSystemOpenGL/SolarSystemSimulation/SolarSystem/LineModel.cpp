#include "LineModel.h"

#include "Cache.h"
#include "constant.h"
#include "SpaceScene.h"

#include "EllipseCurve.h"
#include "Curve.h"

#include "GLutils.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
//#import <fstream>

using namespace glm;

#ifdef __IPHONE_4_0
#define VERTEX_SHADER_PRG			( char * )"SimpleVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"SimpleFragment.glsl"
#else
#define VERTEX_SHADER_PRG			( char * )"shader/SimpleVertex.glsl"
#define FRAGMENT_SHADER_PRG			( char * )"shader/SimpleFragment.glsl"
#endif

#define VERTEX_POSITION 0

#define GetUniform(X,Y) ProgramManagerObj->ProgramGetUniformLocation(X,Y);

// Namespace used
using std::ifstream;
using std::ostringstream;

LineModel::LineModel( Scene* parent, Model* model, ModelType type ):Model(parent, model, type)
{
	if (!parent)
		return;

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    
    //初始化根对象的transformation值
    transformation[0][0] = transformation[1][1] = transformation[2][2] = transformation[3][3] = 1.0;
    
    lineWidth = 1.0f;
    
//    char *rgb = GLUtils::hexStringToRGB((char *)"0xfffff")
    int num = std::stoi("0xFFFFF",0,16);
    int red = num>>16&0xFF;
    int green = num>>8&0xFF;
    int blue = num&0xFF;
    lineColor = glm::vec3(red/255.0,green/255.0,blue/255.0);
    
    LoadMesh();
}

void LineModel::ReleaseMeshResources()
{
    glDeleteVertexArrays(1, &OBJ_VAO_Id);
    glDeleteBuffers(1, &vertexBuffer);
}

void LineModel::LoadMesh()
{
    EllipseCurve *ellipse=new EllipseCurve("EllipseCurve",0.0,0.0,1.0,1.0,0.0,2*M_PI,false,0.0);

    vector<glm::vec4> points = ellipse->getPoints(100);
    stride = sizeof(vec4);
    IndexCount = (int)points.size();

    // Create the VBO for our obj model vertices.
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(vec4), &points[0], GL_STATIC_DRAW);


    // Create the VAO, this will store the vertex attributes into vectore state.
    glGenVertexArrays(1, &OBJ_VAO_Id);
    glBindVertexArray(OBJ_VAO_Id);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(VERTEX_POSITION);
//    glEnableVertexAttribArray(TEX_COORD);
//    glEnableVertexAttribArray(NORMAL_POSITION);
    //其实缓存上传反而不容易出错，而设置vertexattribpointer时可能会有计算错误
    //此前的设置是上传vec4数据，但是却只能最多设置3*GL_FLOAT的shader数据指针
    glVertexAttribPointer(VERTEX_POSITION, 2, GL_FLOAT, GL_FALSE, stride, 0);
//    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, 0);
//    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
    glBindVertexArray(0);

    GLUtils::checkGlError(__func__);
    
    //通过mapperBuffer的方式验证VertexBuffer中的数据并没有问题
     
     GLfloat *vtxMappedBuf;
     
     GLfloat vtxBuf[404] ;
     
     GLint bufferSize = (int)points.size() * sizeof(vec4);
     vtxMappedBuf = (GLfloat*) glMapBufferRange ( GL_ARRAY_BUFFER, 0, (unsigned int)bufferSize , GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT );
     if ( vtxMappedBuf == NULL ) {
     printf( "Error mapping vertex buffer object." );
     }
     
     memcpy (  vtxBuf,vtxMappedBuf, bufferSize );
    
    
    points.clear();
    
//    char modelName[]={"Sphere.obj"};
//    char fname[MAX_PATH]= {""};
//#ifdef __IPHONE_4_0
//    GLUtils::extractPath( getenv( "FILESYSTEM" ), fname );
//#else
//    strcpy( fname, "/sdcard/Models/" );
//#endif
//
//    strcat( fname, modelName);
//
//    objMeshModel    = waveFrontObjectModel.ParseObjModel(fname);
//    IndexCount      = waveFrontObjectModel.IndexTotal();
//    stride          = (2 * sizeof(vec3) )+ sizeof(vec2)+ sizeof(vec4);
//    offset          = ( GLvoid*) ( sizeof(glm::vec3) + sizeof(vec2) );
//    offsetTexCoord  = ( GLvoid*) ( sizeof(glm::vec3) );
//
//
//    // Create the VBO for our obj model vertices.
//    glGenBuffers(1, &vertexBuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//    glBufferData(GL_ARRAY_BUFFER, objMeshModel->vertices.size() * sizeof(objMeshModel->vertices[0]), &objMeshModel->vertices[0], GL_STATIC_DRAW);
//
//
//    // Create the VAO, this will store the vertex attributes into vectore state.
//    glGenVertexArrays(1, &OBJ_VAO_Id);
//    glBindVertexArray(OBJ_VAO_Id);
//    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//    glEnableVertexAttribArray(VERTEX_POSITION);
////    glEnableVertexAttribArray(TEX_COORD);
////    glEnableVertexAttribArray(NORMAL_POSITION);
//    glVertexAttribPointer(VERTEX_POSITION, 3, GL_FLOAT, GL_FALSE, stride, 0);
////    glVertexAttribPointer(TEX_COORD, 2, GL_FLOAT, GL_FALSE, stride, offsetTexCoord);
////    glVertexAttribPointer(NORMAL_POSITION, 3, GL_FLOAT, GL_FALSE, stride, offset);
//    glBindVertexArray(0);
}
/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
LineModel::~LineModel()
{
	PROGRAM* program = NULL;
    if ( (program = (ProgramManagerObj->Program( (char *)"SimpleShader" ))) )
	{
		ProgramManagerObj->RemoveProgram(program);
	}
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void LineModel::InitModel()
{
    //启动子节点的initModel
    Model::InitModel();
    
    program = ProgramManagerObj->ProgramLoad((char *)"SimpleShader", VERTEX_SHADER_PRG, FRAGMENT_SHADER_PRG);

    this->setShader(program);

    // Use Phong Shade Program
    glUseProgram( program->ProgramID );
    
    MVP    = GetUniform( program, ( char* )"uMVPMatrix" );
    UCOLOR = GetUniform( program, ( char* )"uColor" );



    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void LineModel::Render()
{
    glUseProgram(program->ProgramID);
    
    glLineWidth(lineWidth);


    TransformObj->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();

    if(isVisible){
        TransformObj->TransformPushMatrix(); // Local Level
        ApplyModelsLocalTransformation();

        // Apply Transformation.
        glUniformMatrix4fv( MVP, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewProjectionMatrix() );
        glUniform3f(UCOLOR,lineColor.x,lineColor.y,lineColor.z);
        
//        glUniformMatrix4fv( MV, 1, GL_FALSE,( float * )TransformObj->TransformGetModelViewMatrix() );
//        glUniformMatrix4fv( M, 1, GL_FALSE,( float * )TransformObj->TransformGetModelMatrix() );
//        glm::mat4 matrix    = *(TransformObj->TransformGetModelViewMatrix());
//        //对RBT应用的normal矩阵省略计算，直接应用linear transformation部分
//        glm::mat3 normalMat = glm::mat3( glm::vec3(matrix[0]), glm::vec3(matrix[1]), glm::vec3(matrix[2]) );
//        glUniformMatrix3fv( NormalMatrix, 1, GL_FALSE, (float*)&normalMat );

        // Bind with Vertex Array Object for OBJ
        glBindVertexArray(OBJ_VAO_Id);
        
        GLUtils::checkGlError("before line drawing");

        // Draw Geometry
        glDrawArrays(GL_LINE_LOOP, 0, IndexCount );
        
        GLUtils::checkGlError("end line drawing");
        
        glBindVertexArray(0);
        TransformObj->TransformPopMatrix(); // Local Level
    }

    Model::Render();
    TransformObj->TransformPopMatrix();  // Parent Child Level
}

// Apply scenes light on the object
void LineModel::ApplyLight()
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
void LineModel::ApplyMaterial()
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


bool LineModel::IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint)
{
    return false;
}

void LineModel::SetLightPass(bool flag)
{
    isLightPass = flag;

    for(int i =0; i<GetChildren()->size(); i++){
        dynamic_cast<LineModel*>(GetChildren()->at(i))->SetLightPass( flag );
    }
}

void LineModel::SetSurfaceTextureId(GLuint surfaceTextureId){
    this->surfaceTextureId = surfaceTextureId;
}

void LineModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void LineModel::TouchEventMove( float x, float y )
{
}

void LineModel::TouchEventRelease( float x, float y )
{
}

void LineModel::AdjustTimeScale( bool x, float y){
    
}

