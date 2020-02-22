#ifndef SPACE_SCENE_H
#define SPACE_SCENE_H
#include "Scene.h"
#include "FrameBufferObject.h"


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
//    Camera* GetLightPerspective();
    void adjustFOV(int optionIdx);
    
    void switchPlanetViewPostion();
    
private:
    /**
     FBO Scene object.
     */
    FrameBufferObjectSurface* fbo;

    /**
     Depth texture buffer.
     */
    Texture depthTexture;
    
    /**
     Camera view from light's perspective.
     */
    Camera* lightPerspective;
    
    /**
     Actual camera view.
     */
    Camera* viewersPerspective;
    
    static int planetIndex;
    
    static float inclinationAngles;
    
    static float fov_angles;
    
    static glm::vec3 cameraPosition;
};

#endif //SPACE_SCENE_H
