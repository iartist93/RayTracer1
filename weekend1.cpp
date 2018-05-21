#include <iostream>
#include <fstream>
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include <vector>
#include <string>
#include "camera.h"
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()


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


vec3 SampleColor(const Ray & ray, Hitable* hitable_list)
{
    HitResult hitResult;

    if(hitable_list->Hit(ray, 0, 100000000, hitResult))
    {
        vec3 normalColor = 0.5f * (hitResult.n + vec3(1.f, 1.0f, 1.0f));  // convert the noraml[-1, 1] to [0, 1] to output RGB color
        return normalColor;
    }
    else
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
    const int width = 200, height = 100, samples = 100;
    
    //----------- Objects in the scene ------------//
    Hitable* list[2];
    list[0] = new Sphere(vec3(0, 0, -1.f), 0.5f);
    list[1] = new Sphere(vec3(0, -100.5f, -1.f), 100.f);
    Hitable* scene= new HitableList(list, 2);
    
    //----------------- Camera --------------------//  
    Camera cam;

    // -------------- rendering loop --------------//
    srand(time(0));  // Initialize random number generator.

    for(int col = 0; col < height; col++)
    {
        for(int row = 0; row < width; row++)
        {
            vec3 color = vec3(0.0f);
            for(int s = 0; s < samples; s++)
            {
                float u = float(row + (rand() % 10) / 10.f )/float(width);
                float v = float(col + (rand() % 10) / 10.f )/float(height);

                // ray goes to each pixel in the image
                Ray ray = cam.RayCast(u, v);
                color += SampleColor(ray, scene);           // sample a color from a scene using a ray
            }

            color /= samples;       // averaging

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
// each pixel will have many many samples contributing to the final color
// we model in the camera space (-2, -1, -1) to (2, 1, -1)
// 