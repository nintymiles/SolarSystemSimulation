#include "ModelEx.h"
#include "Scene.h"

//目前的实现并不包含针对Model指定着色器
bool Model::setShader(PROGRAM* shaderProgram)
{
    program = shaderProgram;
	return false;
}

//模型构造函数必须传入scene对象，是因为要获得programObj，transformationObj。
Model::Model(Scene*	handler, Model* model, ModelType type, std::string objectName):Object(objectName, model)
{
    SceneHandler        = handler;
    ProgramManagerObj	= handler->SceneProgramManager();
    TransformObj		= handler->SceneTransform();
    modelType           = type;
    transformation      = glm::mat4();
    transformationLocal = glm::mat4();
    center              = glm::vec3(0.0, 0.0, 0.0);
    isVisible           = true;
}

Model::~Model()
{
    RemoveParent();
    RemoveFromParentChildList();
}

/*!
 Load textures, initialize shaders, etc.
 */
void Model::InitModel()
{
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->InitModel();
    }
}

void Model::Rotate( float angle, float x, float y, float z )
{
    transformation = glm::translate( transformation, center);
    transformation = glm::rotate( transformation, angle, glm::vec3( x, y, z ) );
    transformation = glm::translate( transformation, -center);
}

void Model::Translate(float x, float y, float z )
{
    transformation = glm::translate( transformation, glm::vec3( x, y, z ));
}

void Model::Scale(float x, float y, float z )
{
    transformation = glm::scale(transformation, glm::vec3( x, y, z ));
}

void Model::ScaleUniformly(float x){
    transformation = glm::scale(transformation, glm::vec3( x, x, x ));
}

void Model::RotateLocal( float angle, float x, float y, float z )
{
    transformationLocal = glm::rotate( transformationLocal, angle, glm::vec3( x, y, z ) );
}

void Model::TranslateLocal(float x, float y, float z )
{
    transformationLocal = glm::translate( transformationLocal, glm::vec3( x, y, z ));
}

void Model::ScaleLocal(float x, float y, float z )
{
    transformationLocal = glm::scale(transformationLocal, glm::vec3( x, y, z ));
}

void Model::ScaleLocalUniformly(float scaleFactor)
{
    ScaleLocal(scaleFactor,scaleFactor,scaleFactor);
}

glm::mat4 Model::GetEyeCoordinatesFromRoot()
{
    return  GetParentTransformation(dynamic_cast<Model*>(parent)) * transformation * transformationLocal;
}

glm::mat4 Model::GetParentTransformation(Model* parent)
{
    if(parent){
        return  GetParentTransformation(dynamic_cast<Model*>(parent->parent)) * parent->transformation;
    }
    return glm::mat4(); // return identity matrix.
}

void Model::Render()
{
    //render sub-objects recursively
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->Render();
    }
}

//执行射线投射动作，使用raster发出射线，然后计算相交。
//这个方法是raycast的核心，每个对象都会调用
vector<IntersectionData> Model::rayCast(RayCaster *rayCaster){
    vector<IntersectionData> intersects;
    //render sub-objects recursively
    for(int i =0; i<childList.size(); i++){
        Model *object = dynamic_cast<Model*>(childList.at(i));
        vector<IntersectionData> interData = object->rayCast(rayCaster);
        const Material objMaterial = object->GetMaterial();
//        Material material = Material(objMaterial);
//        if(interData.size()>0){
//            material.pickingAlpha = 0.5;
//            object->SetMaterial(material);
//        }
//        Material material = Material(objMaterial);
//        if(interData.size()>0){
//            if(material.pickingAlpha==1.0)
//                material.pickingAlpha = 0.5;
//            else
//                material.pickingAlpha = 1.0;
//            object->SetMaterial(material);
//        }
        
        for(auto data:interData)
            intersects.push_back(data);
    }
    return intersects;
}

void Model::TouchEventDown( float x, float y )
{
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->TouchEventDown( x, y );
    }
}

void Model::TouchEventMove( float x, float y )
{
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->TouchEventMove( x, y );
    }
}

void Model::TouchEventRelease( float x, float y )
{
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->TouchEventRelease( x, y );
    }
}

void Model::AdjustTimeScale (bool x, float y){
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->AdjustTimeScale( x, y );
    }
}

void Model::setSceneHandler(Scene* sceneHandle){
    SceneHandler = sceneHandle;
    for(int i =0; i<childList.size(); i++){
        dynamic_cast<Model*>(childList.at(i))->setSceneHandler( sceneHandle );
    }
}

void Model::SetVisible(bool flag, bool applyToChildren)
{
    isVisible = flag;
    
    if(applyToChildren){
        for(int i =0; i<childList.size(); i++){
            dynamic_cast<Model*>(childList.at(i))->SetVisible( flag, applyToChildren );
        }
    }
}

void Model::ApplyModelsParentsTransformation()
{
    *TransformObj->TransformGetModelMatrix() = *TransformObj->TransformGetModelMatrix()*transformation;
}

void Model::ApplyModelsLocalTransformation()
{
    *TransformObj->TransformGetModelMatrix() = *TransformObj->TransformGetModelMatrix()*transformationLocal;    
}

/*!
    DummyModel CTOR.
 */
DummyModel::DummyModel(Scene*	parentScene, Model* model, ModelType type,std::string objectName):Model(parentScene, model, type, objectName)
{
}

/*!
    DummyModel DTOR.
 */
DummyModel::~DummyModel()
{
    
}

/*!
    The render method does not render anything but applies the preserved relative transformation.
    The local transformation here does not make sense since the object cannot be rendered and additionally the local transformation is also never carried on the children.
 */
void DummyModel::Render()
{
    
    SceneHandler->SceneTransform()->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();
    
    // DUMMY RENDERING
    
    Model::Render();
    SceneHandler->SceneTransform()->TransformPopMatrix();  // Parent Child Level
}

