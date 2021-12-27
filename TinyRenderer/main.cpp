//
//  main.cpp
//  TinyRenderer
//
//  Created by Sherwin Rad on 12/27/21.
//

#include "tgaimage.h"
#include "model.h"

const TGAColor white = TGAColor(255,255,255,255);
const TGAColor red = TGAColor(255,0,0,255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if(std::abs(x0-x1) < std::abs(y0-y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    
    if(x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    
    int dx = x1-x0;
    int dy = y1-y0;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = y0;
    
    if(steep)
    {
        for(int x = x0; x<=x1; x++)
        {
            image.set(y, x, color);
            error2 += derror2;
            if(error2 > dx)
            {
                y += (y1>y0 ? 1: -1);
                error2 -= dx*2;
            }
        }
    }
    else
    {
        for(int x = x0; x<=x1; x++)
        {
            image.set(x, y, color);
            error2 += derror2;
            if(error2 > dx)
            {
                y += (y1>y0 ? 1: -1);
                error2 -= dx*2;
            }
        }
    }
}

int main(int argc, const char * argv[]) {
    if (argc==2)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("/Users/radsherwin/Documents/Xcode/TinyRenderer/TinyRenderer/Models/african_head.obj");
    }
    
    TGAImage image(width, height, TGAImage::RGB);
    for(int i = 0; i < model->nFaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for(int j = 0; j < 3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);
            int x0 = (v0.x + 1) * width/2;
            int y0 = (v0.y + 1) * height/2;
            int x1 = (v1.x + 1) * width/2;
            int y1 = (v1.y + 1) * height/2;
            line(x0, y0, x1, y1, image, white);
            
        }
    }
    
    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
    
}
