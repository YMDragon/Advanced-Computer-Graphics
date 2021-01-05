#ifndef PLANE_H
#define PLANE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>

// ~~TODO~~: Implement Plane representing an infinite plane
// function: ax+by+cz=d
// choose your representation , add more fields and fill in the functions

class Plane : public Object3D
{
public:
    Plane() = delete;

    Plane(const Vector3f &normal, float d, Material *m) : Object3D(m)
    {
        this->normal = normal.normalized();
        this->d = d;
        if (fabs(normal.x()) == 1)
            box = BoundingBox(d, d, -1e20, 1e20, -1e20, 1e20);
        else if (fabs(normal.y()) == 1)
            box = BoundingBox(-1e20, 1e20, d, d, -1e20, 1e20);
        else if (fabs(normal.z()) == 1)
            box = BoundingBox(-1e20, 1e20, -1e20, 1e20, d, d);
        else
            box = BoundingBox(-1e20, 1e20, -1e20, 1e20, -1e20, 1e20);
    }

    ~Plane() override = default;

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        float a = Vector3f::dot(normal, r.getDirection());
        float b = d - Vector3f::dot(normal, r.getOrigin());
        if (a == 0)
            return false;
        float t = b / a;
        if ((t > h.getT()) || (t < tmin))
            return false;
        h.set(t, material, normal);
        return true;
    }

protected:
    Vector3f normal;
    float d;
};

#endif //PLANE_H
