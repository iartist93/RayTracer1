#include <iostream>
#include <fstream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include <vector>
#include <string>

#define OUT 

void ReversePPM(std::string input, std::string output, int width, int height)
{
        
    std::ifstream in_image(input, std::ifstream::in);
    std::vector<std::string> reversedLines;
    std::string line;
    for(int h = 0; h < height; h++)
    {
        std::getline(in_image, line);
        reversedLines.push_back(line);
    }
    in_image.close();

    std::ofstream out_image;
    out_image.open(output);
    out_image << "P3\n" << width << " " << height << "\n255\n";

    for(int h = 0; h < height; h++)
    {
        out_image << reversedLines.back() << std::endl;
        reversedLines.pop_back(); 
    }

    out_image.close();
}

vec3 Color(const Ray & ray, Hitable* hitable_list)
{
    HitResult hitResult;

    if(hitable_list->Hit(ray, 0, 100000000, hitResult))
    {
        vec3 normalColor = 0.5f * (hitResult.n + vec3(1.f, 1.0f, 1.0f));  // convert the noraml[-1, 1] to [0, 1] to output RGB color
        return normalColor;
    }
   /// else
    {
        vec3 ray_dir = unit_vector(ray.Direction());
        float t = 0.5 * (ray_dir.y() + 1.0);            // convert from [-1, 1] to [0, 1]
        return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
    std::ofstream image;
    image.open("output.ppm");
    const int width = 200, height = 100;
    //image << "P3\n" << width << " " << height << "\n255\n";

    vec3 origin     = vec3(0.0, 0.0, 0.0);              // ray origin
    // screen dimension (square to interest with)
    vec3 lower_left = vec3(-2.0, -1.0, -1.0);
    vec3 horizontal = vec3(4.0, 0.0, 0.0);
    vec3 vertical   = vec3(0.0, 2.0, 0.0);
    
    //--------------- Hitables -----------------//
    Hitable* list[2];
    list[0] = new Sphere(vec3(0, 0, -1.f), 0.5f);
    list[1] = new Sphere(vec3(0, -100.5f, -1.f), 100.f);

    Hitable* scene= new HitableList(list, 2);

    for(int col = 0; col < height; col++)
    {
        for(int row = 0; row < width; row++)
        {
            float u = float(row)/float(width);
            float v = float(col)/float(height);

            // ray goes to each pixel in the image
            Ray ray    = Ray(origin, lower_left + u*horizontal + v*vertical);
            vec3 color = Color(ray, scene);
            
            color[0] *= 255.99f;
            color[1] *= 255.99f;
            color[2] *= 255.99f;

            image << int(color[0]) << " " << int(color[1]) << " " << int(color[2]) << " ";
        }
        image << "\n";
    }
    image.close();


    ReversePPM("output.ppm", "output2.ppm", width, height);
}



// all vectors here are [-1, 1]
// the ray direction is not normalized
// normal vector is normalized
// normal of sphere = hit point - center point
// tmin and tmax for what range on the ray to count the hit
