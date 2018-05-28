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
        up = vec3(0.f, 1.f, 0.f);     // by default
        origin  = inOrigin;
        forward = unit_vector(origin - lookAt);
        right   = unit_vector(cross(up, forward));              // the order is R, U, F
        up      = unit_vector(cross(forward, right));

        float theta = fov * M_PI / 180.f;
        float halfHeight = tan(theta/2);

        float aspectRatio = inWidth/inHeight;
        float halfWidth  = halfHeight * aspectRatio;

        lower_left  = origin - halfWidth*right - halfHeight*up - 1.f*forward;       // in the direction if -z (-foward = -W)
        width       = 2.f * halfWidth;    
        height      = 2.f * halfHeight;
    }

    Ray RayCast(float x, float y)
    {
        vec3 target = lower_left + (x*width)*right + (y*height)*up;
        return Ray(origin, unit_vector(target - origin));         // (point - orgin) to get the directional vector from the origin to that point
    }

private:
    vec3 origin;
    vec3 lower_left;
    float width;
    float height;
    
    vec3 right;
    vec3 up;
    vec3 forward;
};