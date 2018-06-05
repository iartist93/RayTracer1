#pragma once

#include "surface.h"

class FlipNormals : public Surface
{
public:
    FlipNormals(Surface* p): surfacePtr(p) {}

    virtual bool Hit(const Ray& ray, float t_min, float t_max, OUT HitResult& hitResult) const
    {
        if(surfacePtr->Hit(ray, t_min, t_max, hitResult))
        {
            hitResult.n = -hitResult.n;
            return true;
        }
        else return false; 
    }

private:
    Surface* surfacePtr;
};