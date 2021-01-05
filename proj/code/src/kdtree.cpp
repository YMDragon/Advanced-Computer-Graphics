#include "kdtree.hpp"
#include <algorithm>

bool cmp_x(Object3D *a, Object3D *b)
{
    return a->getBoundingBox().getVertex().x() < b->getBoundingBox().getVertex().x();
}

bool cmp_y(Object3D *a, Object3D *b)
{
    return a->getBoundingBox().getVertex().y() < b->getBoundingBox().getVertex().y();
}

bool cmp_z(Object3D *a, Object3D *b)
{
    return a->getBoundingBox().getVertex().z() < b->getBoundingBox().getVertex().z();
}

void update(float &t_min, float &t_max, float t0, float t1)
{
    if (t0 > t1)
        std::swap(t0, t1);
    t_min = std::max(t_min, t0);
    t_max = std::min(t_max, t1);
}

KDTree::KDTree(std::vector<Object3D *> &V, int l, int r, int type)
{
    if (type == 0)
        std::sort(V.begin() + l, V.begin() + r, cmp_x);
    if (type == 1)
        std::sort(V.begin() + l, V.begin() + r, cmp_y);
    if (type == 2)
        std::sort(V.begin() + l, V.begin() + r, cmp_z);
    int mid = (l + r) >> 1;
    o = V[mid];
    box = o->getBoundingBox();
    if (l < mid)
    {
        ls = new KDTree(V, l, mid, (type + 1) % 3);
        box = BoundingBox(box, ls->getBoundingBox());
    }
    if (mid + 1 < r)
    {
        rs = new KDTree(V, mid + 1, r, (type + 1) % 3);
        box = BoundingBox(box, rs->getBoundingBox());
    }
}

bool KDTree::intersect(const Ray &r, Hit &h, float tmin)
{
    float t_min = tmin, t_max = h.getT();
    Vector3f a = (box.getVertex(0) - r.getOrigin()) / r.getDirection();
    Vector3f b = (box.getVertex(7) - r.getOrigin()) / r.getDirection();
    update(t_min, t_max, a.x(), b.x());
    update(t_min, t_max, a.y(), b.y());
    update(t_min, t_max, a.z(), b.z());
    if (t_min > t_max)
        return false;
    bool res = false;
    res |= o->intersect(r, h, tmin);
    if (ls != NULL)
        res |= ls->intersect(r, h, tmin);
    if (rs != NULL)
        res |= rs->intersect(r, h, tmin);
    return res;
}