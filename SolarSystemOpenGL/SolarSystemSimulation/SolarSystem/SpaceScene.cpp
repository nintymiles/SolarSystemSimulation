#include "SpaceScene.h"
#include "PlanetModel.h"
#include "SolarSystemModel.h"

int SpaceScene::planetIndex=0;
float SpaceScene::inclinationAngles=-20.0f;
float SpaceScene::fov_angles=45.0f;
glm::vec3 SpaceScene::cameraPosition=glm::vec3 (0.0, 100.0,250.0);

SpaceScene::SpaceScene(std::string name, Object* parentObj):Scene(name, parentObj)
{

    viewersPerspective  = NULL;
}

void SpaceScene::initializeScene()
{
    // Create scene's camera view.
    viewersPerspective = new Camera("Main Camera", this);
    viewersPerspective->SetClearBitFieldMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //从太阳正上方向下俯视的视角
//    viewersPerspective->SetPosition(glm::vec3 (0.0, 30.0,0.0));
//    viewersPerspective->Rotate(glm::vec3(1,0,0), -90.0);
//    fov_angles = 60.0;
    
    //最远的pluto距离太阳233
    //从太阳轨道平面平行或稍为抬高一点仰角的视线
    viewersPerspective->SetPosition(cameraPosition);
    viewersPerspective->Rotate(glm::vec3(1,0,0), inclinationAngles);
    fov_angles = 20.0;
    
//    //较大的仰角
//    viewersPerspective->SetPosition(glm::vec3 (0.0, 35.0,30.0));
//    viewersPerspective->Rotate(glm::vec3(1,0,0), -45.0);
    
    //仿threejs配置
//    viewersPerspective->SetPosition(glm::vec3 (0.0, 100.0,500.0));
    
    
    //near/far平面必须设置，否则在缩放fov时会出现投射问题
    viewersPerspective->SetNearPlane(1.0);
    viewersPerspective->SetFarPlane(2000.0);
    
    
    //viewersPerspective->Rotate(glm::vec3(1,0,0), -15.0);
    
    
    viewersPerspective->SetFov(fov_angles);

    //viewersPerspective->Rotate(glm::vec3(1,0,0), -90.0);
    //viewersPerspective->SetTarget(glm::vec3 (4.0, 0.0, 0.0));
    this->addCamera(viewersPerspective);
    Scene::initializeScene();
    
    rayCaster = new RayCaster();
}

SpaceScene::~SpaceScene(void)
{
}

void SpaceScene::resize( int w, int h)
{
//    lightPerspective->Viewport(0, 0, w, h);
    viewersPerspective->Viewport(0, 0, w, h);
    Scene::resize(w, h);
}

/*!
	Performs rendering for each model
 
	\param[in] void.
 
	\return void.
 */
void SpaceScene::render()
{
    viewersPerspective->SetClearColor();
    // View the scene from camera
    viewersPerspective->Render();
    
    return Scene::render();
    
//    // Cull the back face of the objects.
//    glCullFace(GL_BACK);
//
//    // Disable the polygon offset filling
//    glDisable(GL_POLYGON_OFFSET_FILL);
    
//    // Render the models
//    for( int i=0; i<models.size();  i++ ){
//        currentModel = models.at(i);
//        if(!currentModel){
//            continue;
//        }
//
//        // PASS TWO => Normal scene rendering
//        // Set LIGHT PASS to False, render the scene using fragment shader.
//        //((PlanetModel*)currentModel)->SetLightPass(false);
//
//        currentModel->Render();
//    }
//    return;
}

void SpaceScene::adjustFOV(int optionIdx){
    switch (optionIdx) {
        case 0:
        
            fov_angles +=2.5;
            if(fov_angles>=100)
                fov_angles = 100;
        
            viewersPerspective->SetFov(fov_angles);
            break;
            
        case 1:
            
            fov_angles -=2.5;
            if(fov_angles<=1)
                fov_angles = 1;
            
            viewersPerspective->SetFov(fov_angles);
            break;
            
        case 2:
            
            inclinationAngles +=2.5;
            if(inclinationAngles >= 90.0)
                inclinationAngles = 90.0;
            
            viewersPerspective->StrafeUpside(0.3f);
            
//            viewersPerspective->Reset();
//            viewersPerspective->SetPosition(cameraPosition);
            //viewersPerspective->Rotate(glm::vec3(1,0,0), inclinationAngles * DEG_TO_RAD);
//            viewersPerspective->SetFov(fov_angles);
//            viewersPerspective->SetNearPlane(1.0f);
//            viewersPerspective->SetFarPlane(2000.0f);
            break;
            
        case 3:
            
            inclinationAngles -=2.5;
            if(inclinationAngles <= -90.0)
                inclinationAngles = -90.0;
            
            viewersPerspective->StrafeDownside(0.3f);
            //viewersPerspective->Rotate(glm::vec3(1,0,0), -inclinationAngles * DEG_TO_RAD);
//            viewersPerspective->Reset();
//            viewersPerspective->SetPosition(cameraPosition);
//            viewersPerspective->Rotate(glm::vec3(1,0,0), inclinationAngles * DEG_TO_RAD);
//            viewersPerspective->SetFov(fov_angles);
//            viewersPerspective->SetNearPlane(1.0f);
//            viewersPerspective->SetFarPlane(2000.0f);
            break;
            
        case 4:
            switchPlanetViewPostion();
            break;
            
        case 5:
            viewersPerspective->StrafeLeftSide(1);
            break;
            
        case 6:
            viewersPerspective->StrafeRightSide(1);
            break;
        
        default:
            break;
    }
    
}

void SpaceScene::switchPlanetViewPostion(){
    vector<PlanetRenderData> renderData=SolarSystemModel::retrieveRenderPlanetData();
    
    planetIndex++;
    if(planetIndex>(int)renderData.size()){
        planetIndex=0;
    }
    if(planetIndex==(int)renderData.size()){
        viewersPerspective->Reset();
        viewersPerspective->SetPosition(cameraPosition);
        //viewersPerspective->SetTarget(glm::vec3(0,0,0));
        viewersPerspective->SetFov(fov_angles);
        viewersPerspective->Rotate(glm::vec3(1,0,0), inclinationAngles);
        viewersPerspective->SetNearPlane(1.0f);
        viewersPerspective->SetFarPlane(2000.0f);
        
        return;
    }
    
    PlanetRenderData prd = renderData[planetIndex];
    glm::vec3 planetPosition=prd.translation;
    float cameraDistance = prd.distance-prd.diameter-1.5;
    glm::vec3 cameraPosition = glm::normalize(planetPosition) * cameraDistance;
    //viewersPerspective->Reset();
    viewersPerspective->SetFov(90.0f);
    viewersPerspective->SetPosition(cameraPosition);
    viewersPerspective->SetTarget(prd.translation);
    
}

void SpaceScene::TouchEventDown(float x, float y){
    Scene::TouchEventDown(x, y);
    rayCaster->setFromCamera(vec3(x,y,0.0f), viewersPerspective);
    
    vector<IntersectionData> intersects;
    for( int i=0; i<models.size(); i++ ){
        vector<IntersectionData> interData = models.at(i)->rayCast(rayCaster);
        for(auto data:interData)
            intersects.push_back(data);
    }
    
    for(IntersectionData interData:intersects){
        const Material objMaterial = interData.object->GetMaterial();
        Material material = Material(objMaterial);
        if(material.pickingAlpha==1.0)
            material.pickingAlpha = 0.5;
        else
            material.pickingAlpha = 1.0;
        
        interData.object->SetMaterial(material);
        
    }
    
}

