#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <vecmath.h>
#include "object3d.hpp"

// transforms a 3D point using a matrix, returning a 3D point
static Vector3f transformPoint(const Matrix4f &mat, const Vector3f &point)
{
    return (mat * Vector4f(point, 1)).xyz();
}

// transform a 3D directino using a matrix, returning a direction
static Vector3f transformDirection(const Matrix4f &mat, const Vector3f &dir)
{
    return (mat * Vector4f(dir, 0)).xyz();
}

// ~~TODO~~: implement this class so that the intersect function first transforms the ray
class Transform : public Object3D
{
public:
    Transform() {}

    Transform(const Matrix4f &m, Object3D *obj) : o(obj)
    {
        transformIn = m.inverse();
        transformOut = m;
        BoundingBox trBox = o->getBoundingBox();
        for (int i = 0; i < 8; i++)
            box.update(transformPoint(transformOut, trBox.getVertex(i)));
    }

    ~Transform()
    {
    }

    virtual bool intersect(const Ray &r, Hit &h, float tmin)
    {
        Vector3f trSource = transformPoint(transformIn, r.getOrigin());
        Vector3f trDirection = transformDirection(transformIn, r.getDirection());
        Ray tr(trSource, trDirection);
        bool inter = o->intersect(tr, h, tmin);
        if (inter)
        {
            h.set(h.getT(), h.getMaterial(), transformDirection(transformOut, h.getNormal()).normalized());
        }
        return inter;
    }

protected:
    Object3D *o; //un-transformed object
    Matrix4f transformIn, transformOut;
};

#endif //TRANSFORM_H
