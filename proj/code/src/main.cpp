#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <ctime>
#include <omp.h>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"
#include "sample.hpp"

#include <string>

using namespace std;

SceneParser *sceneparser;

int DEBUG = 0;

void printVector3f(Vector3f v)
{
    printf("%f %f %f\n", v.x(), v.y(), v.z());
}

Vector3f RayTracer(const Ray r, int depth, Vector3f weight, mt19937 *rd)
{
    Hit hit;
    bool isIntersect = sceneparser->getGroup()->intersect(r, hit, 1e-3);
    if (!isIntersect)
        return sceneparser->getBackgroundColor();

    Vector3f hitPosition = r.pointAtParameter(hit.getT());
    Material *material = hit.getMaterial();

    if (depth >= 10)
    {
        Vector3f c = material->getColor();
        //float k = max(max(c.x(), c.y()), c.z());
        float k = max(max(weight.x(), weight.y()), weight.z());
        if ((randfloat(rd) < k) && (depth <= 100))
            weight = weight / k;
        else
            return Vector3f::ZERO; //weight * material->getEmission();
    }

    bool into = Vector3f::dot(hit.getNormal(), r.getDirection()) < 0;
    if (material->getType() == DIFF)
    {
        Vector3f R = cosineWeightSample((into ? 1 : -1) * hit.getNormal(), rd);
        return weight * material->getEmission() + RayTracer(Ray(hitPosition, R), depth + 1, material->getColor() * weight, rd);
    }
    else if (material->getType() == SPEC)
    {
        Vector3f R = r.getDirection() - 2 * Vector3f::dot(hit.getNormal(), r.getDirection()) * hit.getNormal();
        return weight * material->getEmission() + RayTracer(Ray(hitPosition, R), depth + 1, material->getColor() * weight, rd);
    }
    else if (material->getType() == REFR)
    {
        float rate = into ? 1 / material->getRefractIndex() : material->getRefractIndex();
        float cosThetaI = (into ? -1 : 1) * Vector3f::dot(hit.getNormal(), r.getDirection());
        Vector3f R1 = r.getDirection() - 2 * Vector3f::dot(hit.getNormal(), r.getDirection()) * hit.getNormal();
        if ((1 - cosThetaI * cosThetaI) * rate * rate > 1)
            return weight * material->getEmission() + RayTracer(Ray(hitPosition, R1), depth + 1, material->getColor() * weight, rd);
        float cosThetaO = sqrt(1 - rate * rate * (1 - cosThetaI * cosThetaI));
        Vector3f R2 = rate * r.getDirection() - (rate * Vector3f::dot(hit.getNormal(), r.getDirection()) + (into ? 1 : -1) * cosThetaO) * hit.getNormal();
        float r0 = (rate - 1) * (rate - 1) / (rate + 1) / (rate + 1);
        float r = r0 + (1 - r0) * pow(1 - (into ? cosThetaI : cosThetaO), 5);
        if (depth < 3)
            return weight * material->getEmission() + RayTracer(Ray(hitPosition, R1), depth + 1, material->getColor() * r * weight, rd) + RayTracer(Ray(hitPosition, R2), depth + 1, material->getColor() * (1 - r) * weight, rd);
        else
        {
            if (randfloat(rd) < r)
                return weight * material->getEmission() + RayTracer(Ray(hitPosition, R1), depth + 1, material->getColor() * weight, rd);
            else
                return weight * material->getEmission() + RayTracer(Ray(hitPosition, R2), depth + 1, material->getColor() * weight, rd);
        }
    }
    else
    {
        return sceneparser->getBackgroundColor();
    }
}

int main(int argc, char *argv[])
{
    for (int argNum = 1; argNum < argc; ++argNum)
    {
        std::cout << "Argument " << argNum << " is: " << argv[argNum] << std::endl;
    }

    if (argc < 3)
    {
        cout << "Usage: ./bin/PROJECT <input scene file> <output bmp file> [<samples>]" << endl;
        return 1;
    }
    string inputFile = argv[1];
    string outputFile = argv[2];
    int M = 1;
    if (argc >= 4)
        M = atoi(argv[3]);

    srand(time(NULL));
    sceneparser = new SceneParser(inputFile.c_str());
    Camera *camera = sceneparser->getCamera();
    Image img(camera->getWidth(), camera->getHeight());

    int count = 0;
    int start_time = time(NULL), used_time, estimate_time;

#pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < camera->getWidth(); x++)
    {
        mt19937 rd(rand());
        for (int y = 0; y < camera->getHeight(); y++)
        {
            Vector3f color = Vector3f::ZERO;
            for (int i = 0; i < M; i++)
            {
                Ray camRay = camera->generateRay(Vector2f(x + randfloat(&rd), y + randfloat(&rd)), &rd);
                color += RayTracer(camRay, 0, Vector3f(1, 1, 1), &rd);
            }
            img.SetPixel(x, y, color / M);
        }
        if ((++count) % 10 == 0)
            img.SaveBMP(outputFile.c_str());
        used_time = time(NULL) - start_time;
        estimate_time = (long long)used_time * (camera->getWidth() - count) / count;
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%, used time: %dh%02dm%02ds, estimate time: %dh%02dm%02ds", M, 100. * count / camera->getWidth(), used_time / 3600, used_time / 60 % 60, used_time % 60, estimate_time / 3600, estimate_time / 60 % 60, estimate_time % 60);
    }

    fprintf(stderr, "\n");
    img.SaveBMP(outputFile.c_str());

    return 0;
}
