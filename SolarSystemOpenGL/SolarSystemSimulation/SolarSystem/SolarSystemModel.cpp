#include "SolarSystemModel.h"

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
vector<PlanetRenderData> SolarSystemModel::renderPlanetData;
vector<Model *> SolarSystemModel::planetModels;
vector<Model *> SolarSystemModel::trjacetoryModels;

void SolarSystemModel::adjustTimeScale(bool x,float y){
    if(x)
        tScale *= 2;
    else
        tScale /=2;
}

float SolarSystemModel::retrieveTimeScale(){
    return tScale;
}

vector<PlanetRenderData> SolarSystemModel::retrieveRenderPlanetData(){
    return renderPlanetData;
}

/*!
	Simple Default Constructor

	\param[in] None.
	\return None

*/
SolarSystemModel::SolarSystemModel( Scene* parent, Model* model, ModelType type):Model(parent, model, type)
{
	if (!parent)
		return;
    
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
    
    
    Material planetMaterial(MaterialNone);
    planetMaterial.shiness = 0.0001;
    planetMaterial.ambient = glm::vec4(0.1,0.1,0.1,1);
    
    Material sunMaterial(MaterialNone);
    sunMaterial.shiness = 69.0; //不愧是sun
    sunMaterial.ambient = glm::vec4(0.5,0.2,0.1,1);
    
    
    Image* spaceImage = new StbImage();
    spaceImage->loadImage((char *)spaceTexName.c_str());
    
    //scene中的对象有多层级包含关系
    SceneHandler        = parent;
    
    SpaceModel *space = new SpaceModel( parent, this,  None );
    Material spaceMaterial(MaterialNone);
    //spaceMaterial.ambient = glm::vec4(0.01,0.01,0.01,1); //给夜空增加漫射色彩，会让整个夜空相当明亮，尤其星星
    //spaceMaterial.shiness=1;  //夜空的亮度因子设置为5
    space->SetMaterial(spaceMaterial);
    space->SetName(std::string("Space"));
    space->Scale(SpaceScale, SpaceScale, SpaceScale);
    space->Translate(0.0, 0.0, 0.0);
    space->SetSurfaceTextureId(spaceImage->getTextureID());


    
    sun = new PlanetModel( parent, this,  None );
    sun->SetMaterial(Material(sunMaterial));
    sun->SetName(std::string("Sun"));
    sun->Translate(0.0, 0.0, 0.0);
    sun->SetCenter(glm::vec3(0.0, 0.0, 0.0));
    //sun->ScaleLocal(0.25, 0.25, 0.25);
    sun->RotateLocal(1.0, 0.0, 1.0, 0.0);
    sun->SetSurfaceTextureId(image->getTextureID());
    
    //环状的绘制有问题，需要不断改进。
//    string ringTexName = string(fname) + "saturnringcolor.jpg";
//    StbImage *ringImage = new StbImage();
//    image->loadImage((char *)ringTexName.c_str());
//    RingModel *ring = new RingModel( parent, this,  None );
//    ring->SetMaterial(MaterialCopper);
//    ring->SetName(std::string("Ring"));
//    ring->Scale(10, 10, 10);
//    ring->Translate(0.0, 0.0, 0.0);
//    ring->SetSurfaceTextureId(ringImage->getTextureID());
    
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

    for(PlanetRenderData renderData:renderPlanetData){
        LineModel *trjacetory = new LineModel(parent,this,None);
        trjacetory->ScaleLocalUniformly(renderData.distance);
        //glm默认使用degrees，只有定义了宏GLM_FORCE_RADIANS，才会强制使用radians
        trjacetory->RotateLocal(90, 1, 0, 0);
        trjacetory->Rotate(renderData.inclination, 1, 0, 0);
        
        if(renderData.name == "saturn"){
//            RingModel *ring = new RingModel( parent, this,  None );
//            ring->SetMaterial(spaceMaterial);
//            ring->SetName(std::string("Ring"));
//            ring->Scale(10, 10, 10);
//            ring->Translate(0.0, 0.0, 0.0);
//            ring->SetSurfaceTextureId(image->getTextureID());
        }
        
        PlanetModel *star = new PlanetModel( parent, this,  None );
        star->SetMaterial(Material(planetMaterial));
        star->SetName(renderData.name);

        //渲染数据中的变换数据在每次绘制前都要设置一次
//        star->Scale(renderData.diameter, renderData.diameter, renderData.diameter);
//        star->Translate(renderData.distance, 0.0, 0.0);
//        star->SetCenter(glm::vec3(10.0, 2.0, 0.0));
//        star->ScaleLocal(2.0, 2.0, 2.0);

        Image *startSurfaceImage = new StbImage();
        //const char *fileName = renderData.textureLocation.c_str();
        startSurfaceImage->loadImage((char *)renderData.textureLocation.c_str());

        star->SetSurfaceTextureId(startSurfaceImage->getTextureID());

        planetModels.push_back(star);
    }

}


