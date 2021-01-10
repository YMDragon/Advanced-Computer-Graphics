#ifndef MESH_H
#define MESH_H

#include <vector>
#include "object3d.hpp"
#include "triangle.hpp"
#include "kdtree.hpp"
#include "Vector2f.h"
#include "Vector3f.h"

class Mesh : public Object3D
{

public:
    Mesh(const char *filename, Material *m);

    struct TriangleIndex
    {
        TriangleIndex()
        {
            x[0] = 0;
            x[1] = 0;
            x[2] = 0;
        }
        int &operator[](const int i) { return x[i]; }
        // By Computer Graphics convention, counterclockwise winding is front face
        int x[3]{};
    };

    std::vector<Vector3f> v;
    std::vector<Vector2f> vt;
    std::vector<Vector3f> vn;
    std::vector<TriangleIndex> f;
    std::vector<TriangleIndex> ft;
    std::vector<TriangleIndex> fn;
    std::vector<Object3D *> V;
    bool intersect(const Ray &r, Hit &h, double tmin) override;

private:
    KDTree *rt;
};

#endif
