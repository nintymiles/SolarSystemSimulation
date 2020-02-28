//
//  Box.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/26.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Box_h
#define Box_h

#include <stdio.h>
#include <cmath>
#include <limits>

#include <glm/glm.hpp>
#include <vector>

using glm::vec3;
using std::vector;

class Box {
    
public:
    vec3 min;
    vec3 max;
    
    Box(){}
    ~Box();
    
    void makeEmpty(){
        double infinity= std::numeric_limits<double>::infinity();
        min.x=min.y=min.z=-infinity; //-INFINITY; //in math.h
        max.x=max.y=max.z=infinity; //INFINITY; //in math.h
    }
    
    bool isEmpty(){
        return ( max.x < min.x ) || ( max.y < min.y ) || ( max.z < min.z );
    }
    
    void setFromPoints(vector<vec3> points){
        for(vec3 point:points){
            expandByPoint(point);
        }
    }
    
    void expandByPoint(vec3 point) {
        min=glm::min(point,min);
        max=glm::max(point,max);
    }
    
    //获取的aabb box中心坐标的算法
    vec3 getCenter(){
        //如果box未初始化，返回vec3(0,0,0);
        if(isEmpty()) return vec3(0,0,0);
        return (min+max) * 0.5f;
    }


};

#endif /* Box_h */
