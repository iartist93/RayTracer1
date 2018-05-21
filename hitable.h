#pragma once

#include "ray.h"
#include "vec3.h"

#define OUT

struct HitResult
{
    HitResult(): t(0), p(vec3(0)), n(vec3(0)) {}
    HitResult(float t, const vec3& p, const vec3& n): t(t), p(p), n(n) {}
    float t;
    vec3  p;
    vec3  n;
};

class Hitable
{
public:
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const = 0; 
};