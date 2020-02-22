//
//  Curve.h
//  SolarSystem
//
//  Created by SeanRen on 2020/2/21.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Curve_h
#define Curve_h

#include <stdio.h>
#include <string>
#include <vector>
#include <cmath>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>



using namespace std;

struct CurveGeometryData{
    vector<glm::vec4> tangents;
    vector<glm::vec4> normals;
    vector<glm::vec4> binormals;
    CurveGeometryData(vector<glm::vec4> tangents,vector<glm::vec4> normals,vector<glm::vec4> binormals):tangents(tangents),normals(normals),binormals(binormals){}
};

class Curve {
    string type;
    int arcLengthDivisions;
    
    vector<float> cacheArcLengths;
    bool needsUpdate;
    
public:
    Curve(string type="Curve",int arcLengthDivisions=200):type(type),arcLengthDivisions(arcLengthDivisions){}
    
    ~Curve(){}
    
    virtual glm::vec4 getPoint(float u)=0;
    
    // Get point at relative position in curve according to arc length
    // - u [0 .. 1]
    glm::vec4 getPointAt(float u){
        return getPoint(u);
    }
    
    // Get sequence of points using getPoint( t )
    //将所有弧长上的点放入points中
    virtual vector<glm::vec4> getPoints(int divisions){
        if(divisions==0)
            divisions =5;
        
        vector<glm::vec4>  points;
        for(int d=0;d<=divisions;d++){
            points.push_back(getPoint((float)d/divisions));
        }
        
        return points;
    }
    
    // Get sequence of points using getPointAt( u )
    // 获取所有弧分割处的点
    vector<glm::vec4> getSpacedPoints(int divisions){
        if(divisions==0)
            divisions =5;
        
        vector<glm::vec4>  points;
        for(int d=0;d<=divisions;d++){
            points.push_back(getPointAt(d/divisions));
        }
        
        return points;
    
    }
    
    // Get total curve arc length
    int getLength(){
        vector<float> lengths = getLengths(arcLengthDivisions);
        return lengths[ lengths.size() - 1 ];
    }
    
    // Get list of cumulative segment lengths
    //获得弧上一次逐渐累加弧长线段的长度列表
    vector<float> getLengths(int divisions){
        if(divisions==0)
            divisions = arcLengthDivisions;
        
        if ( cacheArcLengths.size()>0 &&
            ( (int)cacheArcLengths.size() == divisions + 1 ) &&
            !needsUpdate ) {
            return cacheArcLengths;
        }
        
        this->needsUpdate = false;
        
        vector<float> cache;
        glm::vec4 current, last = getPoint( 0 );
        int p;
        float sum = 0.0;
        
        cache.push_back( 0 );
        
        //累加计算每段弧长
        for ( p = 1; p <= divisions; p ++ ) {
            
            current = getPoint( p / divisions );
            //sum为逐渐累加的弧长
            sum += glm::distance(current, last);
            cache.push_back( sum );
            last = current;
        }
        
        cacheArcLengths = cache;
        
        return cache; // { sums: cache, sum: sum }; Sum is in the last element.
    }
    
    void updateArcLengths(){
        needsUpdate = true;
        getLengths(arcLengthDivisions);
    }

    // Given u ( 0 .. 1 ), get a t to find p. This gives you points which are equidistant
    // 给出从0到弧长分割的部分的中点在弧长中的比例？？？
    float getUtoTmapping(float u,float distance){
        
        vector<float> arcLengths = getLengths(arcLengthDivisions);
        
        int i = 0, il = (int)arcLengths.size();
        
        float targetArcLength; // The targeted u distance value to get
        
        if ( distance>0 ) {
            
            targetArcLength = distance;
            
        } else {
            
            //没有给出距离，就用u*总弧长算出当前弧起点的弧长
            targetArcLength = u * arcLengths[ il - 1 ];
            
        }
        
        // binary search for the index with largest value smaller than target u distance
        int low = 0, high = il - 1, comparison;
        
        while ( low <= high ) {
            
            //获取两个index的中间index
            i = floor( low + ( high - low ) / 2 ); // less likely to overflow, though probably not issue here, JS doesn't really have integers, all numbers are floats
            
            //不明白为何要用第i段的弧长减去目标弧长？
            comparison = arcLengths[ i ] - targetArcLength;
            
            if ( comparison < 0 ) {
                
                low = i + 1;
                
            } else if ( comparison > 0 ) {
                
                high = i - 1;
                
            } else {
                high = i;
                break;
                
                // DONE
            }
            
        }
        
        i = high;
        
        //这个表达我的理解，此处的arclength和整个弧长相同
        if ( arcLengths[ i ] == targetArcLength ) {
            
            return i / ( il - 1 );

        }
        
        // we could get finer grain at lengths, or use simple interpolation between two points
        
        float lengthBefore = arcLengths[ i ];
        float lengthAfter = arcLengths[ i + 1 ];
        
        float segmentLength = lengthAfter - lengthBefore;
        
        // determine where we are between the 'before' and 'after' points
        float segmentFraction = ( targetArcLength - lengthBefore ) / segmentLength;
        
        // add that fractional amount to t
        float t = ( i + segmentFraction ) / ( il - 1 );
        
        return t;
    }
    
