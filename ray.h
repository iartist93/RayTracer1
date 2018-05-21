#pragma once
#include "vec3.h"

class Ray
{
public:
    Ray(){}
    Ray(const vec3 &A, const vec3 &B): A(A), B(B) {}

    vec3 Origin() const { return A; }
    vec3 Direction() const {return B; }
    vec3 PointAt(float t) const { return A + t*B; }   // a point along a vector 

private:
    vec3 A;     // start point
    vec3 B;     // ray direction
};
