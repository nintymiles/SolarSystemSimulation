//
//  RayCaster.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef RayCaster_h
#define RayCaster_h

#include <stdio.h>
#include <glm/glm.hpp>

#include "Ray.h"
#include "ModelEx.h"
#include "Camera.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;

class RayCaster{

public:
    Ray* ray;
    float near;
    float far;
    
    RayCaster(){
        ray=new Ray();
    }
    ~RayCaster();
    
    //从相机视角生成world frame中的screen-ray
    //屏幕点先是被对应为near plane上的点，然后在转换为world coordinate
    //也就是说先应用反转viewport矩阵，然后在应用unporjection矩阵，最后应用eye matrix（inverse view matrix）
    //screen-ray需要标准化，以便于之后的计算
    void setFromCamera(vec3 screenPos,Camera* camera){
        float rayOriginX = (screenPos.x/camera->GetViewPortParam().width) * 2 - 1;
        float rayOriginY = -(screenPos.y/camera->GetViewPortParam().height) * 2 + 1;
        
        if(camera->GetCameraType()==perespective){
            mat4 eyeMat=glm::inverse(camera->GetViewMatrix());
            mat4 projMat=camera->GetProjectionMatrix();
            vec3 camPosition = vec3(eyeMat[3].x,eyeMat[3].y,eyeMat[3].z);
            vec4 screenPosWorld= (eyeMat*glm::inverse(projMat)) * vec4(rayOriginX,rayOriginY,0.0f,1.0f);
            screenPosWorld = screenPosWorld/screenPosWorld.w;
            //screen-ray方向矢量需要标准化
            vec3 rayDi =glm::normalize(vec3(screenPosWorld) -camPosition);
            
            ray = new Ray(camPosition,rayDi);
        }
        
    }
    
    vector<IntersectionData> intersectObject(Model* object){
        if(object->isPickingEnabled()){
            return object->rayCast(this);
        }
        return vector<IntersectionData>();
    }
    
    vector<IntersectionData> intersectObjects(vector<Model *> objects){
        vector<IntersectionData> intersects;
        for(Model *object:objects){
            if(object->isPickingEnabled()){
                vector<IntersectionData> intersectData=object->rayCast(this);
                for(auto data:intersectData)
                    intersects.push_back(data);
            }
        }
        return intersects;
    }
};

#endif /* RayCaster_h */
