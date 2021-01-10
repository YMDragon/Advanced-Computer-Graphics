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
		vertices[0] = a;
		vertices[1] = b;
		vertices[2] = c;
		normal = Vector3f::cross(b - a, c - a).normalized();
		abc = Vector3f::dot(Vector3f::cross(b - a, c - a), normal);
		vnormal[0] = vnormal[1] = vnormal[2] = normal;
		box.update(vertices[0]);
		box.update(vertices[1]);
		box.update(vertices[2]);
	}

	void setTexture(const Vector2f &ta, const Vector2f &tb, const Vector2f &tc)
	{
		textures[0] = ta;
		textures[1] = tb;
		textures[2] = tc;
	}

	void setNormal(const Vector3f &na, const Vector3f &nb, const Vector3f &nc)
	{
		vnormal[0] = na;
		vnormal[1] = nb;
		vnormal[2] = nc;
	}

	bool intersect(const Ray &r, Hit &h, double tmin) override
	{
		double a = Vector3f::dot(normal, r.getDirection());
		double b = Vector3f::dot(normal, vertices[0]) - Vector3f::dot(normal, r.getOrigin());
		if (a == 0)
			return false;
		double t = b / a;
		if ((t > h.getT()) || (t < tmin))
			return false;
		Vector3f P = r.pointAtParameter(t);
		double pab = Vector3f::dot(Vector3f::cross(vertices[0] - P, vertices[1] - P), normal);
		double pbc = Vector3f::dot(Vector3f::cross(vertices[1] - P, vertices[2] - P), normal);
		double pca = Vector3f::dot(Vector3f::cross(vertices[2] - P, vertices[0] - P), normal);
		if (pab < 0 || pbc < 0 || pca < 0)
			return false;
		Vector2f texture = (textures[0] * pbc + textures[1] * pca + textures[2] * pab) / abc;
		Vector3f normal_ = (vnormal[0] * pbc + vnormal[1] * pca + vnormal[2] * pab) / abc;
		h.set(t, material, normal_, texture);
		return true;
	}

	Vector3f normal;
	Vector3f vertices[3];
	Vector3f vnormal[3];
	Vector2f textures[3];

protected:
	double abc;
};

#endif //TRIANGLE_H
