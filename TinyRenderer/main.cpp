//
//  main.cpp
//  TinyRenderer
//
//  Created by Sherwin Rad on 12/27/21.
//

#include "tgaimage.h"
#include "model.h"
#include <cstdlib>
#include <limits>

const TGAColor white = TGAColor(255,255,255,255);
const TGAColor red = TGAColor(255,0,0,255);
const TGAColor green = TGAColor(0, 255, 0, 255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

//Vec3f cross(Vec3f &a, Vec3f &b)
//{
//    return Vec3f(a.y * b.z - a.z*b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
//}

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if (std::abs(x0-x1)<std::abs(y0-y1))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0>x1)
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

Vec3f barycentric(Vec3f A, Vec3f B, Vec3f C, Vec3f P)
{
    Vec3f s[2];
    for (int i=2; i--; )
    {
        s[i][0] = C[i]-A[i];
        s[i][1] = B[i]-A[i];
        s[i][2] = A[i]-P[i];
    }
    Vec3f u = cross(s[0], s[1]);
    if (std::abs(u[2])>1e-2) // dont forget that u[2] is integer. If it is zero then triangle ABC is degenerate
        return Vec3f(1.f-(u.x+u.y)/u.z, u.y/u.z, u.x/u.z);
    return Vec3f(-1,1,1); // in this case generate negative coordinates, it will be thrown away by the rasterizator
}

void triangle(Vec3f *pts, float *zBuffer, TGAImage &image, TGAColor color)
{
    Vec2f bboxmin( std::numeric_limits<float>::max(),  std::numeric_limits<float>::max());
    Vec2f bboxmax(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
    Vec2f clamp(image.get_width()-1, image.get_height()-1);
    for (int i=0; i<3; i++)
    {
        for (int j=0; j<2; j++)
        {
            bboxmin[j] = std::max(0.f,      std::min(bboxmin[j], pts[i][j]));
            bboxmax[j] = std::min(clamp[j], std::max(bboxmax[j], pts[i][j]));
        }
    }
    Vec3f P;
    for (P.x=bboxmin.x; P.x<=bboxmax.x; P.x++)
    {
        for (P.y=bboxmin.y; P.y<=bboxmax.y; P.y++)
        {
            Vec3f bc_screen  = barycentric(pts[0], pts[1], pts[2], P);
            if (bc_screen.x<0 || bc_screen.y<0 || bc_screen.z<0) continue;
            P.z = 0;
            for (int i=0; i<3; i++)
            {
                P.z += pts[i][2]*bc_screen[i];
            }
            if (zBuffer[int(P.x+P.y*width)]<P.z)
            {
                zBuffer[int(P.x+P.y*width)] = P.z;
                image.set(P.x, P.y, color);
            }
        }
    }
    
}

Vec3f world2screen(Vec3f v)
{
    return Vec3f(int((v.x+1.f)*width/2.f+.5f), int((v.y+1.f)*height/2.f+.5f), v.z);
}

//Intensity of illumination is equal to the scalar product of the light vector and the normal to the given triangle
int main(int argc, const char * argv[]) {
    if (argc==2)
    {
        model = new Model(argv[1]);
    }
    else
    {
        model = new Model("/Users/radsherwin/Documents/Xcode/TinyRenderer/TinyRenderer/Models/african_head/african_head.obj");
    }
    
    float *zBuffer = new float[width*height];
    for(int i = width*height; i--; zBuffer[i] = -std::numeric_limits<float>::max());
    
    TGAImage image(width, height, TGAImage::RGB);
    for(int i = 0; i < model->nFaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec3f pts[3];
        Vec2f uv[3];
        for(int i = 0; i < 3; i++)
        {
            pts[i] = world2screen(model->vert(face[i]));
            uv[i] = model->texCoords(face[i]);
        }
        triangle(pts, zBuffer, image, TGAColor(rand()%255, rand()%255, rand()%255, 255));
    }
    
    
    image.flip_vertically(); //to set origin at the bottom left corner of the image
    image.write_tga_file("output.tga");
    delete model;
    return 0;
    
}
