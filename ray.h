#pragma once
#include "vec3.h"

class Ray
{
public:
    Ray(){}
    Ray(const vec3 &A, const vec3 &B, float ti): A(A), B(B), _time(ti) {}

    vec3 Origin() const { return A; }
    vec3 Direction() const {return B; }
    vec3 PointAt(float t) const { return A + t*B; }   // a point along a vector 
    float Time() const { return  _time; }
private:
    vec3 A;        // start point
    vec3 B;        // ray direction
    float _time;   // apature time
};