/*!
	Simple Destructor

	\param[in] None.
	\return None

*/
SolarSystemModel::~SolarSystemModel()
{
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SolarSystemModel::InitModel()
{

    Model::InitModel();
    return;
}

/*!
	Initialize the scene, reserve shaders, compile and cache program

	\param[in] None.
	\return None

*/
void SolarSystemModel::Render()
{
    double now = OpenGL_Helper::PerfMonitor::GetCurrentTime();
    
    //dTime为两次渲染调用之间的时间差量，乘以时间放大的因子
    double dTime = (now - startTime) * tScale;
    startTime = now;
    
    for(int i=0;i<(int)renderPlanetData.size();i++){
        //当前行星的渲染数据
        PlanetRenderData &renderData = renderPlanetData[i];


        //获取当前行星model
        Model* planetModel = planetModels[i];
        //重设planet model的变换数据
        planetModel->Reset();

        if(renderData.name == "pluto"){
            planetModel->ScaleLocal(renderData.diameter*10, renderData.diameter*10, renderData.diameter*10);
        }else
            planetModel->ScaleLocal(renderData.diameter, renderData.diameter, renderData.diameter);

        //planetModel->Translate(renderData.distance, 0.0, 0.0);
        //planetModel->SetCenter(glm::vec3(10.0, 2.0, 0.0));
        //planetModel->ScaleLocal(2.0, 2.0, 2.0);
//        transformationLocal[0][0] = transformationLocal[1][1] = transformationLocal[2][2] = transformationLocal[3][3] = 1.0;
//        planetModel->SetCenter(glm::vec3(10.0, 2.0, 0.0));
//        planetModel->ScaleLocal(2.0, 2.0, 2.0);


        //考虑时间放大因子后，实际时间流逝累加的自转角度，设置围绕y轴的旋转角度。
        renderData.euler.y += renderData.rotation * dTime;
        //围绕当前planetModel的y轴旋转
        planetModel->RotateLocal(renderData.euler.y, 0.0, 1.0, 0.0);




        //!公转累加角度的计算和存储
        double dTheta = renderData.dTheta * dTime;  //dTheta为时间差内公转的角度
        renderData.theta += dTheta; //累加公转的角度

        double phi = renderData.inclination * sin(renderData.theta); //inclination可以理解为随着公转角度的仰角变化速率

        //LOGI("Planet name = %s , inclination = %f \n",renderData.name.c_str(),phi);
        //LOGI("Planet name = %s , distance = %f \n",renderData.name.c_str(),renderData.distance);

        //通过地平经角和极角，再辅助以距离太阳的距离，来确定行星当前的位置。
        //Determine x,y, and z coordinates of planets based off theta + phi
        float tz = (renderData.distance) * sin(renderData.theta);
        float tx = (renderData.distance) * cos(renderData.theta);
        float ty = -(renderData.distance) * cos(M_PI/2 - phi);
        renderData.translation = glm::vec3(tx,ty,tz);
        planetModel->Translate(tx, ty, tz);

    }
    
//    sun->Rotate(-2, 0, 1, 0);
//    mercury->Reset();
//    //mercury->UniformlyScale(renderPlanetData[0].diameter);
//    mercury->Translate(renderPlanetData[0].translation.x, renderPlanetData[0].translation.y, renderPlanetData[0].translation.z);
//    mercury->Rotate(renderPlanetData[0].euler.y, 0, 1, 0);
//    mercury->Rotate(-2, 1, 0, 0);

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

void SolarSystemModel::TouchEventDown( float x, float y )
{
    Model::TouchEventDown(x,y);
}

void SolarSystemModel::TouchEventMove( float x, float y )
{
    Model::TouchEventMove(x,y);
}

void SolarSystemModel::TouchEventRelease( float x, float y )
{
    Model::TouchEventRelease(x,y);
}

void SolarSystemModel::AdjustTimeScale( bool x, float y){
    if(x)
        tScale *= 2;
    else
        tScale /=2;
}

//// Discuss in There More section.
//
//bool SolarSystemModel::IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint)
//{
//    Model::IntersectWithRay(ray0, intersectionPoint);
//    return false;
//}

