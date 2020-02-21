#include "RendererEx.h"

#include <typeinfo>
#include <iostream>

#include "SpaceScene.h"

//! Renderer Constructor
Renderer::Renderer()
{
}

//! Renderer Destructor
Renderer::~Renderer()
{
    
}

//! Initialize Scene
void Renderer::initializeScenes()
{
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->initializeScene();
    }
}

//渲染器调整窗口尺寸，实际实现是调整每个scene的大小
//! resize screen
void Renderer::resize( int w, int h )
{
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->resize(w, h);
    }
}

//! Cache the scene
void Renderer::addScene( Scene* scene)
{
    if(!scene)
        return;
    
    for (std::vector<Scene*>::iterator it = scenes.begin() ; it != scenes.end(); ++it){
        if( (*it) == scene ){
            //If already added return;
            return;
        }
    }

    scenes.push_back( scene );
    scene->setRenderer(this);
}

bool Renderer::removeScene(Scene* scene)
{
    int i = 0;
    for (std::vector<Scene*>::iterator it = scenes.begin(); it != scenes.end(); it++,i++)
    {
        if( (*it) == scene){
            scenes.erase(scenes.begin()+i);
            return true;
        }
    }
    return false;
}

Scene* Renderer::getScene(char* name)
{
    for (std::vector<Scene*>::iterator it = scenes.begin() ; it != scenes.end(); ++it){
        if( (*it)->GetName().compare(name) == 0 ){
            return (*it);
        }
    }
    return NULL;
}

//渲染过程实际就是每个场景的渲染
//! Render the Models
void Renderer::render()
{
//    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->render();
    }
}

//每个事件其实被传递到scene中，由scene响应。
//! Gesture Press Event
void Renderer::TouchEventDown( float x, float y )
{
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->TouchEventDown(x, y);
    }
}

//! Gesture Move Event
void Renderer::TouchEventMove( float x, float y )
{
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->TouchEventMove(x, y);
    }
}

//! Gesture Release Event
void Renderer::TouchEventRelease( float x, float y )
{
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->TouchEventRelease(x, y);
    }
}

void Renderer::AdjustTimeScale( bool x, float y ){
    for( int i=0; i<scenes.size();  i++ ){
        scenes.at(i)->AdjustTimeScale(x, y);
    }
}

void Renderer::AdjustFOV(int optionIdx){
    
    for( int i=0; i<scenes.size();  i++ ){
        Scene *scene = scenes.at(i);
//        const std::type_info &ti = typeid(scene);
//        const std::type_info &ti2 = typeid(CustomScene);
//        std::cout << "object scene.name = " << ti.name() <<"customescene.type = " <<ti2.name() << std::endl;
        scene->adjustFOV(optionIdx);
        //scene->switchPlanetViewPostion();

    }
    
}

