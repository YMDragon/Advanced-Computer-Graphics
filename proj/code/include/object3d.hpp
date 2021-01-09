#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "ray.hpp"
#include "hit.hpp"
#include "material.hpp"
#include <algorithm>

using namespace std;

class BoundingBox
{
public:
    BoundingBox() : x_min(1e20), x_max(-1e20), y_min(1e20), y_max(-1e20), z_min(1e20), z_max(-1e20) {}

    BoundingBox(double x_min, double x_max, double y_min, double y_max, double z_min, double z_max) : x_min(x_min), x_max(x_max), y_min(y_min), y_max(y_max), z_min(z_min), z_max(z_max) {}

    BoundingBox(const BoundingBox &box)
    {
        x_min = box.x_min;
        x_max = box.x_max;
        y_min = box.y_min;
        y_max = box.y_max;
        z_min = box.z_min;
        z_max = box.z_max;
    }

    BoundingBox(const BoundingBox &box1, const BoundingBox &box2)
    {
        x_min = min(box1.x_min, box2.x_min);
        x_max = max(box1.x_max, box2.x_max);
        y_min = min(box1.y_min, box2.y_min);
        y_max = max(box1.y_max, box2.y_max);
        z_min = min(box1.z_min, box2.z_min);
        z_max = max(box1.z_max, box2.z_max);
    }

    virtual ~BoundingBox() = default;

    void update(const Vector3f &p)
    {
        x_min = min(x_min, p.x());
        x_max = max(x_max, p.x());
        y_min = min(y_min, p.y());
        y_max = max(y_max, p.y());
        z_min = min(z_min, p.z());
        z_max = max(z_max, p.z());
    }

    Vector3f getVertex(const int index = 0)
    {
        return Vector3f(index & 4 ? x_max : x_min, index & 2 ? y_max : y_min, index & 1 ? z_max : z_min);
    }

private:
    double x_min,
        x_max,
        y_min,
        y_max,
        z_min,
        z_max;
};

// Base class for all 3d entities.
class Object3D
{
public:
    Object3D() : material(nullptr) {}

    virtual ~Object3D() = default;

    explicit Object3D(Material *material)
    {
        this->material = material;
    }

    // Intersect Ray with this object. If hit, store information in hit structure.
    virtual bool intersect(const Ray &r, Hit &h, double tmin) = 0;

    BoundingBox getBoundingBox()
    {
        return box;
    }

protected:
    Material *material;
    BoundingBox box;
};

#endif
