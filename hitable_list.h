#pragma once

#include "surface.h"

class HitableList : public Surface
{
public:
    HitableList(){}
    HitableList(Surface** list, int size): list(list), size(size) {}
    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const;
    int Size() const {return size; }

private:
    Surface **list;             // array of pointers to Surface objects
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
