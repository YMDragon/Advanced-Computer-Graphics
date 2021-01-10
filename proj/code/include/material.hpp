#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include "image.hpp"
#include <iostream>
#include <string.h>

enum Type
{
    DIFF,
    SPEC,
    REFR,
    MIX
};

class Material
{
public:
    explicit Material(const Type type, const char *filename, const Vector3f &color = Vector3f::ZERO, const Vector3f &emission = Vector3f::ZERO, const double &rate = 1) : type(type), color(color), emission(emission), rate(rate)
    {
        if (filename[0] != 0)
        {
            const char *ext = &filename[strlen(filename) - 4];
            if (!strcmp(ext, ".tga"))
                texture = Image::LoadTGA(filename);
            else if (!strcmp(ext, ".ppm"))
                texture = Image::LoadPPM(filename);
            else
                texture = NULL;
        }
        else
            texture = NULL;
    }

    virtual ~Material() = default;

    virtual Type getType() const
    {
        return type;
    }

    virtual Vector3f getColor(Vector2f tex) const
    {
        if (texture == NULL)
            return color;
        else
        {
            tex.x() -= int(tex.x()) - (tex.x() < 0);
            tex.y() -= int(tex.y()) - (tex.y() < 0);
            return texture->GetPixel(int(tex.x() * texture->Width()), int(tex.y() * texture->Height()));
        }
    }

    virtual Vector3f getEmission() const
    {
        return emission;
    }

    virtual double getRate() const
    {
        return rate;
    }

protected:
    Type type;
    Vector3f color;
    Image *texture;
    Vector3f emission;
    double rate;
};

#endif // MATERIAL_H
