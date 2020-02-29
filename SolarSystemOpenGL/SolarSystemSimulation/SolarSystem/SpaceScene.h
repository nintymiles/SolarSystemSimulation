#ifndef SPACE_SCENE_H
#define SPACE_SCENE_H
#include "Scene.h"


class SpaceScene : public Scene
{
    
public:
    
    //! Default Constructor
    SpaceScene(std::string name = "", Object* parentObj = NULL);
    
    //! Destructor
    virtual ~SpaceScene(void);
    //! Initialize Scene
    void initializeScene();

    void render();
    void resize( int w, int h);
    
    void adjustFOV(int optionIdx);
    
    void switchPlanetViewPostion();
    
    void TouchEventDown( float a, float b );
    
    void switchPlanetView(string planetName);
    void switchSolarSystemView();
    
private:
    
    /**
     Actual camera view.
     */
    Camera* viewersPerspective;
    Camera* planerViewPerspective;
    
    static int planetIndex;
    
    static float inclinationAngles;
    
    static float fov_angles;
    
    static glm::vec3 cameraPosition;
    
    Light* globalLight; // Global Light
    
    Model* solarSystemModel;
    Model* planetViewModel;
    bool planetViewFlag;
};

#endif //SPACE_SCENE_H
