#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "object3d.hpp"
#include <vecmath.h>
#include <cmath>
#include <iostream>
using namespace std;

// ~~TODO~~: implement this class and add more fields as necessary,
class Triangle : public Object3D
{

public:
	Triangle() = delete;

	// a b c are three vertex positions of the triangle
	Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c, Material *m) : Object3D(m)
	{
		this->vertices[0] = a;
		this->vertices[1] = b;
		this->vertices[2] = c;
		this->normal = Vector3f::cross(b - a, c - a).normalized();
	}

	bool intersect(const Ray &r, Hit &h, float tmin) override
	{
		float a = Vector3f::dot(normal, r.getDirection());
		float b = Vector3f::dot(normal, vertices[0]) - Vector3f::dot(normal, r.getOrigin());
		if (a == 0)
			return false;
		float t = b / a;
		if ((t > h.getT()) || (t < tmin))
			return false;
		Vector3f P = r.pointAtParameter(t);
		if (Vector3f::dot(Vector3f::cross(vertices[0] - P, vertices[1] - P), normal) < 0)
			return false;
		if (Vector3f::dot(Vector3f::cross(vertices[1] - P, vertices[2] - P), normal) < 0)
			return false;
		if (Vector3f::dot(Vector3f::cross(vertices[2] - P, vertices[0] - P), normal) < 0)
			return false;
		h.set(t, material, normal);
		return true;
	}
	Vector3f normal;
	Vector3f vertices[3];

protected:
};

#endif //TRIANGLE_H
