//
//  PlanetViewModel.h
//
//
//  Created by seanren on 1/30/20.
//  Copyright (c) 2020 seanren. All rights reserved.
//

#ifndef PLANET_VIEW_MODEL_H
#define PLANET_VIEW_MODEL_H

#include <iostream>

#include "ModelEx.h"
#include "GLutils.h"
#include "RendererEx.h"

#include "PlanetModel.h"
#include "StbImage.h"

#include "Event.h"

#include <string>

#include <map>
#include <vector>

#include "SolarSystemModel.h"

using namespace std;
using namespace namespaceimage;

//太阳系各天体的数据信息
struct PlanetInfo;

//用于渲染的天体数据
struct PlanetRenderData;

class PlanetViewModel : public Model
{
    
public:
    // Constructor for PlanetViewModel
    PlanetViewModel( Scene* parent, Model* model, ModelType type ,string planetName);

    // Destructor for PlanetViewModel
    virtual ~PlanetViewModel(){
        renderPlanetData.clear();
        planetModels.clear();
        trjacetoryModels.clear();
    }

    // Initialize our Model class
    void InitModel();
    
    // Render the Model class
    void Render();
    
    void LoadPlanet(string planetName);
    
    // Touch Events
	void TouchEventDown( float x, float y );
    
	void TouchEventMove( float x, float y );
	
    void TouchEventRelease( float x, float y );
    
    void AdjustTimeScale( bool x, float y);

    //bool IntersectWithRay(Ray ray0, glm::vec3& intersectionPoint);
    static void adjustTimeScale(bool x,float y);
    static float retrieveTimeScale();
    
    //static vector<PlanetRenderData> retrieveRenderPlanetData();

private:
    char MVP;
    char TEX;
    Image* image;
    Image* starSurfaceImage;
    Material sunMaterial;
    Material planetMaterial;
    glm::vec2 texCoordinates[4];
    glm::vec3 vertices[4];
    char* textureImage;
    
    string planetName;
    
    static vector<PlanetRenderData> renderPlanetData;
    static vector<Model*> planetModels;
    static vector<Model*> trjacetoryModels;
    
    PlanetModel* aPlanet;
    
};
#endif
 
