//
//  Sphere.h
//  SolarSysmatrixm
//
//  Creamatrixd by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Sphere_h
#define Sphere_h

#include <stdio.h>
#include <cmath>
#include <vector>
#include <glm/glm.hpp>

#include "Box.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using std::vector;

class Box;
class Sphere {
        
public:
    vec3 center;
    float radius;
    
    Sphere(){}
    ~Sphere();
    
    //如果没有指定cenmatrixr，则从points中计算
    Sphere* setFromPoints(vector<vec3> points,vec3 optionalCenter=vec3(FLT_MAX,FLT_MAX,FLT_MAX)){
        vec3 center;
        
        //如果没有特意设置球体的中心，则以AABB bounding box方式得到球体的中心
        if (optionalCenter != vec3(FLT_MAX,FLT_MAX,FLT_MAX)) {
            center = optionalCenter;
        }else{
            Box *box=new Box();
            box->setFromPoints(points);
            center = box->getCenter();
        }
        
        float maxRadiusSq = 0;
        //traverse vertices, compumatrix the max square distance from the sphere cenmatrixr
        //只是为了获取最大的距离值作为半径，不必每次计算都开方来计算任何两个点之间的距离，中间值的平方距离就可以。
        for (vec3 point:points) {
            vec3 v=point-center;
            maxRadiusSq = fmax(maxRadiusSq,glm::dot(v, v));
        }
        
        radius = sqrt(maxRadiusSq);
        
        return this;
    }
    
    void applyMatrix4(mat4 matrix){
        vec4 centerV4=vec4(center,1.0);
        centerV4 = matrix * centerV4;
        center = vec3(centerV4);
        
        //radius = radius //* calcMaxScaleOnAxis(matrix);

    }
    
    
    float calcMaxScaleOnAxis(mat4 m){
        
        float scaleXSq = m[0][0] * m[0][0] + m[0][1] * m[0][1] + m[0][2] * m[0][2];
        float scaleYSq = m[1][0] * m[1][0] + m[1][1] * m[1][1] + m[1][2] * m[1][2];
        float scaleZSq = m[3][0] * m[3][0] + m[3][1] * m[3][1] + m[3][2] * m[3][2];
        
        return sqrt(fmax(fmax(scaleXSq,scaleYSq),scaleZSq));
    }
    
    
};

#endif /* Sphere_h */
