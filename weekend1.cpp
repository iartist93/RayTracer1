#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <sstream>
#include <chrono>   // for calc. execution time
   
#include "vec3.h"
#include "ray.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "texture.h"
#include "common.h"

#define OUT 

#define MAXDEPTH 4
#define MAXSAMPLE 2

void OutputPPM(std::string input, std::string output, int width, int height)
{
    std::ifstream in_image(input, std::ifstream::in);
    std::vector<std::string> imageLines;
    std::string line;
    for(int h = 0; h < height; h++)
    {
        std::getline(in_image, line);
        imageLines.push_back(line);
    }
    in_image.close();

    std::ofstream out_image;
    out_image.open(output);
    out_image << "P3\n" << width << " " << height << "\n255\n";

    for(int h = 0; h < height; h++)
    {
        std::string line = imageLines[h];
        std::stringstream ss(line);

        //std::cout << line.size() << std::endl;

        std::vector<int> pixels;
        int pixel;
        while(ss >> pixel)
        {
            pixels.push_back(pixel);
        }


        // remember the values are written as   R G B so we reverse then in the same order
        for(int i = 1; i <= pixels.size()/3; i++)
        {
            for(int j = 0; j < 3; j++)
            {
                auto it = pixels.end() - 3*i + j;
                out_image << *it << " ";
                it++;      
            }
        }
        out_image << std::endl;
    
        //out_image << imageLines[h] << std::endl;
    }
    out_image.close();
}

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

vec3 SampleColor(const Ray & ray, HitableList* hitable_list, int depth = 0)
{
    HitResult hitResult;
    
    if(hitable_list->Hit(ray, 0.001, 100000000, hitResult))
    {
        Ray scatteredRay;
        vec3 attentuation;
		if (depth < MAXDEPTH && hitResult.matPtr->Scatter(ray, hitResult, attentuation, scatteredRay))
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

HitableList* PopulateRandomScene(int n)
{
    Surface** spheresList = new Surface*[n+1];
    spheresList[0] = new Sphere(vec3(0, -1000.f, 0), 1000.f, new Lambert(
        new CheckerTexture(new Color(vec3(.2f, .3f, .1f)), new Color(vec3(.9, .9, .9)) )));

    int i = 1;
    for(int a = -11; a < 11; a++)
    {
        for(int b = -11; b < 11; b++)
        {
            float randMat = rand()%10/10.0f;
            vec3 center(a + rand()%10/10.f, 0.2f, b + rand()%10/10.f);
            
            if((center - vec3(4.f, 0.2f, 0.f)).length() > 1.5f)  // away from the front large sphere
            {
                if(randMat < 0.8f) {
                    spheresList[i++] = new Sphere(center, 0.2f, new Lambert(new Color(vec3(rand()%10/10.f, rand()%10/10.f, rand()%10/10.f))));
                } 
                else if(randMat < 0.95) {
                    spheresList[i++] = new Sphere(center, 0.2f, 
                    new Metal(vec3(0.5*(1-rand()%10/10.f), 0.5*(1-rand()%10/10.f), 0.5*(1-rand()%10/10.f)),
                                0.5f*(rand()%10/10.f)));
                }
                else {
                    spheresList[i++] = new Sphere(center, 0.2f, new Dielectric(1.5f));
                }
            }
        }
    }
    
    spheresList[i++] = new Sphere(vec3(4.f, 1.f, 0), 1.f, new Metal(vec3(0.7f, 0.6f, 0.5f), 0.0f));
    spheresList[i++] = new Sphere(vec3(0,    1.f, 0), 1.f, new Dielectric(1.5f));
    
    int width, height, noChannels;
    unsigned char *imageData = LoadTexture("pavement.jpg", width, height, noChannels);
    std::cout << width << " " << height << " " << noChannels << std::endl;
    spheresList[i++] = new Sphere(vec3(-4.f,  1.f, 0), 1.f, new Lambert(new ImageTexture(imageData, width, height)));


    return new HitableList(spheresList, i);
}

int main()
{
    auto startTime = std::chrono::steady_clock::now();

	srand((unsigned)time(NULL));

    std::ofstream image;
    image.open("output.ppm");
    const int width = 1200, height = 800, samples = 100;
    
    //----------- Objects in the scene ------------//
    // Surface* list[5];
    // list[0] = new Sphere(vec3(0, 0, -1.5f), 1.8f,         new Lambert(vec3(.8f, .3f, .3f)) );
    // list[1] = new Sphere(vec3(0, -600.9f, -1.5f), 600.f, new Lambert(vec3(.8f, .8f, 0)) );
    // list[2] = new Sphere(vec3(-3.6f, 0, -1.54f), 1.8f,     new Dielectric(1.5f) );
    // list[3] = new Sphere(vec3(3.6f, 0, -1.5f), 1.8f,      new Metal(vec3(.8f, .6f, 0.2f), 0.5f) );
    // list[4] = new Sphere(vec3(-3.65f, 0, -1.5f), -1.7f, new Dielectric(1.5f) );

    // Surface* scene= new HitableList(list, 5);

    HitableList* scene= PopulateRandomScene(500);

    //----------------- Camera --------------------//  
    vec3 cameraOrigin = vec3(13.f, 2.f, 3.f);
    vec3 cameraLookAt = vec3(0, 0, 0.f);
    
    // more in equation in page 69 Realistic RayTracer
    float focalLength = 22.f;       // f 
    float distanceFromLength  = 10.f; // s
    Camera cam(float(width), float(height), 20.f, cameraOrigin, cameraLookAt, 0.1f, focalLength, distanceFromLength);
    
    // 1- (s) can't be smaller than (f)
    // 2- apature should be very small
    std::cout << distanceFromLength << std::endl;

    // -------------- rendering loop --------------//

    for(int col = 0; col < height; col++)
    {
        for(int row = 0; row < width; row++)
        {
            vec3 color = vec3(0.0f);
            for(int s = 0; s < MAXSAMPLE; s++)
            {
                float u = float(row + (rand() % 10) / 10.f) / float(width);
                float v = float(col + (rand() % 10) / 10.f) / float(height);

                // ray goes to each pixel in the image
                Ray ray = cam.RayCast(u, v);
                color += SampleColor(ray, scene, 0);           // sample a color from a scene using a ray
            }

            color /= float(MAXSAMPLE);       // averaging
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

    // OutputPPM("output.ppm", "output2.ppm", width, height);
    ReversePPM("output.ppm", "output2.ppm", width, height);

    auto endTime = std::chrono::steady_clock::now();

    std::cout << "Time : " << std::chrono::duration<double>(endTime-startTime).count();
}



// all vectors here are [-1, 1]
// the ray direction is not normalized
// normal vector is normalized
// normal of sphere = hit point - center point
// tmin and tmax for what range on the ray to count the hit
// each pixel will have many many samples contributing to the final color
// we model in the camera space (-2, -1, -1) to (2, 1, -1)
// 


