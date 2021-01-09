#ifndef SAMPLE_H
#define SAMPLE_H

#include <vecmath.h>
#include <random>

using namespace std;

static double randdouble(mt19937 *rd)
{
    return (double)rd->operator()() / rd->max();
}

static Vector3f uniformSample(const Vector3f n, mt19937 *rd)
{
    double theta = 2 * M_PI * randdouble(rd);
    double cosPhi = 2 * randdouble(rd) - 1;
    double sinPhi = sqrt(1 - cosPhi * cosPhi);
    Vector3f v = Vector3f(cos(theta) * sinPhi, sin(theta) * sinPhi, cosPhi);
    return Vector3f::dot(n, v) < 0 ? v : -v;
}

static Vector3f cosineWeightSample(const Vector3f n, mt19937 *rd)
{
    Vector3f a = Vector3f::cross(n, fabs(n.x()) > 0.1 ? Vector3f(0, 1, 0) : Vector3f(1, 0, 0)).normalized();
    Vector3f b = Vector3f::cross(n, a);
    double theta = 2 * M_PI * randdouble(rd);
    double t = randdouble(rd), sinPhi = sqrt(t), cosPhi = sqrt(1 - t);
    return cos(theta) * sinPhi * a + sin(theta) * sinPhi * b + cosPhi * n;
}

#endif