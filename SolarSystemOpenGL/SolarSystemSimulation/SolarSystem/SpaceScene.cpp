#include "SpaceScene.h"
#include "PlanetModel.h"
#include "SolarSystemModel.h"

int SpaceScene::planetIndex=0;
float SpaceScene::inclinationAngles=-20.0f;
float SpaceScene::fov_angles=45.0f;
glm::vec3 SpaceScene::cameraPosition=glm::vec3 (0.0, 100.0,250.0);

//prepare FBO and attach FBO/Textures
SpaceScene::SpaceScene(std::string name, Object* parentObj):Scene(name, parentObj)
{
//    //Create the Frame buffer object
//    fbo = new FrameBufferObjectSurface();
//
//    // Generate the FBO ID
//    fbo->GenerateFBO();
//
//    depthTexture.generateTexture2D(GL_TEXTURE_2D, fbo->GetWidth(), fbo->GetHeight(), GL_DEPTH_COMPONENT32F, GL_FLOAT, GL_DEPTH_COMPONENT,0,true,0,0,GL_CLAMP_TO_EDGE,GL_CLAMP_TO_EDGE,GL_NEAREST,GL_NEAREST );
//
//    // Attach the Depth Buffer to FBO's depth attachment
//    fbo->AttachTexture(depthTexture, GL_DEPTH_ATTACHMENT);
//
//    // Check the status of the FBO
//    fbo->CheckFboStatus();
    
    lightPerspective    = NULL;
    viewersPerspective  = NULL;
}

void SpaceScene::initializeScene()
{
//    // Create camera view from lights perspective in order to capture depth buffer.
//    lightPerspective = new Camera("lightPerspective", this);
//    // clear color/depth buffer from viewer's perspective point
//    lightPerspective->SetClearBitFieldMask(GL_DEPTH_BUFFER_BIT);
//    // set position of camera to the position of first light source
//    lightPerspective->SetPosition(glm::vec3(this->lights.at(0)->position));
//    lightPerspective->SetTarget(glm::vec3 (0.0, 0.0,0.0));
//    this->addCamera(lightPerspective);
    
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
//    // Set Framebuffer to the FBO
//    //绑定要输出的FBO，一个渲染输出通道
//    //如果需要离屏输出，则定义一个FrameBufferObject，在渲染前执行push操作，渲染后执行pop操作即可
//    fbo->Push();
//
//    // Render the scene from lights perspective
//    //基本的渲染视角变幻矩阵设置
//    lightPerspective->Render();
//
//    // Enable the back face culling.
//    glEnable(GL_CULL_FACE);                 // Important to avoid culling artefacts
//    glCullFace(GL_FRONT);
//
//    // Enable the polygon offset filling, in order to avoid the artefact on the object backside faces.
//    glEnable(GL_POLYGON_OFFSET_FILL);
//    glPolygonOffset(2.5f, 20.0f);
//
//    for( int i=0; i<models.size();  i++ ){
//        currentModel = models.at(i);
//        if(!currentModel){
//            continue;
//        }
//
//        //这个框架将每个Model作为一个基本渲染单位，Model中包括对应的渲染基本的几何数据，shader配置
//        //当然也可以独立设置material，texture等数据，然后在model加入到场景中，场景在加入到渲染器中
//        //就可以实现复杂场景的渲染，虽然整体比较松散，但是易于理解，相比FCG中的封装更让人感觉轻快，其实现
//        //接近底层。
//        // Set LIGHT PASS (PASS ONE) to True, this will avoid fragment
//        // shader execution and only records depth buffer.
//        ((ObjLoader*)currentModel)->SetLightPass(true);
//        currentModel->Render();
//    }
//    // Reset to previous frame buffer
//    fbo->Pop();

    // Bind the texture unit 0 to depth texture of FBO
//    glActiveTexture (GL_TEXTURE0);
//    glBindTexture(GL_TEXTURE_2D, depthTexture.getTextureID());

    
    // View the scene from camera
    viewersPerspective->Render();
    
    // Cull the back face of the objects.
    glCullFace(GL_BACK);
    
    // Disable the polygon offset filling
    glDisable(GL_POLYGON_OFFSET_FILL);
    
    // Render the models
    for( int i=0; i<models.size();  i++ ){
        currentModel = models.at(i);
        if(!currentModel){
            continue;
        }
        
        // PASS TWO => Normal scene rendering
        // Set LIGHT PASS to False, render the scene using fragment shader.
        //((PlanetModel*)currentModel)->SetLightPass(false);
        
        currentModel->Render();
    }
    return;
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

//Camera* SpaceScene::GetLightPerspective()
//{
//    return lightPerspective;
//}

