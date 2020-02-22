//
//  EllipseCurve.cpp
//  SolarSystem
//
//  Created by SeanRen on 2020/2/21.
//  Copyright © 2020 macbook. All rights reserved.
//

#include "EllipseCurve.h"


glm::vec4 EllipseCurve::getPoint(float t){
    
    float twoPi = M_PI * 2;
    float deltaAngle = aEndAngle - aStartAngle;
    bool samePoints = ( abs(deltaAngle)<DBL_EPSILON);
    
    // ensures that deltaAngle is 0 .. 2 PI
    while(deltaAngle < 0) deltaAngle += twoPi;
    while(deltaAngle > twoPi) deltaAngle -= twoPi;
    
    if (deltaAngle < DBL_EPSILON) {
        if(samePoints) {
            deltaAngle = 0;
        }else {
            deltaAngle = twoPi;
        }
        
    }
    
    if (aClockwise == true && !samePoints) {
        if ( deltaAngle == twoPi ) {
            deltaAngle = - twoPi;
        } else {
            deltaAngle = deltaAngle - twoPi;
        }
        
    }
    
    float angle = aStartAngle + t * deltaAngle;
    float x = aX + xRadius * cos(angle);
    float y = aY + yRadius * sin(angle);
    
    if (aRotation != 0) {
        float cosV = cos(aRotation);
        float sinV = sin(aRotation);
        
        float tx = x - aX;
        float ty = y - aY;
        
        // Rotate the point about the center of the ellipse.
        x = tx * cosV - ty * sinV + aX;
        y = tx * sinV + ty * cosV + aY;
        
    }
    
    return glm::vec4(x,y,0,0);

}
