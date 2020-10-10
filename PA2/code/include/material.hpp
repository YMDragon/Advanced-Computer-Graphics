#ifndef MATERIAL_H
#define MATERIAL_H

#include <cassert>
#include <vecmath.h>

#include "ray.hpp"
#include "hit.hpp"
#include <iostream>
#include <glut.h>

// ~~TODO~~ (PA2): Copy from PA1.
class Material
{
public:
    explicit Material(const Vector3f &d_color, const Vector3f &s_color = Vector3f::ZERO, float s = 0) : diffuseColor(d_color), specularColor(s_color), shininess(s)
    {
    }

    virtual ~Material() = default;

    virtual Vector3f getDiffuseColor() const
    {
        return diffuseColor;
    }

    Vector3f Shade(const Ray &ray, const Hit &hit,
                   const Vector3f &dirToLight, const Vector3f &lightColor)
    {
        float diffuse = Vector3f::dot(dirToLight, hit.getNormal());
        if (diffuse < 0)
            diffuse = 0;
        Vector3f R = 2 * Vector3f::dot(hit.getNormal(), dirToLight) * hit.getNormal() - dirToLight;
        float specular = Vector3f::dot(-ray.getDirection(), R.normalized());
        if (specular < 0)
            specular = 0;
        specular = pow(specular, shininess);
        return lightColor * (diffuseColor * diffuse + specularColor * specular);
    }

    // For OpenGL, this is fully implemented
    void Use()
    {
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, Vector4f(diffuseColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, Vector4f(specularColor, 1.0f));
        glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, Vector2f(shininess * 4.0, 1.0f));
    }

protected:
    Vector3f diffuseColor;
    Vector3f specularColor;
    float shininess;
};

#endif // MATERIAL_H
