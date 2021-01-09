#ifndef SPHERE_H
#define SPHERE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <algorithm>

// ~~TODO~~: Implement functions and add more fields as necessary

class Sphere : public Object3D
{
public:
    Sphere() = delete;

    Sphere(const Vector3f &center, double radius, Material *material) : Object3D(material)
    {
        this->center = center;
        this->radius = radius;
        box.update(center + Vector3f(radius));
        box.update(center - Vector3f(radius));
    }

    ~Sphere() override = default;

    bool intersect(const Ray &r, Hit &h, double tmin) override
    {
        double OH = Vector3f::dot(r.getDirection(), center - r.getOrigin()) / r.getDirection().length();
        double CH = sqrt(std::max((double)0, (center - r.getOrigin()).squaredLength() - OH * OH));
        if (CH > radius)
            return false;
        double PH = sqrt(std::max((double)0, radius * radius - CH * CH));
        double t = (OH - PH) / r.getDirection().length();
        if (t < tmin)
            t = (OH + PH) / r.getDirection().length();
        if ((t > h.getT()) || (t < tmin))
            return false;
        Vector3f P = (r.pointAtParameter(t) - center).normalized();
        h.set(t, material, P, Vector2f(atan2(P.y(), P.x()) / M_PI / 2 + 0.5, acos(P.z()) / M_PI));
        return true;
    }

protected:
    Vector3f center;
    double radius;
};

#endif
