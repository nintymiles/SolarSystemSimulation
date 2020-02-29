#ifndef MODEL_H
#define MODEL_H

#include "constant.h"
#include "ProgramManager.h"
#include "Transform.h"
#include "Material.h"
#include "Object.h"
#include "Ray.h"
#include "Sphere.h"
#include "Box.h"
#include "Event.h"

#include <vector>
#include <glm/glm.hpp>
using glm::vec3;

//当模型中也加入scene等parent level的对象后，cycle reference已经形成，这种情形下，scene对象应该是
//弱引用类型
//! Forward declaration
class Scene;
class ProgramManager;
class RayCaster;
class Model;

//基础相交数据结构定义，如果针对mesh结构几何数据，可能还不足够
struct IntersectionData {
    float distance;
    vec3 intersectionPointWorld;
    Model* object;
};

class RayCast{
public:
    virtual bool isPickingEnabled()=0;
    virtual vector<IntersectionData> rayCast(RayCaster *rayCaster)=0;
};

// Interface for the rendering models.
class Model : public Object, public GestureEvent,public RayCast{

public:
    
    //构造函数
    Model(Scene* SceneHandler, Model* model, ModelType type,std::string objectName = "");
    
    virtual ~Model();
    
    /*!
      Load textures, initialize shaders, etc.
      */
    virtual void InitModel();

    /*!
      This is called prior to every frame.  Use this
      to update your animation.
      */
	virtual void Update( float t ) {}

    /*!
      Draw your Model.
      */
    virtual void Render() = 0;

    /*!
      Called when screen is resized
      */
	virtual void Resize(int, int) {}
    
    /*!
     Reset Model transformation data
     */
    virtual void Reset(){
        transformation = glm::mat4();
        transformationLocal = glm::mat4();
    }
    
    /*!
     用于决定是否执行picking渲染
     Specify whether or not to perform picking rendering
     */
    virtual bool isPickingEnabled(){
        //for default,disable picking action
        return true;
    }


    /*!
      Called when screen is resized
      */
	virtual ModelType GetModelType() { return modelType; }

    /*!
      Select the specified program for rendering current Model.
      */
	virtual bool setShader(PROGRAM* shaderProgram);

	/*!
      The method will be called before rendering, this help to set the different states of the model.
      */
	virtual void setStates(){}

	/*!
      The method will be called after rendering, this help to set the different states of the model.
      */
	virtual void releaseStates(){}

    void TouchEventDown( float a, float b );

    void TouchEventMove( float a, float b );

    void TouchEventRelease( float a, float b );
    
    void AdjustTimeScale (bool forward, float factor);
    
    void Rotate( float angle, float x, float y, float z );
    
    void Translate(float x, float y, float z );
    
    void Scale(float x, float y, float z );
    
    void ScaleUniformly(float scaleFactor);

    void RotateLocal( float angle, float x, float y, float z );
    
    void TranslateLocal(float x, float y, float z );
    
    void ScaleLocal(float x, float y, float z );
    void ScaleLocalUniformly(float scaleFactor);
    
    glm::mat4 GetEyeCoordinatesFromRoot();

    glm::mat4 GetParentTransformation(Model* parent);

    void SetMaterial(Material mat){ materialObj = mat; }
    
    const Material& GetMaterial(){ return materialObj; }
    
    void setSceneHandler(Scene* sceneHandle);
    
    void SetCenter(glm::vec3 centerPoint){center = centerPoint;}
    
    glm::vec3 GetCenter(){ return center; }
    
    void SetVisible(bool flag, bool applyToChildren = false);

    bool GetVisible(){ return isVisible; }
    
    //! Applies parent's transformation, this will be carried on to the children.
    void ApplyModelsParentsTransformation();

    //! Applies the local transformation, this will not affect the children
    void ApplyModelsLocalTransformation();
    
    virtual vector<IntersectionData> rayCast(RayCaster *rayCaster);
    
protected:
    /*!
      Store current program ID
      */
	unsigned int	ProgramID;

	/*!
      Store current program metaData
      */
	PROGRAM*		program;

	/*!
      Store current program handler
      */
	ProgramManager* ProgramManagerObj;

	/*!
      Handle Projection-View current program metaData
      */
	Transform*	TransformObj;

	/*!
      Drawing engine
      */
	Scene*	SceneHandler;

	/*!
      Type of the model rendered
      */
	ModelType		modelType;
    
    /*!
    Transformation of the model
     */
    glm::mat4 transformation;

    /*!
     Local transformation of the model
     */
    glm::mat4 transformationLocal;

    /*!
     Local transformation of the model
     */
    glm::vec3 center;
    
    Material materialObj;
    
    vector<vec3> points;
    Sphere* boundingSphere;
    Box* boundingBox;
    
    
    bool isVisible;

};

// Dummy model does render any thing, its used for setting dummy parents.
class DummyModel : public Model{
public:
    // Constructor
    DummyModel(Scene*	SceneHandler, Model* model, ModelType type,std::string objectName = "");

    // Destructor
    virtual ~DummyModel();

    // Render the dummy model.
    void Render();
};
#endif // MODEL_H
