#pragma once

#include "ray.h"
#include "vec3.h"

#define OUT
class Material;

struct HitResult
{
    HitResult(): t(0), p(vec3(0)), n(vec3(0)) {}
    HitResult(float t, const vec3& p, const vec3& n, Material* matPtr): t(t), p(p), n(n), matPtr(matPtr) {}
    HitResult(float t, const vec3& p, const vec3& n, Material* matPtr, float u, float v): 
                t(t), p(p), n(n), matPtr(matPtr), u(u), v(v) {}

    float t;
    vec3  p;
    vec3  n;
    Material* matPtr;       // to the Surface material
    float u;
    float v;
};

class Surface
{
public:
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const = 0;
};