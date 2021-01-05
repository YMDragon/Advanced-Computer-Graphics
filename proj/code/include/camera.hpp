#ifndef CAMERA_H
#define CAMERA_H

#include "ray.hpp"
#include <vecmath.h>
#include <float.h>
#include <cmath>
#include "sample.hpp"

class Camera
{
public:
    Camera(const Vector3f &center, const Vector3f &direction, const Vector3f &up, int imgW, int imgH)
    {
        this->center = center;
        this->direction = direction.normalized();
        this->horizontal = Vector3f::cross(this->direction, up);
        this->up = Vector3f::cross(this->horizontal, this->direction);
        this->width = imgW;
        this->height = imgH;
    }

    // Generate rays for each screen-space coordinate
    virtual Ray generateRay(const Vector2f &point) = 0;
    virtual Ray generateRay(const Vector2f &point, mt19937 *rd) = 0;
    virtual ~Camera() = default;

    int getWidth() const { return width; }
    int getHeight() const { return height; }

protected:
    // Extrinsic parameters
    Vector3f center;
    Vector3f direction;
    Vector3f up;
    Vector3f horizontal;
    // Intrinsic parameters
    int width;
    int height;
};

// ~~TODO~~: Implement Perspective camera
// You can add new functions or variables whenever needed.
class PerspectiveCamera : public Camera
{

public:
    PerspectiveCamera(const Vector3f &center, const Vector3f &direction,
                      const Vector3f &up, int imgW, int imgH, float angle) : Camera(center, direction, up, imgW, imgH)
    {
        // angle is in radian.
        length = tan(angle / 2) / imgH * 2;
        R = Matrix3f(this->horizontal, this->up, this->direction);
    }

    Ray generateRay(const Vector2f &point) override
    {
        Vector2f p = (point - 0.5 * Vector2f(width, height)) * length;
        Vector3f P = Vector3f(p.x(), p.y(), 1);
        return Ray(center, (R * P).normalized());
    }

    Ray generateRay(const Vector2f &point, mt19937 *rd) override
    {
        return generateRay(point);
    }

private:
    float length;
    Matrix3f R;
};

class FocusCamera : public Camera
{
public:
    FocusCamera(const Vector3f &center, const Vector3f &direction,
                const Vector3f &up, int imgW, int imgH, float angle,
                const Vector3f &focus, float radius) : Camera(center, direction, up, imgW, imgH)
    {
        // angle is in radian.
        length = tan(angle / 2) / imgH * 2;
        R = Matrix3f(this->horizontal, this->up, this->direction);
        focusLength = Vector3f::dot(focus - center, this->direction);
        this->radius = radius;
    }

    Ray generateRay(const Vector2f &point) override
    {
        Vector2f p = (point - 0.5 * Vector2f(width, height)) * length;
        Vector3f P = Vector3f(p.x(), p.y(), 1);
        return Ray(center, (R * P).normalized());
    }

    Ray generateRay(const Vector2f &point, mt19937 *rd) override
    {
        Vector2f p = (point - 0.5 * Vector2f(width, height)) * length;
        Vector3f P = Vector3f(p.x(), p.y(), 1);
        Vector3f d = radius * uniformSample(direction, rd);
        return Ray(center + d, (R * P * focusLength - d).normalized());
    }

private:
    float length;
    Matrix3f R;
    float focusLength;
    float radius;
};

#endif //CAMERA_H
