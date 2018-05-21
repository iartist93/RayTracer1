#pragma once

#include "hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() : center(vec3(0)), radius(0) {}
    Sphere(const vec3& center, float radius): center(center), radius(radius) {}
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;

private:
    vec3 center;
    float radius;
};


bool Sphere::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    vec3 A = ray.Origin();
    vec3 B = ray.Direction();
    vec3 A_minus_C = A - center;

    // for the discriminator equation
    float a = dot(B, B);
    float b = 2.f * dot(B, A_minus_C);
    float c = dot(A_minus_C, A_minus_C) - (radius * radius);
    
    float discriminator = (b*b) - (4.f*a*c);

    if(discriminator > 0)
    {
        float t1 = (-b + sqrt(discriminator)) / 2.f*a;
        float t2 = (-b - sqrt(discriminator)) / 2.f*a;
        
        if(t2 >= t_min && t2 <= t_max)
        {
            vec3 hitPoint = ray.PointAt(t2);
            vec3 normal = unit_vector(hitPoint - center);     // normalize

            // std::cout << "1 > Normal " << normal.x() << " " << normal.y() << " " << normal.z() << " ";            
            // vec3 r = (hitPoint - center)/radius;
            // std::cout << "1 > /Radius " << r.x() << " " << r.y() << " " << r.z() << std::endl;

            hitResult = HitResult(t2, hitPoint, normal);
            return true;
        }
        
        if(t1 >= t_min && t1 <= t_max)
        {
            vec3 hitPoint = ray.PointAt(t1);
            vec3 normal = unit_vector(hitPoint - center);     // normalize
            hitResult = HitResult(t1, hitPoint, normal);

            return true;
        }
    }
    return false;
}
