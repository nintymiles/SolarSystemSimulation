// OpenGL ES 2.0 Cookcook code

#ifdef __IPHONE_4_0
#include "RendererEx.h"
//#include "Button.h"
#include "SolarSystemModel.h"
#include "SpaceScene.h"
#else
#include "../Scene/RendererEx.h"
#include "../Scene/ObjLoader.h"
#include "../Scene/Button.h"
#include "../Scene/CustomModel.h"
#endif

Renderer*   graphicsEngine;         // Graphics Engine
Scene*      customScene;            // CustomScene object
Light*      globalLight;            // Global Light
Model*      customModel;            // CustomModel object

bool GraphicsInit()
{
    graphicsEngine  = new Renderer();
    customScene     = new SpaceScene("SpaceScene", graphicsEngine);
    
    // Set the lights
    globalLight     = new Light(Material(MaterialWhite), glm::vec4(0.00001, 0.0, 0.0, 1.0));
    customScene->addLight(globalLight);
    
    //场景中仅被添加了一个customModel，但是customModel中包含了其它子model。CustomModel是顶级对象
    // Camera's are added in the CustomScene's constructor
    customModel     = new SolarSystemModel( customScene, NULL, None );
    customScene->addModel(customModel);
    graphicsEngine->initializeScenes();

    return true;
}

bool GraphicsResize( int width, int height )
{
    graphicsEngine->resize(width, height);
    return true;
}

bool GraphicsRender()
{
    //customModel->Rotate(1.0, 0.0, 1.0, 0.0);
    graphicsEngine->render();
    return true;
}

void TouchEventDown( float x, float y )
{
    graphicsEngine->TouchEventDown( x, y );
}

void TouchEventMove( float x, float y )
{
    graphicsEngine->TouchEventMove( x, y );
}

void TouchEventRelease( float x, float y )
{
    graphicsEngine->TouchEventRelease( x, y );
}

void AdjustTimeScale (bool forward, float factor){
    graphicsEngine->AdjustTimeScale(forward, factor);
}

void AdjustFOV(int optionIdx){
    graphicsEngine->AdjustFOV(optionIdx);
}


#ifdef __ANDROID__

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_init( JNIEnv *env, jobject obj, jstring FilePath )
{
    setenv( "FILESYSTEM", env->GetStringUTFChars( FilePath, NULL ), 1 );
    GraphicsInit();
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_resize( JNIEnv *env, jobject obj, jint width, jint height)
{
    GraphicsResize( width, height );
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_step(JNIEnv * env, jobject obj)
{
    GraphicsRender();
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventStart(JNIEnv * env, jobject obj, float x, float y )
{
    TouchEventDown(x ,y);
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventMove(JNIEnv * env, jobject obj, float x, float y )
{
    TouchEventMove(x ,y);
}

JNIEXPORT void JNICALL Java_cookbook_gles_GLESNativeLib_TouchEventRelease(JNIEnv * env, jobject obj, float x, float y )
{
    TouchEventRelease(x ,y);
}

#endif
