#pragma once

#include "hitable.h"

class Sphere : public Hitable
{
public:
    Sphere() {}
    Sphere(const vec3& center, float radius, Material* material): center(center), radius(radius), material(material) {}
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;

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
    
    float discriminator = (b*b) - (a*c);

    if(discriminator > 0)
    {
        float t1 = (-b + sqrt(discriminator)) / a;
        float t2 = (-b - sqrt(discriminator)) / a;
        
        if(t2 > t_min && t2 < t_max)
        {
            vec3 hitPoint = ray.PointAt(t2);
            vec3 normal = (hitPoint - center) / radius;     // normalize

            // std::cout << "1 > Normal " << normal.x() << " " << normal.y() << " " << normal.z() << " ";            
            // vec3 r = (hitPoint - center)/radius;
            // std::cout << "1 > /Radius " << r.x() << " " << r.y() << " " << r.z() << std::endl;

            hitResult = HitResult(t2, hitPoint, normal, material);
            return true;
        }
        
        if(t1 > t_min && t1 < t_max)
        {
            vec3 hitPoint = ray.PointAt(t1);
            vec3 normal = (hitPoint - center) / radius;     // normalize
            hitResult = HitResult(t1, hitPoint, normal, material);

            return true;
        }
    }
    return false;
}
