#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// ~~TODO~~: Implement functions and add more fields as necessary

class Sphere : public Object3D
{
public:
    Sphere() = delete;

    Sphere(const Vector3f &center, float radius, Material *material) : Object3D(material)
    {
        this->center = center;
        this->radius = radius;
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float OH = Vector3f::dot(r.getDirection(), center - r.getOrigin()) / r.getDirection().length();
        float CH = sqrt((center - r.getOrigin()).squaredLength() - OH * OH);
        if (CH > radius)
            return false;
        float PH = sqrt(radius * radius - CH * CH);
        float t = (OH - PH) / r.getDirection().length();
        if ((t > h.getT()) || (t < tmin))
            return false;
        Vector3f P = r.pointAtParameter(t);
        h.set(t, material, (P - center).normalized());
        return true;
    }

protected:
    Vector3f center;
    float radius;
};

#endif
