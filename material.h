#pragma once

#include "hitable.h"
#include "ray.h"

vec3 RandomPointUnitSphere()
{
    vec3 p;
    do {
        p = (2.0f * vec3(rand()%10 / 10.f, rand()%10 / 10.f, rand()%10 / 10.f)) - vec3(1.f);
    } while(p.squared_length() >= 1.f);     // must be <1 to be on sphere

    // std::cout << p.x() << " " << p.y() << " " << p.z() << " | " << p.squared_length() << std::endl;
    return p;
}

vec3 Reflect(const vec3& V, const vec3& N)
{
    return vec3(V - 2*N * dot(N, V));
}

class Material
{
public:
    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const = 0;
};

class Lambert : public Material
{
public:
    Lambert() {}
    Lambert(const vec3& albedo): albedo(albedo) {}
    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const
    {
        vec3 target = inHitResult.p + inHitResult.n + RandomPointUnitSphere();
        outScatteredRay = Ray(inHitResult.p, target - inHitResult.p);
        outAttentuation = albedo;
        return true;
    }

private:
    vec3 albedo;
};


class Metal : public Material
{
public:
    Metal() {}
    Metal(const vec3& albedo): albedo(albedo) {}
    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const
    {
        vec3 reflected = Reflect(unit_vector(inRay.Direction()), inHitResult.n);
        outScatteredRay = Ray(inHitResult.p, reflected);
        outAttentuation = albedo;
        return (dot(outScatteredRay.Direction(), inHitResult.n) > 0);       // if not perpendicular
    }

private:
    vec3 albedo;
};

