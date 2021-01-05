#ifndef GROUP_H
#define GROUP_H

#include "object3d.hpp"
#include "ray.hpp"
#include "hit.hpp"
#include "kdtree.hpp"
#include <iostream>
#include <vector>

// ~~TODO~~: Implement Group - add data structure to store a list of Object*
class Group : public Object3D
{

public:
    Group() = delete;

    explicit Group(int num_objects)
    {
        V.resize(num_objects);
    }

    ~Group() override
    {
    }

    bool intersect(const Ray &r, Hit &h, float tmin) override
    {
        if (V.size() <= 20)
        {
            bool res = false;
            for (auto obj : V)
                res |= obj->intersect(r, h, tmin);
            return res;
        }
        else
            return rt->intersect(r, h, tmin);
    }

    void addObject(int index, Object3D *obj)
    {
        V[index] = obj;
    }

    int getGroupSize()
    {
        return V.size();
    }

    void buildKDTree()
    {
        rt = new KDTree(V, 0, (int)V.size(), 0);
    }

private:
    std::vector<Object3D *> V;
    KDTree *rt;
};

#endif
