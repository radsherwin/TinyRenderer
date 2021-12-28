//
//  model.h
//  TinyRenderer
//
//  Created by Sherwin Rad on 12/27/21.
//

#ifndef model_h
#define model_h

#include <vector>
#include "geometry.h"

class Model
{
private:
    std::vector<Vec3f> verts_;
    std::vector<Vec2f> texCoords_;
    std::vector<std::vector<int>> faces_;
public:
    Model(const char* const fileName);
    Model(const Model&) =delete;
    Model& operator=(const Model&) = delete;
    Model(Model&&) = delete;
    Model& operator=(Model&&) = delete;
    ~Model();
    
    int nVerts();
    int nTexCoords();
    int nFaces();
    Vec3f vert(int i);
    Vec2f texCoords(int i);
    std::vector<int> face(int idx);
};


#endif /* model_h */
