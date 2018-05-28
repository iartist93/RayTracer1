#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"


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

vec3 SampleColor(const Ray & ray, Surface* hitable_list, int depth = 0)
{
    HitResult hitResult;
    
    if(hitable_list->Hit(ray, 0.001, 100000000, hitResult))
    {
        Ray scatteredRay;
        vec3 attentuation;
		if (depth < 50 && hitResult.matPtr->Scatter(ray, hitResult, attentuation, scatteredRay))
            return attentuation * SampleColor(scatteredRay, hitable_list, depth + 1);
		else    // if not scatterd -> absorb this ray (if angel > 90) <- don't scatter below the surface
			return vec3(0.0f); 
    }
    else // if doesn't hit any object return the background color (environment)
    {
        vec3 ray_dir = unit_vector(ray.Direction());
        float t = 0.5 * (ray_dir.y() + 1.0);            // convert from [-1, 1] to [0, 1]
        return (1.0-t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
    }
}

int main()
{
	srand((unsigned)time(NULL));

    std::ofstream image;
    image.open("output.ppm");
    const int width = 800, height = 600, samples = 100;
    
    //----------- Objects in the scene ------------//
    Surface* list[5];
    list[0] = new Sphere(vec3(0, 0, -1.5f), 1.8f,         new Lambert(vec3(.8f, .3f, .3f)) );
    list[1] = new Sphere(vec3(0, -600.9f, -1.5f), 600.f, new Lambert(vec3(.8f, .8f, 0)) );
    list[2] = new Sphere(vec3(-3.6f, 0, -1.54f), 1.8f,     new Dielectric(1.5f) );
    list[3] = new Sphere(vec3(3.6f, 0, -1.5f), 1.8f,      new Metal(vec3(.8f, .6f, 0.2f), 0.5f) );
    list[4] = new Sphere(vec3(-3.65f, 0, -1.5f), -1.7f, new Dielectric(1.5f) );

    Surface* scene= new HitableList(list, 5);
    
    //----------------- Camera --------------------//  
    vec3 cameraOrigin = vec3(6.f, 6.f, 4.f);
    vec3 cameraLookAt = vec3(0, 0, -1.f);
    Camera cam(width, height, 60.f, cameraOrigin, cameraLookAt, 2.f, (cameraLookAt-cameraOrigin).length());

    // -------------- rendering loop --------------//

    for(int col = 0; col < height; col++)
    {
        for(int row = 0; row < width; row++)
        {
            vec3 color = vec3(0.0f);
            for(int s = 0; s < samples; s++)
            {
                float u = float(row + (rand() % 10) / 10.f) / float(width);
                float v = float(col + (rand() % 10) / 10.f) / float(height);

                // ray goes to each pixel in the image
                Ray ray = cam.RayCast(u, v);
                color += SampleColor(ray, scene, 0);           // sample a color from a scene using a ray
            }

            color /= float(samples);       // averaging
            color = vec3(sqrt(color[0]), sqrt(color[1]), sqrt(color[2]));

            color[0] *= 255.99f;
            color[1] *= 255.99f;
            color[2] *= 255.99f;

            image << int(color[0]) << " " << int(color[1]) << " " << int(color[2]) << " ";
        }
		//std::cout << col << std::endl;
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