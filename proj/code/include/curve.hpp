#ifndef CURVE_HPP
#define CURVE_HPP

#include "object3d.hpp"
#include <vecmath.h>
#include <vector>
#include <utility>

#include <algorithm>

// TODO (PA3): Implement Bernstein class to compute spline basis function.
//       You may refer to the python-script for implementation.

// The CurvePoint object stores information about a point on a curve
// after it has been tesselated: the vertex (V) and the tangent (T)
// It is the responsiblility of functions that create these objects to fill in all the data.
struct CurvePoint
{
    Vector3f V; // Vertex
    Vector3f T; // Tangent  (unit)
};

class Curve : public Object3D
{
protected:
    std::vector<Vector3f> controls;

public:
    explicit Curve(std::vector<Vector3f> points) : controls(std::move(points)) {}

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        return false;
    }

    std::vector<Vector3f> &getControls()
    {
        return controls;
    }

    virtual void discretize(int resolution, std::vector<CurvePoint> &data) = 0;
};

class BezierCurve : public Curve
{
public:
    explicit BezierCurve(const std::vector<Vector3f> &points) : Curve(points)
    {
        if (points.size() < 4 || points.size() % 3 != 1)
        {
            printf("Number of control points of BezierCurve must be 3n+1!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint> &data) override
    {
        data.clear();
        // TODO (PA3): fill in data vector
        int n = controls.size() - 1;
        double B[n + 1], dB[n + 1];
        for (int I = 0; I <= resolution; I++)
        {
            double t = 1.0 * I / resolution;
            B[0] = 1;
            for (int i = 1; i <= n; i++)
                B[i] = 0;
            for (int i = 1; i < n; i++)
            {
                for (int j = i; j; j--)
                    B[j] = (1 - t) * B[j] + t * B[j - 1];
                B[0] = (1 - t) * B[0];
            }
            for (int j = n; j; j--)
            {
                dB[j] = n * (B[j - 1] - B[j]);
                B[j] = (1 - t) * B[j] + t * B[j - 1];
            }
            dB[0] = -n * B[0];
            B[0] = (1 - t) * B[0];
            CurvePoint P = (CurvePoint){Vector3f::ZERO, Vector3f::ZERO};
            for (int i = 0; i <= n; i++)
            {
                P.V += B[i] * controls[i];
                P.T += dB[i] * controls[i];
            }
            P.T.normalize();
            data.push_back(P);
        }
    }

protected:
};

class BsplineCurve : public Curve
{
public:
    BsplineCurve(const std::vector<Vector3f> &points) : Curve(points)
    {
        if (points.size() < 4)
        {
            printf("Number of control points of BspineCurve must be more than 4!\n");
            exit(0);
        }
    }

    void discretize(int resolution, std::vector<CurvePoint> &data) override
    {
        data.clear();
        // ~~TODO~~ (PA3): fill in data vector
        int n = controls.size() - 1, k = 3;
        double B[n + k + 1], dB[n + 1], t[n + k + 2];
        for (int i = 0; i <= n + k + 1; i++)
            t[i] = 1.0 * i / (n + k + 1);
        for (int I = k; I <= n; I++)
            for (int J = 0; J <= resolution; J++)
            {
                if (I != n && J == resolution)
                    continue;
                double T = ((resolution - J) * t[I] + J * t[I + 1]) / resolution;
                for (int i = 0; i <= n + k; i++)
                    B[i] = 0;
                B[I] = 1;
                for (int p = 1; p < k; p++)
                    for (int i = 0; i <= n + k - p; i++)
                        B[i] = (T - t[i]) / (t[i + p] - t[i]) * B[i] + (t[i + p + 1] - T) / (t[i + p + 1] - t[i + 1]) * B[i + 1];
                for (int i = 0; i <= n; i++)
                {
                    dB[i] = k * (B[i] / (t[i + k] - t[i]) - B[i + 1] / (t[i + k + 1] - t[i + 1]));
                    B[i] = (T - t[i]) / (t[i + k] - t[i]) * B[i] + (t[i + k + 1] - T) / (t[i + k + 1] - t[i + 1]) * B[i + 1];
                }
                CurvePoint P = (CurvePoint){Vector3f::ZERO, Vector3f::ZERO};
                for (int i = 0; i <= n; i++)
                {
                    P.V += B[i] * controls[i];
                    P.T += dB[i] * controls[i];
                }
                P.T.normalize();
                data.push_back(P);
            }
    }

protected:
};

#endif // CURVE_HPP
