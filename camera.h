#pragma once

#include "ray.h"
class Camera
{
public:
    /** 
     * width and height are the image's pixel
     * FOV in degree 
     */
    Camera(float inWidth, float inHeight, float fov)
    {
        float theta = fov * M_PI / 180.f;
        float halfHeight = tan(theta/2);

        float aspectRatio = inWidth/inHeight;
        float halfWidth  = halfHeight * aspectRatio;

        origin      = vec3(0.f);
        lower_left  = vec3(-halfWidth, -halfHeight, -1.f);
        width       = vec3(2.f * halfWidth, 0, 0);
        height      = vec3(0, 2.f * halfHeight, 0);
    }

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