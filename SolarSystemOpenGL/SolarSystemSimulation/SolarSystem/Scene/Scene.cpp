#include "Scene.h"
#include "ObjLoader.h"
#include "Button.h"


Scene::Scene(std::string name, Object* parentObj):Object(name, parentObj)
{
    renderManager   = NULL;
    currentCamera   = NULL;
    currentModel    = NULL;

    if(parentObj){
        Renderer* renderEngine = dynamic_cast<Renderer*>(parentObj);
        if(renderEngine){
            renderEngine->addScene(this); //加入父对象的体系中去
        }
    }
}

Scene::~Scene(void)
{
}

void Scene::initializeScene()
{
    LOGI("initializeScene");
    setUpModels();
}

/*!
	Remove all the Model classes from the Scene.
 
	\param[in] None.
 
	\return None.
 */
void Scene::clearModels()
{
    for( int i=0; i<models.size();  i++ ){
        delete models.at(i);
    }
    models.clear();
}

/*!
	Setup of the view port and projection initialization. In projection initialization the Ortho or Perspective is set
	as per requirement.
 
	\param[in] width of the screen.
	\param[in] height of the screen.
 
	\return void.
 */
void Scene::resize(int w, int h)
{
    screenWidth		= w;
    screenHeight	= h;
}

/*!
	Create and Initializes all models.
 
	\param[in] void.
 
	\return void.
 */
void Scene::setUpModels()
{
    //! Do the init stuff as per model requirements
    initializeModels();
}

/*!
	This returns the model Scene of the model. A Model Scene is basically a class responsible for managing the same type of models.
 
	\param[in] type Enumeration of the model type want to query.
 
	\return Model pointer.
 */
Model* Scene::getModel( ModelType type)
{
    for( int i=0; i<models.size();  i++ )
    {
        Model* model = models.at(i);
        
        if ( model->GetModelType() == type )
            return model;
    }
    
    return NULL;
}

/*!
	This function is responsible for adding a model in the Scene model vector.
	The Scene handle all the models from a unique vector based list.
 
	\param[in] Model pointer of the Model user want to add.
 
	\return None.
 */
void Scene::addModel(Model* model)
{
    if(!model){
        return;
    }
    
    // Add root model only
    if(model->GetParent() == NULL){
        models.push_back( model );
        model->setSceneHandler(this);
    }
}

//! Add lights
void Scene::addLight( Light* lightObj)
{
    for(int i =0; i<lights.size(); i++){
        if(lights.at(i) == lightObj)
            return;
    }
    lights.push_back(lightObj);
}


void Scene::removeModel(Model* model)
{
    model->RemoveParent();
    
    for(int i =0; i<models.size(); i++){
        if(model == models.at(i)){
            models.erase (models.begin()+i);
            printf("Removed from parent");
            return;
        }
    }
}

//! Get the renderer of the scene
Renderer* Scene::getRenderer()
{
    return renderManager;
}

//! Set the renderer of the scene
void Scene::setRenderer( Renderer* renderer)
{
    if( !renderer ){
        return;
    }
    if (renderManager){
        // this scene contains in some other renderer, remove from it first;
        renderManager->removeScene(this);
        renderManager = NULL;
    }
    
    renderer->addScene(this);
    renderManager = renderer;
}

/*!
	This function handle Touch event down action.
 
	\param[in] x and y screen pixel position.
 
	\return None.
 */
void Scene::TouchEventDown( float x, float y )
{
    for( int i=0; i<models.size(); i++ ){
        models.at(i)->TouchEventDown(x, y);
    }
}

/*!
	This function handle Touch event move action.
 
	\param[in] x and y screen pixel position.
 
	\return None.
 */
void Scene::TouchEventMove( float x, float y )
{
    for( int i=0; i<models.size(); i++ ){
        models.at(i)->TouchEventMove(x, y);
    }
}

/*!
	This function handle Touch event release action.
 
	\param[in] x and y screen pixel position.
 
	\return None.
 */
void Scene::TouchEventRelease( float x, float y )
{
    for( int i=0; i<models.size(); i++ ){
        models.at(i)->TouchEventRelease(x, y);
    }
}

void Scene::AdjustTimeScale( bool x, float y){
    for( int i=0; i<models.size(); i++ ){
        models.at(i)->AdjustTimeScale(x, y);
    }
}

//void Scene::adjustFOV(){
//    
//};

/*!
	Performs initializing of all the models this may include setup for shaders their caching.
	rendering for each model, creation and caching of IBO, VBO, FBO etc.
 
	\param[in] void.
 
	\return void.
 */
void Scene::initializeModels()
{
    for( int i=0; i<models.size();  i++ )
        models.at(i)->InitModel();
}

/*!
	Performs rendering for each model
 
	\param[in] void.
 
	\return void.
 */
void Scene::render()
{
    for(int j=0; j<cameras.size(); j++){
        
        currentCamera = cameras.at(j);
        if(!currentCamera) {
            continue;
        }
        currentCamera->Render();
    
        for( int i=0; i<models.size();  i++ ){
            currentModel = models.at(i);
            if(!currentModel){
                continue;
            }
                
            currentModel->Render();
        }
    }
    
    // On Finish set the current camera and model to NULL
    currentCamera   = NULL;
    currentModel    = NULL;
}

//! Get the camera from the scene
Camera* Scene::getCamera( std::string camName )
{
    for(int i=0;i<cameras.size(); i++){
        Camera* camera = cameras.at(i);
        if(camera->GetName().compare(camName) == 0){
            return camera;
        }
    }
    
    return NULL;
}

//! Add the camera to the scene
void Scene::addCamera( Camera* cameraObj )
{
    if(!cameraObj){
        return;
    }
    cameraObj->SetParent(this);
    if(!getCamera(cameraObj->GetName())){
        cameras.push_back( cameraObj );
    }
}


