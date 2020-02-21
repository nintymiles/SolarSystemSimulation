#ifndef OBJECT_H
#define OBJECT_H

#include "constant.h"
#include <string>
#include <vector>
#include <typeinfo>

//3D场景中基本物体对象需要具备层次体系能力
//实际框架中，基础的render也需要层次体系能力，可以将scene加入其中？
// Parminder: Interface for the rendering models.
class Object
{
public:
    Object(std::string name = "", Object* parentObj = NULL);
    
    virtual ~Object();

    void SetName(std::string mdlName){ name = mdlName;}
    
    std::string GetName(){return name;}

    void SetParent(Object* parent = 0);
    
    void RemoveParent();
    
    void SetChild(Object* child = 0);
    
    void RemoveFromParentChildList();
    
    Object*  GetParent();
    
    std::vector<Object*>* GetChildren();

protected:

    /*!
     Model's name
     */
    std::string name;

    /*!
     Model's parent
     */
    Object* parent;
    
    /*!
     Model's child list
     */
    std::vector<Object*> childList;
};

#endif // OBJECT_H
