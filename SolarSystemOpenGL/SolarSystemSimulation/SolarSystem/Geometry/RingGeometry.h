//
//  RingGeometry.hpp
//  SolarSystem
//
//  Created by SeanRen on 2020/2/24.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef RingGeometry_H
#define RingGeometry_H

#include <stdio.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>

#include "GLutils.h"

using namespace std;
using namespace glm;

struct RingVertex{
    vec4 vertex;
    vec3 normal;
    vec2 uv;
};

struct RingGeometryData {
    vector<GLuint> indices;
    vector<RingVertex> vertices;

    RingGeometryData(vector<GLuint> indices,vector<RingVertex> vertices):indices(indices),vertices(vertices){}
};

class RingGeometry {
    float innerRadius;
    float outerRadius;
    int thetaSegments; //经线切片数目
    int phiSegments; //纬线切片数目
    float thetaStart; //经线起始点角度
    float thetaLength; //经线结束点角度
    
public:
    RingGeometry(float innerRadius=0.5f,float outerRaidus=1.0f,int thetaSegments=100,int phiSegments=20,float thetaStart=0.0f,float thetaLength=2*M_PI)
    :innerRadius(innerRadius),outerRadius(outerRaidus),thetaSegments(fmax(3,thetaSegments)),phiSegments(fmax(1,phiSegments)),thetaStart(thetaStart),thetaLength(thetaLength){}
    
    ~RingGeometry(){};
    
    RingGeometryData createRingGeometry(){
        
        vector<GLuint> indices;
        vector<RingVertex> vertices;
        
        // some helper variables
        float segment;
        float radius = innerRadius;
        float radiusStep = ( ( outerRadius - innerRadius ) / phiSegments );
        vec4 vertex;
        vec2 uv;
        
        int j, i;
        
        // generate vertices, normals and uvs
        for ( j = 0; j <= phiSegments; j ++ ) {
            
            for ( i = 0; i <= thetaSegments; i ++ ) {
                RingVertex ringVertex;
                
                // values are generate from the inside of the ring to the outside
                // 经线弧上每一段的角度
                segment = thetaStart + i / (float)thetaSegments * thetaLength;
                
                // vertex
                //内圈每一个段结尾处点的坐标
                vertex.x = radius * std::cos(segment);
                vertex.y = radius * std::sin(segment);
                
                //将内圈顶点依次放入缓存中
                vertex.z = 0.0f;
                vertex.w = 1.0f;
                ringVertex.vertex=vertex;
                
                // normal
                //内圈顶点的法线，因为是平面，所以默认+z轴
                ringVertex.normal=vec3(0, 0, 1);
                
                // uv
                //内圈的uv坐标
                //如innerRadius为0.5-1，则x和y的范围为[0.5..0.75]-[0.75..1]
                uv.x = ( vertex.x / outerRadius + 1 ) / 2;
                uv.y = ( vertex.y / outerRadius + 1 ) / 2;
//                uv.x = radius/outerRadius;
//                uv.y = (vertex.y - radius)/0.5;
                
                ringVertex.uv=uv;
                
                vertices.push_back(ringVertex);
            }
            
            // increase the radius for next row of vertices
            
            radius += radiusStep;
            
        }
        
        // indices
        
        for ( j = 0; j < phiSegments; j ++ ) {
            
            int thetaSegmentLevel = j * ( thetaSegments + 1 );
            
            for ( i = 0; i < thetaSegments; i ++ ) {
                
                segment = i + thetaSegmentLevel;
                
                int a = segment;
                int b = segment + thetaSegments + 1;
                int c = segment + thetaSegments + 2;
                int d = segment + 1;
                
                // faces
                indices.push_back(a);
                indices.push_back(b);
                indices.push_back(d);
                
                indices.push_back(b);
                indices.push_back(c);
                indices.push_back(d);
                
            }
            
        }
    
        return RingGeometryData(indices,vertices);
    }
    
};


#endif /* RingGeometry_H */
