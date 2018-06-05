#pragma once

#include "vec3.h"
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

unsigned char* LoadTexture(std::string filename, int& width, int& height, int &noChannels)
{
    return stbi_load(filename.c_str(), &width, &height, &noChannels, 0);
}

class Texture
{
public:
    /**
     * u, v : are the texture coordinates.
     * p    : is a reference to the hitpoint of interest to assign the texture color at u, v to
     */
    virtual vec3 Value(float u, float v,const vec3& p) const = 0;
};


class Color : public Texture
{
public:
    Color(){}
    Color(const vec3& inColor) : color(inColor){}

    virtual vec3 Value(float u, float v,const vec3& p)const
    {
        return color;
    }
private:
    vec3 color;
};

class CheckerTexture : public Texture
{
public:
    CheckerTexture() {}
    CheckerTexture(Color* a, Color* b, int size = 10) : a(a), b(b), size(size){}

    virtual vec3 Value(float u, float v,const vec3& p) const 
    {
        float sines = sin(size*p.x()) * sin(size*p.y()) * sin(size*p.z());   
        return sines > 0 ? a->Value(u, v, p) : b->Value(u, v, p);
    }

private:
    Color* a;
    Color* b;
    int size;
};


class ImageTexture : public Texture
{
public:
    ImageTexture(){}
    ImageTexture(unsigned char* inImageData, int nx, int ny): 
                imageData(inImageData), 
                width(nx), 
                height(ny) {}

    virtual vec3 Value(float u, float v,const vec3& p)const
    {   
        // texture coodinate to pixel coordiante
        int i = u * width;
        int j = (1 - v) * height-0.001f;         // stb_image.h first pixel is top-left so we need to invert v_axis

        // wrapping mode = clip to edges
        if(i < 0) i = 0; 
        if(j < 0) j = 0;
        if(i > width-1)  i = width-1;
        if(j > height-1) j = height-1;

        // sampling
        int offsetY = (3 * width * j);                 // how many rows to skip = 3 components * width pixels * row number
        int offsetX = (3 * i);                         // how many cols to skip = 3 components * current pixel index

        float r = int(imageData[(3 * width * j) + (3 * i) + 0]) / 255.0f;       // 1st component 
        float g = int(imageData[(3 * width * j) + (3 * i) + 1]) / 255.0f;       // 2nd component
        float b = int(imageData[(3 * width * j) + (3 * i) + 2]) / 255.0f;       // 3rd component

        // std::cout << "w " << width << " h " << height << " m " << int(imageData[(3 * width * j) + (3 * i) + 0]) << std::endl;
        // std::cout << r << " " << g << " " << b << std::endl;

        return vec3(r, g, b);
    }

private:
    unsigned char* imageData;
    int width, height;
};