    // Returns a unit vector tangent at t
    // In case any sub curve does not implement its tangent derivation,
    // 2 points a small delta apart will be used to find its gradient
    // which seems to give a reasonable approximation
    glm::vec4 getTangent(float t){
        float delta = 0.0001;
        float t1 = t - delta;
        float t2 = t + delta;
        
        // Capping in case of danger
        // 获得弧长某比例处两个很近的点，用以构成切线
        if ( t1 < 0 ) t1 = 0;
        if ( t2 > 1 ) t2 = 1;
        
        glm::vec4 pt1 = getPoint( t1 );
        glm::vec4 pt2 = getPoint( t2 );
        
        //注意这句表达，产生新的数据结构，用以表达切线矢量
        glm::vec4 vec = pt2 - pt1;
        return glm::normalize(vec);
    }
    
    glm::vec4 getTangentAt(float u){
        float t = getUtoTmapping(u, 0);
        return getTangent(t);
    }

    CurveGeometryData computeFrenetFrames(int segments,bool closed=false){
        // see http://www.cs.indiana.edu/pub/techreports/TR425.pdf
        
        glm::vec3 normal;
        
        vector<glm::vec4> tangents(segments);
        vector<glm::vec4> normals(segments);
        vector<glm::vec4> binormals(segments);
        
        glm::vec3 vec;
        glm::mat3 mat;
        
        int i;
        float u, theta;
        
        // compute the tangent vectors for each segment on the curve
        for ( i = 0; i <= segments; i ++ ) {
            
            u = i / segments;
            
            glm::vec3 tangent = glm::vec3(getTangentAt(u));
            tangents.push_back(glm::vec4(glm::normalize(tangent),0));
        }
        
        // select an initial normal vector perpendicular to the first tangent vector,
        // and in the direction of the minimum tangent xyz component
        normals.push_back(glm::vec4());
        binormals.push_back(glm::vec4());
        float min = MAXFLOAT;
        float tx = abs( tangents[ 0 ].x );
        float ty = abs( tangents[ 0 ].y );
        float tz = abs( tangents[ 0 ].z );
        
        //哪个轴上的坐标值接近于0，则发现接近于这个轴
        if ( tx <= min ) {
            
            min = tx;
            normal=glm::vec3( 1, 0, 0);
            
        }
        
        if ( ty <= min ) {
            
            min = ty;
            normal=glm::vec3( 0, 1, 0);
            
        }
        
        if ( tz <= min ) {
            
            normal=glm::vec3( 0, 0, 1);
            
        }
        
        vec = glm::normalize(glm::cross(glm::vec3(tangents[0]),normal));
        
        normals.push_back(glm::vec4(glm::cross(glm::vec3(tangents[0]), vec),0));
        binormals.push_back(glm::vec4(glm::cross(glm::vec3(tangents[0]), glm::vec3(normals[0])),0));
        
        
        // compute the slowly-varying normal and binormal vectors for each segment on the curve
        for ( i = 1; i <= segments; i ++ ) {
            
            normals[i] = normals[i - 1];
            
            binormals[i] = binormals[i - 1];
            
            vec = glm::cross(glm::vec3(tangents[i - 1]), glm::vec3(tangents[i]));
            
            //如果两个相邻的切线无限重合，则向量集计算出的正交矢量无限小，小于最小精度，则无变化
            if ( vec.length() > DBL_EPSILON ) {
                
                vec=glm::normalize(vec);
                
                //相对来说,3D图形线性代数计算的细节较多，相关的函数都要理解
                float dotVal = glm::dot(tangents[i - 1],tangents[i]);
                float clampVal= glm::clamp(dotVal, -1.0f, 1.0f); //clamp函数的使用最让人头疼，由于其支持众多的格式，所以其对const literal的要求及其明确
                theta = acos(clampVal); // clamp for floating pt errors
                
                glm::mat4 rotMat= glm::rotate(glm::mat4(), theta, vec);
                
                normals[i] = rotMat * glm::vec4(vec,0);
                
            }
            
            
            binormals[i]=glm::vec4(glm::cross(glm::vec3(tangents[i]), glm::vec3(normals[i])),0);
            
        }
        
        // if the curve is closed, postprocess the vectors so the first and last normal vectors are the same
        if ( closed == true ) {
            
            theta = acos(glm::clamp(glm::dot(normals[0],normals[segments]), -1.0f, 1.0f));
            
            theta /= segments;
            
            if (glm::dot(glm::vec3(tangents[0]),glm::cross(glm::vec3(normals[0]),glm::vec3(normals[segments])))>0){
                theta = - theta;
            }
            
            for ( i = 1; i <= segments; i ++ ) {
                
                // twist a little...
                glm::mat4 rotMat = glm::rotate(glm::mat4(),theta*i,glm::vec3(tangents[i]));
                normals[i] = rotMat * normals[i];
                binormals[i] = glm::vec4(glm::cross(glm::vec3(tangents[i]),glm::vec3(normals[i])),0);
                
            }
            
        }
        
        CurveGeometryData curveData(tangents,normals,binormals);
        
        return curveData;
        
//        return {
//        tangents: tangents,
//        normals: normals,
//        binormals: binormals
//        };
    }

};


#endif /* Curve_h */
