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
#include "moving_sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "material.h"
#include "rectangle.h"
#include "texture.h"
#include "common.h"
#include "flip_normals.h"
#include "box.h"
#define OUT 

#define MAXDEPTH 50
#define MAXSAMPLE 100

#define _CRT_RAND_S  
#include <stdlib.h>  
#include <stdio.h>  
#include <limits.h>  



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
    if(hitable_list->Hit(ray, 0.1f, 100000000, hitResult))
    {
        Ray scatteredRay;
        vec3 attentuation;
        vec3 emittedColor = hitResult.matPtr->Emitted(hitResult.u, hitResult.v, hitResult.p);

		if (depth < MAXDEPTH && hitResult.matPtr->Scatter(ray, hitResult, attentuation, scatteredRay)) {
            //std::cout << hitResult.n.x() << " "  << hitResult.n.y() << " "  << hitResult.n.z() << std::endl;
            //std::cout << scatteredRay.Direction().x() << std::endl; 
            return emittedColor + attentuation * SampleColor(scatteredRay, hitable_list, depth + 1);
        }
		else   
            // if not scatterd 
            //      -> absorb this ray (if angel > 90) <- don't scatter below the surface
            //      -> if it emit color return that color
            //std::cout << "Emitted " << emittedColor.x() << " "  << emittedColor.y() << " "  << emittedColor.z() << std::endl;
			return emittedColor;
    }
    else // if doesn't hit any object return the background color (black in this case)
    {
        // vec3 unit_direction = unit_vector(ray.Direction());
        // float t = 0.5*(unit_direction.y() + 1.0);
        // return (1.0-t)*vec3(1.0, 1.0, 1.0) + t*vec3(0.5, 0.7, 1.0);
        return vec3(0.0f);
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
            float randMat = (rand() / (RAND_MAX + 1.0));
            vec3 center(a + (rand() / (RAND_MAX + 1.0)), 0.2f, b + (rand() / (RAND_MAX + 1.0)));
            
            if((center - vec3(4.f, 0.2f, 0.f)).length() > 1.5f)  // away from the front large sphere
            {
                if(randMat < 0.8f) {
                    spheresList[i++] = new MovingSphere(center, center + vec3(0, .5f * (rand() / (RAND_MAX + 1.0)), 0), 0.f, 1.f, 0.2f, 
                                new Lambert(new Color(vec3((rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)), (rand() / (RAND_MAX + 1.0)))))
                                    );
                } 
                else if(randMat < 0.95) {
                    spheresList[i++] = new Sphere(center, 0.2f, 
                    new Metal(vec3(0.5*(1-(rand() / (RAND_MAX + 1.0))), 0.5*(1-(rand() / (RAND_MAX + 1.0))), 0.5*(1-(rand() / (RAND_MAX + 1.0)))),
                                0.5f*((rand() / (RAND_MAX + 1.0)))));
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
    unsigned char *imageData = LoadTexture("waterPaint.jpg", width, height, noChannels);
    spheresList[i++] = new Sphere(vec3(-4.f,  1.f, 0), 1.f, new Lambert(new ImageTexture(imageData, width, height)));


    return new HitableList(spheresList, i);
}


HitableList* CornellBox()
{
    Surface** list = new Surface*[8];

    Material* red    = new Lambert(new Color(vec3(0.65f, 0.05f, 0.05f)));
    Material* white  = new Lambert(new Color(vec3(0.73f, 0.73f, 0.73f)));
    Material* green  = new Lambert(new Color(vec3(0.12f, 0.45f, 0.15f)));
    Material* light  = new DiffuseLightMat(new Color(vec3(15, 15, 15)));
    Material* light2  = new DiffuseLightMat(new Color(vec3(7, 7, 7)));

    int i = 0; 
    list[i++] = new FlipNormals(new RectangleYZ(0, 555, 0, 555, 555, green));
    list[i++] = new RectangleYZ(0, 555, 0, 555, 0, red);

    list[i++] = new FlipNormals(new RectangleXZ(0, 555, 0, 555, 555, white));
    list[i++] = new RectangleXZ(0, 555, 0, 555, 0, white);

    list[i++] = new FlipNormals(new RectangleXY(0, 555, 0, 555, 555, white)); 

    list[i++] = new Box(vec3(130, 0, 65), vec3(295, 165, 230), white);  
    list[i++] = new Box(vec3(265, 0, 295), vec3(430, 330, 460), white);  

    // int width, height, noChannels;
    // unsigned char *imageData = LoadTexture("waterPaint.jpg", width, height, noChannels);
    // list[i++] = new Sphere(vec3(212.5f, 265.f, 174.5f), 100.f, new Lambert(new ImageTexture(imageData, width, height)));

    list[i++] = new FlipNormals(new RectangleXZ(200, 325, 214, 345, 554.9f, light));
    list[i++] = new FlipNormals(new RectangleXZ(325, 465, 227, 332, 554.9f, light2));

    // list[i++] = new Sphere(vec3(278.f, 278.f, 278.f), 50.f, light2);
    // list[i++] = (new RectangleXZ(213, 343, 227, 332, 554, light));

    return new HitableList(list, i);
}

int main()
{
    auto startTime = std::chrono::steady_clock::now();
    
	srand((unsigned)time(NULL));

    std::ofstream image;
    image.open("output.ppm");
    const int width = 800, height = 800, samples = 100;
    
    //----------- Objects in the scene ------------//
    // Surface* list[5];
    // list[0] = new Sphere(vec3(0, 0, -1.5f), 1.8f,         new Lambert(vec3(.8f, .3f, .3f)) );
    // list[1] = new Sphere(vec3(0, -600.9f, -1.5f), 600.f, new Lambert(vec3(.8f, .8f, 0)) );
    // list[2] = new Sphere(vec3(-3.6f, 0, -1.54f), 1.8f,     new Dielectric(1.5f) );
    // list[3] = new Sphere(vec3(3.6f, 0, -1.5f), 1.8f,      new Metal(vec3(.8f, .6f, 0.2f), 0.5f) );
    // list[4] = new Sphere(vec3(-3.65f, 0, -1.5f), -1.7f, new Dielectric(1.5f) );

    // Surface* scene= new HitableList(list, 5);

    // HitableList* scene= PopulateRandomScene(500);

    HitableList* scene= CornellBox();

    //----------------- Camera --------------------//  
    vec3 cameraOrigin = vec3(277.f, 277.f, -800.f);     // middle
    vec3 cameraLookAt = vec3(277.f, 277.f, 0.f);            
    
    // more in equation in page 69 Realistic RayTracer
    float focalLength = 22.f;            // f 
    float distanceFromLength  = 10.f;    // s
    float aparture = 0.0f;
    float FOV = 40.f;

    Camera cam(float(width), float(height), FOV, cameraOrigin, cameraLookAt, aparture, focalLength, distanceFromLength, 0.0, 1.0);
    
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
                float u = float(row + (rand() / (RAND_MAX + 1.0))) / float(width);
                float v = float(col + (rand() / (RAND_MAX + 1.0))) / float(height);

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


