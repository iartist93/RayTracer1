#pragma once

#include "hitable.h"

class HitableList : public Hitable
{
public:
    HitableList(){}
    HitableList(Hitable** list, int size): list(list), size(size) {}
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
    int Size() const {return size; }

private:
    Hitable **list;             // array of pointers to Hitable objects
    int size;
};



bool HitableList::Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
{
    HitResult result;
    float closest_so_far = t_max;
    double hitSomething = false;

    for(int i = 0; i < size; i++)
    {
        if(list[i]->Hit(ray, t_min, closest_so_far, result))
        {
            closest_so_far = result.t;
            hitResult = result;
            hitSomething = true;
        }
    }
    return hitSomething;
}
