#pragma once

#include "surface.h"

class RectangleXY : public Surface
{
public:

    RectangleXY() {}
    RectangleXY(float x0, float x1, float y0, float y1, float k, Material* mat): 
            k(k), x0(x0), x1(x1), y0(y0), y1(y1), mat(mat) {}

    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
private:
    float k;        // z = k : the plane that the rectangle is part of
    float x0, x1, y0, y1;
    Material* mat;
};

// 1- check the ray hit the plane z = k
// 2- check if the hit result (x, y) are inside the rectangle
bool RectangleXY::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 origin = ray.Origin();
    vec3 dir = ray.Direction();
    
    float t = (k - origin.z()) / dir.z();

    if(t < t_min || t > t_max)
        return false;
    
    float x = origin.x() + t * dir.x();
    float y = origin.y() + t * dir.y();

    if(x > x0 && x < x1 && y > y0 && y < y1)
    {
        hitResult.p = ray.PointAt(t);
        hitResult.matPtr = mat;
        hitResult.t = t;
        hitResult.n = vec3(0, 0, 1.f);
        hitResult.u = (x - x0) / (x1 - x0);
        hitResult.v = (y - y0) / (y1 - y0);
        return true;
    }
    else
        return false;
}



//================================================================//

class RectangleXZ : public Surface
{
public:

    RectangleXZ() {}
    RectangleXZ(float x0, float x1, float z0, float z1, float k, Material* mat): 
            k(k), x0(x0), x1(x1), z0(z0), z1(z1), mat(mat) {}

    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
private:
    float k;        // y = k : the plane that the rectangle is part of
    float x0, x1, z0, z1;
    Material* mat;
};

bool RectangleXZ::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 origin = ray.Origin();
    vec3 dir = ray.Direction();
    
    float t = (k - origin.y()) / dir.y();

    if(t < t_min || t > t_max) return false;
    
    float x = origin.x() + t * dir.x();
    float z = origin.z() + t * dir.z();

    if(x < x0 || x > x1 || z < z0 || z > z1)
        return false;

    hitResult.p = ray.PointAt(t);
    hitResult.matPtr = mat;
    hitResult.t = t;
    hitResult.n = vec3(0, 1.f, 0);
    hitResult.u = (x - x0) / (x1 - x0);
    hitResult.v = (z - z0) / (z1 - z0);
    return true;
}

//================================================================//



class RectangleYZ : public Surface
{
public:

    RectangleYZ() {}
    RectangleYZ(float y0, float y1, float z0, float z1, float k, Material* mat): 
            k(k), y0(y0), y1(y1), z0(z0), z1(z1), mat(mat) {}

    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
private:
    float k;        // x = k : the plane that the rectangle is part of
    float y0, y1, z0, z1;
    Material* mat;
};

bool RectangleYZ::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 origin = ray.Origin();
    vec3 dir = ray.Direction();
    
    float t = (k - origin.x()) / dir.x();

    if(t < t_min || t > t_max) return false;
    
    float y = origin.y() + t * dir.y();
    float z = origin.z() + t * dir.z();

    if(y < y0 || y > y1 || z < z0 || z > z1)
        return false;

    hitResult.p = ray.PointAt(t);
    hitResult.matPtr = mat;
    hitResult.t = t;
    hitResult.n = vec3(1.f, 0, 0);
    hitResult.u = (y - y0) / (y1 - y0);
    hitResult.v = (z - z0) / (z1 - z0);
    return true;
}