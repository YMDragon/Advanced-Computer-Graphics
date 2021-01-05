#ifndef KDTREE_H
#define KDTREE_H

#include "object3d.hpp"
#include <vector>

class KDTree : public Object3D
{
public:
    KDTree() = delete;
    KDTree(std::vector<Object3D *> &V, int l, int r, int type);

    bool intersect(const Ray &r, Hit &h, float tmin) override;

private:
    Object3D *o;
    KDTree *ls, *rs;
};

#endif