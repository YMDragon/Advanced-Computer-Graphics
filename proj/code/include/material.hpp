#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>

// TODO: Implement Shade function that computes Phong introduced in class.

enum Type
{
    DIFF,
    SPEC,
    REFR
};

class Material
{
public:
    explicit Material(const Type type, const Vector3f &color = Vector3f::ZERO, const Vector3f &emission = Vector3f::ZERO, const float &refractIndex = 1) : type(type), color(color), emission(emission), refractIndex(refractIndex)
    {
    }

    virtual ~Material() = default;

    virtual Type getType() const
    {
        return type;
    }

    virtual Vector3f getColor() const
    {
        return color;
    }

    virtual Vector3f getEmission() const
    {
        return emission;
    }

    virtual float getRefractIndex() const
    {
        return refractIndex;
    }

protected:
    Type type;
    Vector3f color;
    Vector3f emission;
    float refractIndex;
};

#endif // MATERIAL_H
