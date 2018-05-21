#pragma once

#include "ray.h"
class Camera
{
public:
    Camera() :
        origin(vec3(0.f)), 
        lower_left(vec3(-2.f, -1.f, -1.f)),
        width(vec3(4.0f, 0, 0)), 
        height(vec3(0, 2.f, 0)) {}

    Camera(const vec3& origin, const vec3& lower_left, const vec3& width, const vec3& height):
        origin(origin), lower_left(lower_left), width(width), height(height) {}

    Ray RayCast(float x, float y)
    {
        return Ray(origin, (lower_left + x*width + y*height) - origin);         // (point - orgin) to get the directional vector from the origin to that point
    }

private:
    vec3 origin;
    vec3 lower_left;
    vec3 width;
    vec3 height;
};