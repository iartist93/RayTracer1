#pragma once

#include "surface.h"
#include "hitable_list.h"
#include "rectangle.h"
#include "flip_normals.h"

class Box : public Surface
{
public:
    Box(){}
    Box(const vec3& p1, const vec3& p2, Material* mat);
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;

private:
    vec3 pMin, pMax;
    Material* mat;
    HitableList* surfaceListPtr;
};


Box::Box(const vec3& p1, const vec3& p2, Material* mat)
{  
    Surface** list = new Surface*[6];

    pMin = p1;
    pMax = p2;

    list[0] = new FlipNormals(new RectangleXY(p1.x(), p2.x(), p1.y(), p2.y(), p1.z(), mat));
    list[1] = (new RectangleXY(p1.x(), p2.x(), p1.y(), p2.y(), p2.z(), mat));

    list[2] = (new RectangleXZ(p1.x(), p2.x(), p1.z(), p2.z(), p2.y(), mat));
    list[3] = new FlipNormals(new RectangleXZ(p1.x(), p2.x(), p1.z(), p2.z(), p1.y(), mat));
   
    list[4] = new FlipNormals(new RectangleYZ(p1.y(), p2.y(), p1.z(), p2.z(), p1.x(), mat));
    list[5] = new RectangleYZ(p1.y(), p2.y(), p1.z(), p2.z(), p2.x(), mat);
    
    surfaceListPtr = new HitableList(list, 6);
}

bool Box::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    return surfaceListPtr->Hit(ray, t_min, t_max, hitResult);   // loop and return the min hit if found
}

