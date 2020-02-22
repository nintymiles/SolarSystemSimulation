//
//  EllipseCurve.hpp
//  SolarSystem
//
//  Created by SeanRen on 2020/2/21.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef EllipseCurve_hpp
#define EllipseCurve_hpp

#include <stdio.h>
#include "Curve.h"

//派生类如果想要访问基类的成员，则要将继承关系设置为public
class EllipseCurve:public Curve {
    string type;
    float aX;
    float aY;
    
    float xRadius;
    float yRadius;
    
    float aStartAngle;
    float aEndAngle;
    
    bool aClockwise;
    float aRotation;
    
//    this.type = 'EllipseCurve';
//
//    this.aX = aX || 0;
//    this.aY = aY || 0;
//
//    this.xRadius = xRadius || 1;
//    this.yRadius = yRadius || 1;
//
//    this.aStartAngle = aStartAngle || 0;
//    this.aEndAngle = aEndAngle || 2 * Math.PI;
//
//    this.aClockwise = aClockwise || false;
//
//    this.aRotation = aRotation || 0;
    
public:
    EllipseCurve(string type="EllipseCurve",float aX=0,float aY=0,float xRadius=0,float yRadius=0,float aStartAngle=0,float aEndAngle=2*M_PI,bool aClockwise=false,float aRotation=0):type(type),aX(aX),aY(aY),xRadius(xRadius),yRadius(yRadius),aStartAngle(aStartAngle),aEndAngle(aEndAngle),aClockwise(aClockwise),aRotation(aRotation){}
    
    glm::vec4 getPoint(float u);
};

#endif /* EllipseCurve_hpp */
