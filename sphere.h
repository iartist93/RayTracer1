#pragma once

#include "Surface.h"

class Sphere : public Surface
{
public:
    Sphere() {}
    Sphere(const vec3& center, float radius, Material* material): center(center), radius(radius), material(material) {}
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
    void GetTextCoords(const vec3& p, float& u, float& v) const;

private:
    vec3 center;
    float radius;
    Material* material;
};


bool Sphere::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 A = ray.Origin();
    vec3 B = ray.Direction();
    vec3 A_minus_C = A - center;

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
            vec3 normal = (hitPoint - center) / radius;     // normalize
            float u, v;
            GetTextCoords((hitPoint-center)/radius, u, v);
            hitResult = HitResult(t2, hitPoint, normal, material, u, v);
            return true;
        }
        
        if(t1 > t_min && t1 < t_max)
        {
            vec3 hitPoint = ray.PointAt(t1);
            vec3 normal = (hitPoint - center) / radius;     // normalize

            float u, v;
            GetTextCoords((hitPoint-center)/radius, u, v);          // unit vector on unit sphere
            hitResult = HitResult(t1, hitPoint, normal, material, u, v);

            return true;
        }
    }
    return false;
}

/// p is a unit vector on a unit sphere
void Sphere::GetTextCoords(const vec3& p, float& u, float& v) const
{
    float phi = atan2(p.z(), p.x());        // z, x  : as Y is the vertical component not the forward
    float theta = asin(p.y());

    u = 1.f - (phi + M_PI) / (2.f*M_PI);        // atan(0, 1) = 0 ==> this is the right-axis so we want to invert this to 1
    v = (theta + M_PI/2.f) / M_PI;
}
