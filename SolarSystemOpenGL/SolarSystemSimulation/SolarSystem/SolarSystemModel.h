//
//  SolarSystemModel.h
//
//
//  Created by seanren on 1/30/20.
//  Copyright (c) 2020 seanren. All rights reserved.
//

#ifndef SOLAR_SYSTEM_MODEL_H
#define SOLAR_SYSTEM_MODEL_H

#include <iostream>

#include "ModelEx.h"
#include "GLutils.h"
#include "RendererEx.h"

#include "PlanetModel.h"

#include "PngImage.h"
#include "Event.h"

#include <string>

#include <map>
#include <vector>

using namespace std;
using namespace namespaceimage;

//太阳系各天体的数据信息
struct PlanetInfo{
    string name;
    long distance_KM;
    float distance_AU;
    long diameter;
    float period_days;
    float rotation_days;
    float inclination;
};

//用于渲染的天体数据
struct PlanetRenderData{
    string name;
    double theta;
    double dTheta;  //公转的角速度，单位raidant/sec
    float diameter;
    float distance;
    float period;
    double inclination;
    double rotation;
    glm::vec3 euler;
    glm::vec3 translation;
    string textureLocation;
};

static vector<PlanetInfo> planetDat = {

    {.name="mercury",.distance_KM=57910000,.distance_AU=0.387,.diameter=4800,.period_days=88,.rotation_days=59,.inclination=7},
    {.name="venus",.distance_KM=108200000,.distance_AU=0.732,.diameter=12100,.period_days=225,.rotation_days=243,.inclination=3.9},
    {.name="earth",.distance_KM=149600000,.distance_AU=1,.diameter=12750,.period_days=365,.rotation_days=1,.inclination=0},
    {.name="mars",.distance_KM=227940000,.distance_AU=1.524,.diameter=6800,.period_days=697,.rotation_days=1.017,.inclination=1.85},
    {.name="jupiter",.distance_KM=778330000,.distance_AU=5.203,.diameter=142800,.period_days=4234,.rotation_days=0.417,.inclination=1.3},
    {.name="saturn",.distance_KM=1424600000,.distance_AU=9.523,.diameter=120660,.period_days=10752.9,.rotation_days=0.433,.inclination=2.49},
    {.name="uranus",.distance_KM=2873550000,.distance_AU=19.208,.diameter=51800,.period_days=30660,.rotation_days=0.708,.inclination=0.77},
    {.name="neptune",.distance_KM=4501000000,.distance_AU=30.087,.diameter=49500,.period_days=60225,.rotation_days=0.833,.inclination=1.77},
    {.name="pluto",.distance_KM=5945900000,.distance_AU=39.746,.diameter=3300,.period_days=90520,.rotation_days=6.4,.inclination=17.2}
    
};


class SolarSystemModel : public Model
{
    
public:
    // Constructor for SolarSystemModel
    SolarSystemModel( Scene* parent, Model* model, ModelType type);

    // Destructor for SolarSystemModel
    virtual ~SolarSystemModel();

    // Initialize our Model class
    void InitModel();
    
    // Render the Model class
    void Render();
    
    // Touch Events
	void TouchEventDown( float x, float y );
    
	void TouchEventMove( float x, float y );
	
    void TouchEventRelease( float x, float y );
    
    void AdjustTimeScale( bool x, float y);

    //bool IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint);
    static void adjustTimeScale(bool x,float y);
    static float retrieveTimeScale();
    
    static vector<PlanetRenderData> retrieveRenderPlanetData();

private:
    char MVP;
    char TEX;
    Image* image;
    glm::vec2 texCoordinates[4];
    glm::vec3 vertices[4];
    char* textureImage;
    
    static vector<PlanetRenderData> renderPlanetData;
    static vector<Model*> planetModels;
    static vector<Model*> trjacetoryModels;
    
    PlanetModel* sun;
//    PlanetModel* mercury;
    
};
#endif // SOLAR_SYSTEM_MODEL_H
 
