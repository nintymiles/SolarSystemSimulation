#ifndef SCENE_H
#define SCENE_H
#include "ModelEx.h"
#include "ProgramManager.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"
#include "Event.h"
#include <vector>

class Renderer;

//对比threejs，有相当的相似度，除了camera在threejs中直接在renderer中使用之外
class Scene : public Object, public GestureEvent
{
    
public:
    //! Default Constructor
    Scene(std::string name = "", Object* parentObj = NULL);
    
    //! Destructor
    virtual ~Scene(void);
    
    //! Initialize Scene
    virtual void initializeScene();
    
    //! Return the ProgramManager handler
    inline ProgramManager* SceneProgramManager(){ return &ProgramManagerObj; }
    
    //! Return the Transform handler
    inline Transform*  SceneTransform() { return &TransformObj;  }
    
    //! Screen Width returned
    inline int screenWidthPixel() { return screenWidth; }
    
    //! Screen Height returned
    inline int screenHeightPixel() { return screenHeight; }
        
    //! Render the Models
    virtual void render();
    
    //! Initializes the Models
    virtual void initializeModels();
    
    //! Remove all the models
    virtual void clearModels();
    
    //! Initializes the Models
    virtual void setUpModels();
    
    //! Get the model
    Model* getModel( ModelType );
    
    //! Cache the model
    void addModel( Model* );

    //! Add lights
    void addLight( Light* );
    
    //! Get the camera from the scene
    Camera* getCamera( std::string camName );
    
    //! Add the camera to the scene
    void addCamera( Camera* cameraObj );
    
    //! Get the renderer of the scene
    Renderer* getRenderer();
    
    //! Set the renderer of the scene
    void setRenderer( Renderer* renderer);
    
    void removeModel(Model*);
    
    void TouchEventDown( float a, float b );
    
    void TouchEventMove( float a, float b );
    
    void TouchEventRelease( float a, float b );
    
    void AdjustTimeScale( bool x, float y);
    
    //! resize screen
    virtual void resize( int w, int h );
    
    virtual void adjustFOV(int optionIdx)=0;
    
    virtual void switchPlanetViewPostion()=0;
    
    std::vector <Light*>& getLights(){ return lights; }
protected:

    //! Program handler for render
    ProgramManager	ProgramManagerObj;
    
    //! Transform operation handler
    Transform	TransformObj;
    
    //! Screen Height
    int screenHeight;
    
    //! Screen Width
    int screenWidth;
    
    //! Model List
    std::vector <Model*> models;

    //! Light List
    std::vector <Light*> lights;

    //! Model List
    std::vector <Camera*> cameras;
    
    
    //! Renderer in which scene is executing
    Renderer* renderManager;

    // Current Camera in use
    Camera* currentCamera;
    
    // Current Model in use
    Model* currentModel;
};

#endif
