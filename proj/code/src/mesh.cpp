#include "mesh.hpp"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <utility>
#include <sstream>

bool Mesh::intersect(const Ray &r, Hit &h, double tmin)
{
    if (V.size() <= 0)
    {
        bool res = false;
        for (auto obj : V)
            res |= obj->intersect(r, h, tmin);
        return res;
    }
    else
        return rt->intersect(r, h, tmin);
}

Mesh::Mesh(const char *filename, Material *material) : Object3D(material)
{

    // Optional: Use tiny obj loader to replace this simple one.
    std::ifstream file;
    file.open(filename);
    if (!file.is_open())
    {
        std::cout << "Cannot open " << filename << "\n";
        return;
    }
    std::string line;
    std::string vTok("v");
    std::string fTok("f");
    std::string texTok("vt");
    char bslash = '/', space = ' ';
    std::string tok;
    while (true)
    {
        std::getline(file, line);
        if (file.eof())
        {
            break;
        }
        if (line.size() < 3)
        {
            continue;
        }
        if (line.at(0) == '#')
        {
            continue;
        }
        std::stringstream ss(line);
        ss >> tok;
        if (tok == vTok)
        {
            Vector3f vec;
            ss >> vec[0] >> vec[1] >> vec[2];
            v.push_back(vec);
            box.update(vec);
        }
        else if (tok == fTok)
        {
            if (line.find(bslash) != std::string::npos)
            {
                std::replace(line.begin(), line.end(), bslash, space);
                std::stringstream facess(line);
                TriangleIndex trig;
                TriangleIndex texID;
                facess >> tok;
                for (int ii = 0; ii < 3; ii++)
                {
                    facess >> trig[ii] >> texID[ii];
                    trig[ii]--;
                }
                f.push_back(trig);
                ft.push_back(texID);
            }
            else
            {
                TriangleIndex trig;
                for (int ii = 0; ii < 3; ii++)
                {
                    ss >> trig[ii];
                    trig[ii]--;
                }
                f.push_back(trig);
            }
        }
        else if (tok == texTok)
        {
            Vector2f texcoord;
            ss >> texcoord[0];
            ss >> texcoord[1];
            vt.push_back(texcoord);
        }
    }

    file.close();

    V.resize(f.size());
    for (int triId = 0; triId < (int)f.size(); ++triId)
        if (ft.size() == f.size())
        {
            TriangleIndex &triIndex = f[triId];
            TriangleIndex &texIndex = ft[triId];
            V[triId] = new Triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], vt[texIndex[0]], vt[texIndex[1]], vt[texIndex[2]], material);
        }
        else
        {
            TriangleIndex &triIndex = f[triId];
            V[triId] = new Triangle(v[triIndex[0]], v[triIndex[1]], v[triIndex[2]], material);
        }

    rt = new KDTree(V, 0, (int)V.size(), 0);
}
