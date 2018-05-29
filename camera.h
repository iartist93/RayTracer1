#pragma once

#include "ray.h"

/**
 * return a point [-1, 1] on a disk, 
 * simulating a rondom lens ray's origin
 */
vec3 RandomPointOnDisk()
{
    vec3 p;
    do {
        // convert from range [0, 1] to range [-1, 1]
        p = 2.f*vec3(rand()%10/10.f, rand()%10/10.f, 0) - vec3(1.f, 1.f, 0.f);
    } while(dot(p, p) >=1); // x^2 + y^2 < 1 to be inside the circle
    return p;
}

class Camera
{
public:
    /** 
     * width and height are the image's pixel
     * FOV in degree 
     */
    Camera(float inWidth, float inHeight, float fov, vec3 inOrigin, vec3 lookAt, float inAperture, float inFocalLength, float inDistanceFromLens)
    {
        lens_radius = inAperture/2.f;
        aperture = inAperture;
        f = inFocalLength;
        s = inDistanceFromLens;


        up = vec3(0.f, 1.f, 0.f);     // by default
        origin  = inOrigin;
        forward = unit_vector(origin - lookAt);
        right   = unit_vector(cross(up, forward));              // the order is R, U, F
        up      = unit_vector(cross(forward, right));

        float theta = fov * M_PI / 180.f;
        float halfHeight = tan(theta/2);

        float aspectRatio = inWidth/inHeight;
        float halfWidth  = halfHeight * aspectRatio;

        lower_left  = origin -  halfWidth*((f-s)/f)*right - halfHeight*((f-s)/f)*up - s * forward;       // in the direction if -z (-foward = -W)
        width       = 2.f * halfWidth  * ((f-s)/f);    
        height      = 2.f * halfHeight * ((f-s)/f);
    }

    Ray RayCast(float x, float y)
    {
        vec3 lens_rand = lens_radius * RandomPointOnDisk();
        vec3 offset = lens_rand.x() * right + lens_rand.y() * up;
        vec3 target = lower_left + (x*width)*right + (y*height)*up -offset;
        return Ray(origin + offset, unit_vector(target - origin));         // (point - orgin) to get the directional vector from the origin to that point
    }

private:
    vec3 origin;
    vec3 lower_left;

    float width;
    float height;
    
    vec3 right;
    vec3 up;
    vec3 forward;

    float aperture;
    float f;            // focal lens
    float s;            // distance from lens
    float lens_radius;
};