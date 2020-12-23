#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iostream>
#include <random>
#include <omp.h>

#include "scene_parser.hpp"
#include "image.hpp"
#include "camera.hpp"
#include "group.hpp"

#include <string>

using namespace std;

SceneParser *sceneparser;

int DEBUG = 0;

void printVector3f(Vector3f v)
{
    printf("%f %f %f\n", v.x(), v.y(), v.z());
}

float randfloat(mt19937 *rd)
{
    return (float)rd->operator()() / rd->max();
}

Vector3f uniformSample(const Vector3f n, mt19937 *rd)
{
    float theta, cosPhi, sinPhi;
    Vector3f v;
    do
    {
        theta = 2 * M_PI * randfloat(rd);
        cosPhi = 2 * randfloat(rd) - 1;
        sinPhi = sqrt(1 - cosPhi * cosPhi);
        v = Vector3f(cos(theta) * sinPhi, sin(theta) * sinPhi, cosPhi);
    } while (Vector3f::dot(n, v) <= 0);
    return v;
}

Vector3f cosineWeightSample(const Vector3f n, mt19937 *rd)
{
    Vector3f a = Vector3f::cross(n, fabs(n.x()) > 0.1 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0)).normalized();
    Vector3f b = Vector3f::cross(n, a);
    float theta = 2 * M_PI * randfloat(rd);
    float t = randfloat(rd), sinPhi = sqrt(t), cosPhi = sqrt(1 - t);
    return cos(theta) * sinPhi * a + sin(theta) * sinPhi * b + cosPhi * n;
}

Vector3f RayTracer(const Ray r, int depth, Vector3f weight, mt19937 *rd)
{
    Hit hit;
    bool isIntersect = sceneparser->getGroup()->intersect(r, hit, 1e-4);
    if (!isIntersect)
        return sceneparser->getBackgroundColor();

    Vector3f hitPosition = r.pointAtParameter(hit.getT());
    Material *material = hit.getMaterial();

    if (depth >= 10)
    {
        Vector3f c = material->getColor();
        float k = max(max(c.x(), c.y()), c.z());
        if (randfloat(rd) < k)
            weight = weight / k;
        else
            return Vector3f::ZERO; //weight * material->getEmission();
    }

    if (material->getType() == DIFF)
    {
        Vector3f R = cosineWeightSample(hit.getNormal(), rd);
        return weight * material->getEmission() + RayTracer(Ray(hitPosition, R), depth + 1, material->getColor() * weight, rd);
    }
    else if (material->getType() == SPEC)
    {
        Vector3f R = r.getDirection() - 2 * Vector3f::dot(hit.getNormal(), r.getDirection()) * hit.getNormal();
        return weight * material->getEmission() + RayTracer(Ray(hitPosition, R), depth + 1, material->getColor() * weight, rd);
    }
    else if (material->getType() == REFR)
    {
        bool into = Vector3f::dot(hit.getNormal(), r.getDirection()) < 0;
        float rate = into ? 1 / material->getRefractIndex() : material->getRefractIndex();
        float cosThetaI = (into ? -1 : 1) * Vector3f::dot(hit.getNormal(), r.getDirection());
        Vector3f R1 = r.getDirection() - 2 * Vector3f::dot(hit.getNormal(), r.getDirection()) * hit.getNormal();
        if ((1 - cosThetaI * cosThetaI) * rate * rate > 1)
            return weight * material->getEmission() + RayTracer(Ray(hitPosition, R1), depth + 1, material->getColor() * weight, rd);
        float cosThetaO = sqrt(1 - rate * rate * (1 - cosThetaI * cosThetaI));
        Vector3f R2 = rate * r.getDirection() - (rate * Vector3f::dot(hit.getNormal(), r.getDirection()) + (into ? 1 : -1) * cosThetaO) * hit.getNormal();
        float r0 = (rate - 1) * (rate - 1) / (rate + 1) / (rate + 1);
        float r = r0 + (1 - r0) * pow(1 - cosThetaI, 5);
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

#pragma omp parallel for schedule(dynamic, 1)
    for (int x = 0; x < camera->getWidth(); x++)
    {
        fprintf(stderr, "\rRendering (%d spp) %5.2f%%", M, 100. * (x + 1) / camera->getWidth());
        mt19937 rd(rand());
        for (int y = 0; y < camera->getHeight(); y++)
        {
            Vector3f color = Vector3f::ZERO;
            for (int i = 0; i < M; i++)
            {
                Ray camRay = camera->generateRay(Vector2f(x + randfloat(&rd), y + randfloat(&rd)));
                color += RayTracer(camRay, 0, Vector3f(1, 1, 1), &rd);
            }
            img.SetPixel(x, y, color / M);
        }
    }

    img.SaveBMP(outputFile.c_str());

    return 0;
}
