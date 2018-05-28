#pragma once

#include "ray.h"
class Camera
{
public:
    /** 
     * width and height are the image's pixel
     * FOV in degree 
     */
    Camera(float inWidth, float inHeight, float fov, vec3 inOrigin, vec3 lookAt)
    {
        vec3 right;   
        vec3 up     = vec3(0, 1, 0);      // world up by default
        vec3 forward;       

        origin  = inOrigin;
        forward = unit_vector(origin - lookAt);
        right   = unit_vector(cross(up, forward));              // the order is R, U, F
        up      = unit_vector(cross(forward, right));

        float theta = fov * M_PI / 180.f;
        float halfHeight = tan(theta/2);

        float aspectRatio = inWidth/inHeight;
        float halfWidth  = halfHeight * aspectRatio;

        lower_left  = origin - halfWidth*right - halfHeight*up - 1.f*forward;       // in the direction if -z (-foward = -W)
        width       = 2.f * halfWidth  * right;     // length + direction
        height      = 2.f * halfHeight * up;
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