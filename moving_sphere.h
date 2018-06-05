#pragma once

#include "Surface.h"

class MovingSphere : public Surface
{
public:
    MovingSphere() {}
    MovingSphere(const vec3& center1, const vec3& center2, float time1, float time2, float radius, Material* material): 
                center1(center1), center2(center2), time1(time1), time2(time2), radius(radius), material(material) {}

    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
    void GetTextCoords(const vec3& p, float& u, float& v) const;
    vec3 Center(float inTime) const;

private:
    vec3 center1;
    vec3 center2;
    float time1;
    float time2;
    float radius;
    Material* material;
};

/// Getting the sphere center at "Ray" time .. so the camera shoot a ray at specific apeture time and the ray time is passed to all
vec3 MovingSphere::Center(float inTime) const
{
    return (center1 + ((inTime - time1) * (center2 - center1)) / (time2 - time1));      // lerp time1 -> time2 and center1->center2
}

bool MovingSphere::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 A = ray.Origin();
    vec3 B = ray.Direction();
    vec3 A_minus_C = A - Center(ray.Time());

    // for the discriminator equation
    float a = dot(B, B);
    float b = dot(B, A_minus_C);
    float c = dot(A_minus_C, A_minus_C) - (radius * radius);
    
    float discriminator = (b*b) - (a*c);            // we eliminate 2's from all equations including [B]

    if(discriminator > 0)
    {
        float t1 = (-b + sqrt(discriminator)) / a;
        float t2 = (-b - sqrt(discriminator)) / a;
        
        if(t2 > t_min && t2 < t_max)
        {
            vec3 hitPoint = ray.PointAt(t2);
            vec3 normal = (hitPoint - Center(ray.Time())) / radius;     // normalize
            float u, v;
            GetTextCoords((hitPoint-Center(ray.Time()))/radius, u, v);
            hitResult = HitResult(t2, hitPoint, normal, material, u, v);
            return true;
        }
        
        if(t1 > t_min && t1 < t_max)
        {
            vec3 hitPoint = ray.PointAt(t1);
            vec3 normal = (hitPoint - Center(ray.Time())) / radius;     // normalize

            float u, v;
            GetTextCoords((hitPoint-Center(ray.Time()))/radius, u, v);          // unit vector on unit sphere
            hitResult = HitResult(t1, hitPoint, normal, material, u, v);

            return true;
        }
    }
    return false;
}

/// p is a unit vector on a unit sphere
void MovingSphere::GetTextCoords(const vec3& p, float& u, float& v) const
{
    float phi = atan2(p.z(), p.x());        // z, x  : as Y is the vertical component not the forward
    float theta = asin(p.y());

    u = 1.f - (phi + M_PI) / (2.f*M_PI);        // atan(0, 1) = 0 ==> this is the right-axis so we want to invert this to 1
    v = (theta + M_PI/2.f) / M_PI;
}
