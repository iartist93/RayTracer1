#pragma once

#include "Surface.h"
#include "ray.h"
#include "texture.h"

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

// refraction equation "Fundemental Book Page 325"
bool Refract(const vec3& V, const vec3& N, float ni_over_nt, vec3& outRefracted)
{
    vec3 uN = unit_vector(N);
    vec3 uV = unit_vector(V);

    float NdotV  = dot(N, uV);
    float disciminator = 1.f - ni_over_nt*ni_over_nt * (1 - NdotV*NdotV);

    if(disciminator <= 0)   // no refraction and all energy is reflected
    {
        return false;
    } else
    {
        outRefracted = ni_over_nt * (uV - N * NdotV) - N * sqrt(disciminator);
        return true;
    }
}

// Suppose that one of the meduims is air N = 1
// "Fundamental page 325"
float SchlickReflectivity(float nt, float cosTheta)
{
    float R0 = (1-nt) / (1+nt);
    R0 = R0*R0;
    float minus_cosTheta = (1-cosTheta);
    float R  = R0 + (1-R0)* minus_cosTheta*minus_cosTheta*minus_cosTheta*minus_cosTheta*minus_cosTheta;
    return R;
}


class Material
{
public:
    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const = 0;
};


// Diffuse-only material
class Lambert : public Material
{
public:
    Lambert() {}
    Lambert(Texture* albedo): albedo(albedo) {}
    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const
    {
        vec3 target = inHitResult.p + inHitResult.n + RandomPointUnitSphere();
        outScatteredRay = Ray(inHitResult.p, target - inHitResult.p);
        outAttentuation = albedo->Value(inHitResult.u, inHitResult.v, inHitResult.p);
        
        // std::cout << "U = " << inHitResult.u  << " V = " << inHitResult.v << std::endl;

        return true;
    }

private:
    Texture* albedo;
};


/// Like Mirrors
class Metal : public Material
{
public:
    Metal() {}
    Metal(const vec3& inAlbedo, float inFuzz): albedo(inAlbedo), fuzz(inFuzz)
    {
        /* /brief! don't let the sphere to be to big, as for the grazing 
           incidence rays it will scatter below the surface */

        if (inFuzz > 1) fuzz = 1;
        else fuzz = inFuzz;
    }

    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const
    {
        vec3 reflected = Reflect(unit_vector(inRay.Direction()), inHitResult.n);
        outScatteredRay = Ray(inHitResult.p, reflected + fuzz * RandomPointUnitSphere());
        outAttentuation = albedo;
        return (dot(outScatteredRay.Direction(), inHitResult.n) > 0);       // if the angel > 90 (don't scatter below the surface)
    }

private:
    vec3 albedo;
    float fuzz;         // radius of the sphere that randomize the reflections
};



/// Like Glass, Water, ..
class Dielectric : public Material
{
public:
    Dielectric() {}
    Dielectric(float nt): nt(nt) {}

    virtual bool Scatter(const Ray& inRay, const HitResult& inHitResult, vec3& outAttentuation, Ray& outScatteredRay) const
    {  
        outAttentuation = vec3(1.f); 
        vec3 reflected = Reflect(inRay.Direction(), inHitResult.n);
        float cosTheta;   

        /// depends on whether the ray in incoming from air to material or outgoing
        vec3 normalVec;
        float ni_over_nt;
        if(dot(inRay.Direction(), inHitResult.n) <= 0)    // incoming ray
        {
            normalVec = inHitResult.n;
            ni_over_nt = 1.f/nt;
            cosTheta = -dot(inRay.Direction(), inHitResult.n) / inRay.Direction().length();
        } else    // outgoing ray (from inside to outside) <-- in case of the previous ray was refracted
        {
            normalVec = -inHitResult.n;
            ni_over_nt = nt/1.f;
            cosTheta = -dot(inRay.Direction(), inHitResult.n) / inRay.Direction().length();

            // as FersnelSchlick fails when ni > n2, so we swap cos(Theta i) with cos(Theta t)
            cosTheta = sqrt(1 - nt*nt*(1 - cosTheta*cosTheta));        // Page 324 .. convert Phi into Theta
        }

        /// Get the probababilty of reflection
        vec3 refracted;
        float R;        // reflectivity proabablity
        if(Refract(inRay.Direction(), normalVec, ni_over_nt, refracted)) 
        {
            R = SchlickReflectivity(nt, cosTheta);
           // std::cout << "Refracted" << std::endl;
        } else
        {
            R = 1.f;        // 100% reflection
        }

        /// if reflectance propability > 0.55 .. or it can be rand() probabiblity
        if(R > rand()%10/10.f)      
        {
            outScatteredRay =  Ray(inHitResult.p, reflected);
           // std::cout << "L" << std::endl;
        } else
        {
            outScatteredRay = Ray(inHitResult.p, refracted);
            // std::cout << "R" << std::endl;
        }
        return true;
    }

private:
    float nt;           // refraction index
};


