//
//  model.cpp
//  TinyRenderer
//
//  Created by Sherwin Rad on 12/27/21.
//

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_()
{
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v "))
        {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "f "))
        {
            std::vector<int> f;
            int itrash, idx;
            iss >> trash;
            while (iss >> idx >> trash >> itrash >> trash >> itrash)
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);
            }
            faces_.push_back(f);
        }
        else if (!line.compare(0, 2, "vt"))
        {
            iss >> trash;
            iss >> trash;
            Vec2f vt;
            for(int i = 0; i < 2; i++)
            {
                iss >> vt[i];
            }
            texCoords_.push_back(vt);
        }
    }
    std::cerr << "vt: " << texCoords_.size() <<" v: " << verts_.size() << " f: "  << faces_.size() << std::endl;
}

Model::~Model()
{
}

int Model::nVerts()
{
    return (int)verts_.size();
}

int Model::nTexCoords()
{
    return (int)texCoords_.size();
}

int Model::nFaces()
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx)
{
    return faces_[idx];
}

Vec3f Model::vert(int i)
{
    return verts_[i];
}

Vec2f Model::texCoords(int i)
{
    return texCoords_[i];
}

