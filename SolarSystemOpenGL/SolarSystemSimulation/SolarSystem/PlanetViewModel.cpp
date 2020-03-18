#include "PlanetViewModel.h"

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <ctime>

#include <cmath>
#include <string>

#include "Cache.h"
#include "constant.h"

#include "perfMonitor.h"

#include "PlanetModel.h"
#include "SpaceModel.h"
#include "RingModel.h"

#include "LineModel.h"

// Namespace used
using std::ifstream;
using std::ostringstream;
using namespace glm;

static const int   SecInDay = 87600;
static const double EarthScale = (1.0/510)*0.00002;
static const float SpaceScale = 280.0;

static float tScale = 100000;
static double startTime = OpenGL_Helper::PerfMonitor::GetCurrentTime();

//the definitions of class static data member
vector<PlanetRenderData> PlanetViewModel::renderPlanetData;
vector<Model *> PlanetViewModel::planetModels;
vector<Model *> PlanetViewModel::trjacetoryModels;

void PlanetViewModel::adjustTimeScale(bool x,float y){
    if(x)
        tScale *= 2;
    else
        tScale /=2;
}

float PlanetViewModel::retrieveTimeScale(){
    return tScale;
}

//vector<PlanetRenderData> PlanetViewModel::retrieveRenderPlanetData(){
//    return renderPlanetData;
//}

/*!
	Simple Default Constructor

	\param[in] None.
	\return None

*/
PlanetViewModel::PlanetViewModel( Scene* parent, Model* model, ModelType type,string planetName,shared_ptr<Geometry> geometryptr):Model(parent, model, type)
{
	if (!parent)
		return;
    
    this->planetName = planetName;
    this->geometry = geometryptr;
    
    char fname[256]= {""};
#ifdef __APPLE__
    GLUtils::extractPath( getenv( "FILESYSTEM" ), fname );
#else
    strcpy( fname, "/sdcard/GLPIFramework/Images/" );
#endif
    
    string sunTexName = string(fname) + "sun_texture.jpg";
    string spaceTexName = string(fname) + "galaxy_starfield.png";
    
    image = new StbImage();
    image->loadImage((char *)sunTexName.c_str());
    
    
    planetMaterial = Material(MaterialNone);
    planetMaterial.shiness = 0.0001;
    planetMaterial.ambient = glm::vec4(0.1,0.1,0.1,1);
    
    sunMaterial = Material(MaterialNone);
    sunMaterial.shiness = 69.0; //不愧是sun
    sunMaterial.ambient = glm::vec4(0.5,0.2,0.1,1);
    
    
    Image* spaceImage = new StbImage();
    spaceImage->loadImage((char *)spaceTexName.c_str());
    
    //scene中的对象有多层级包含关系
    SceneHandler        = parent;
    
    SpaceModel *space = new SpaceModel(parent,this,None,geometry);
    Material spaceMaterial(MaterialNone);
    //spaceMaterial.ambient = glm::vec4(0.01,0.01,0.01,1); //给夜空增加漫射色彩，会让整个夜空相当明亮，尤其星星
    //spaceMaterial.shiness=1;  //夜空的亮度因子设置为5
//    space->SetMaterial(spaceMaterial);
//    space->SetName(std::string("Space"));
//    space->Scale(SpaceScale, SpaceScale, SpaceScale);
//    space->Translate(0.0, 0.0, 0.0);
//    space->SetSurfaceTextureId(spaceImage->getTextureID());
    
    //环状的绘制有问题，需要不断改进。
//    string ringTexName = string(fname) + "saturnringcolor.jpg";
//    StbImage *ringImage = new StbImage();
//    ringImage->loadImage((char *)ringTexName.c_str());
//    RingModel *ring = new RingModel( parent, this,  None );
//    ring->SetMaterial(planetMaterial);
//    ring->SetName(std::string("Ring"));
//    ring->ScaleLocalUniformly(120);
//    ring->Rotate(-90,1.0, 0.0, 0.0);
//    ring->SetSurfaceTextureId(ringImage->getTextureID());
    

    aPlanet = new PlanetModel(parent,this,None,geometry);
    aPlanet->SetCenter(glm::vec3(0.0, 0.0, 0.0));
    //sun->ScaleLocal(0.25, 0.25, 0.25);
    //aPlanet->RotateLocal(1.0, 0.0, 1.0, 0.0);
    
    

    starSurfaceImage = new StbImage();
    for (PlanetInfo planet:planetDat){
        PlanetRenderData prd;

        prd.name = planet.name;
        prd.theta = 0;
        prd.dTheta = (2*M_PI)/(planet.period_days * SecInDay);  //公转的角速度，单位raidant/sec
        prd.diameter = planet.diameter * 1000 * EarthScale;
        prd.distance = planet.distance_KM * EarthScale;
        prd.period = planet.period_days * tScale;
        prd.inclination = planet.inclination * (M_PI/180);
        prd.rotation = (2*M_PI)/(planet.rotation_days * SecInDay); //自转的角速度，单位raidant/sec

        prd.textureLocation = string(fname) + planet.name + "_texture.jpg";

        renderPlanetData.push_back(prd);
    }

    LoadPlanet(planetName);

}

void PlanetViewModel::LoadPlanet(string planetName){
    aPlanet->SetPlanetRadius(50.0);
    if(planetName == "Sun"){
        aPlanet->SetMaterial(Material(sunMaterial));
        aPlanet->SetSurfaceTextureId(image->getTextureID());
        aPlanet->SetName("Sun");
    }else{
        for(PlanetRenderData renderData:renderPlanetData){
            if(renderData.name == planetName){
                aPlanet->SetMaterial(Material(planetMaterial));
                image->loadImage((char *)renderData.textureLocation.c_str());
                starSurfaceImage->loadImage((char *)renderData.textureLocation.c_str());
                aPlanet->SetSurfaceTextureId(starSurfaceImage->getTextureID());
                aPlanet->SetName(planetName);
            }
        }
    }
}


//PlanetViewModel::~PlanetViewModel()
//{
//}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void PlanetViewModel::InitModel()
{

    Model::InitModel();
    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void PlanetViewModel::Render(){
    double now = OpenGL_Helper::PerfMonitor::GetCurrentTime();
    
    //dTime为程序启动后的时间差量
    double dTime = (now - startTime);
    
    //所有变换相关的设置，每次变换前都应重置
    //若要保持效果，只需要追踪变换矩阵变量的变化即可
    aPlanet->Reset();
    
    aPlanet->Translate(0.0, 0.0, 0.0);
    aPlanet->Rotate(dTime*3, 0, 1, 0);

    TransformObj->TransformPushMatrix(); // Parent Child Level
    ApplyModelsParentsTransformation();

    if(isVisible)
    {
        TransformObj->TransformPushMatrix(); // Local Level
        ApplyModelsLocalTransformation();
        TransformObj->TransformPopMatrix(); // Local Level
    }

    //由于每个Model都被加为CustomScene的子对象，故而此处渲染的就是所有的子对象
    //此方法的执行有些奇怪，类方法的继承体系稍微有别于其它语言。可以跨越中间继承体系的相同实现方法。
    Model::Render();
    TransformObj->TransformPopMatrix();  // Parent Child Level
}

void PlanetViewModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void PlanetViewModel::TouchEventMove( float x, float y )
{
    Model::TouchEventMove(x,y);
}

void PlanetViewModel::TouchEventRelease( float x, float y )
{
    Model::TouchEventRelease(x,y);
}

void PlanetViewModel::AdjustTimeScale( bool x, float y){
    if(x)
        tScale *= 2;
    else
        tScale /=2;
}

