//
//  Geometry.h
//  SolarSystem
//
//  Created by SeanRen on 2020/3/17.
//  Copyright © 2020 macbook. All rights reserved.
//

#ifndef Geometry_h
#define Geometry_h

#include "GLutils.h"
#include "WaveFrontOBJ.h"


// Classes inheriting Noncopyable will not have default compiler generated copy
// constructor and assignment operator
class Noncopyable {
protected:
    Noncopyable() {}
    ~Noncopyable() {}
private:
    //参数名后跟着&符号的含义？代表reference调用参数，reference在函数期间不会被改变，故而一般前置const
    Noncopyable(const Noncopyable&);
    //阻止copy的方式是重载赋值操作赋，让其返回reference类型
    const Noncopyable& operator= (const Noncopyable&);
};

// Light wrapper around a GL buffer object handle that automatically allocates
// and deallocates. Can be casted to a GLuint.
class GlBufferObject:Noncopyable{
protected:
    GLuint handle_;
    
public:
    GlBufferObject() {
        glGenBuffers(1, &handle_);
    }
    
    ~GlBufferObject() {
        glDeleteBuffers(1, &handle_);
    }
    
    // Casts to GLuint so can be used directly glBindBuffer and so on
    operator GLuint() const {
        return handle_;
    }
};

// --------- Geometry
// Macro used to obtain relative offset of a field within a struct
#define FIELD_OFFSET(StructType, field) &(((StructType *)0)->field)

struct Geometry {
    GlBufferObject vbo, ibo;
    int vboLen, iboLen;
    vector<vec3> points;
    
    Geometry(){}
    ~Geometry(){}
    
    Geometry(vector<Vertex> vtx, int vboLen) {
        this->vboLen = vboLen;
        //this->iboLen = iboLen;
        
//        // Create the VBO for our obj model vertices.
//        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
//        glBufferData(GL_ARRAY_BUFFER, objMeshModel->vertices.size() * sizeof(objMeshModel->vertices[0]), &objMeshModel->vertices[0], GL_STATIC_DRAW);
        
        // Now create the VBO and IBO
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vboLen, &vtx[0], GL_STATIC_DRAW);
        
        
        for(Vertex v:vtx){
            points.push_back(v.position);
        }
    }
    
    static shared_ptr<Geometry> loadMeshGeometry(string meshFileName){
        shared_ptr<Geometry> geometry;
        // Object for wavefront model parser
        OBJMesh waveFrontObjectModel;
        // Mesh structure object for storing parsed information.
        Mesh* objMeshModel = waveFrontObjectModel.ParseObjModel((char *)meshFileName.c_str());
        //int vertexCount    = waveFrontObjectModel.IndexTotal();
        
        // Create the VBO for our obj model vertices.
        //    glGenBuffers(1, &vertexBuffer);
        //    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        //    glBufferData(GL_ARRAY_BUFFER, objMeshModel->vertices.size() * sizeof(objMeshModel->vertices[0]), &objMeshModel->vertices[0], GL_STATIC_DRAW);
        geometry.reset(new Geometry(objMeshModel->vertices,objMeshModel->vertices.size()));
        
        //clean loaded mesh data
        objMeshModel->vertices.clear();
        
        return geometry;
    }
};


#endif /* Geometry_h */
