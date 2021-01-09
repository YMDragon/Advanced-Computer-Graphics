#ifndef HIT_H
#define HIT_H

#include <vecmath.h>
#include "ray.hpp"

class Material;
class Object3D;

class Hit
{
public:
    // constructors
    Hit()
    {
        material = nullptr;
        t = 1e38;
    }

    Hit(double _t, Material *m, const Vector3f &n, const Vector2f &tex)
    {
        t = _t;
        material = m;
        normal = n;
        texture = tex;
    }

    Hit(const Hit &h)
    {
        t = h.t;
        material = h.material;
        normal = h.normal;
        texture = h.texture;
    }

    // destructor
    ~Hit() = default;

    double getT() const
    {
        return t;
    }

    Material *getMaterial() const
    {
        return material;
    }

    const Vector3f &getNormal() const
    {
        return normal;
    }

    const Vector2f &getTexture() const
    {
        return texture;
    }

    void set(double _t, Material *m, const Vector3f &n, const Vector2f &tex)
    {
        t = _t;
        material = m;
        normal = n;
        texture = tex;
    }

private:
    double t;
    Material *material;
    Vector3f normal;
    Vector2f texture;
};

inline std::ostream &operator<<(std::ostream &os, const Hit &h)
{
    os << "Hit <" << h.getT() << ", " << h.getNormal() << ">";
    return os;
}

#endif // HIT_H
