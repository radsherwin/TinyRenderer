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
const TGAColor green = TGAColor(0, 255, 0, 255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

Vec3f cross(Vec3f &a, Vec3f &b)
{
    return Vec3f(a.y * b.z - a.z*b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

//void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color)
void line(Vec2i p0, Vec2i p1, TGAImage &image, TGAColor color)
{
    bool steep = false;
    if(std::abs(p0.x - p1.x) < std::abs(p0.y-p1.y))
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }
    
    if(p0.x > p1.x)
    {
        std::swap(p0, p1);
    }
    
    int dx = p1.x-p0.x;
    int dy = p1.y-p0.y;
    int derror2 = std::abs(dy)*2;
    int error2 = 0;
    int y = p0.y;
    
    if(steep)
    {
        for(int x = p0.x; x<=p1.x; x++)
        {
            image.set(y, x, color);
            error2 += derror2;
            if(error2 > dx)
            {
                y += (p1.y>p0.y ? 1: -1);
                error2 -= dx*2;
            }
        }
    }
    else
    {
        for(int x = p0.x; x<=p1.x; x++)
        {
            image.set(x, y, color);
            error2 += derror2;
            if(error2 > dx)
            {
                y += (p1.y>p0.y ? 1: -1);
                error2 -= dx*2;
            }
        }
    }
}

void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color)
{
    if(t0.y == t1.y && t0.y==t2.y) return;
    if(t0.y > t1.y) std::swap(t0, t1);
    if(t0.y > t2.y) std::swap(t0, t2);
    if(t1.y > t2.y) std::swap(t1, t2);
    int total_height = t2.y - t0.y;
    for(int i = 0; i < total_height; i++)
    {
        bool second_half = i > t1.y - t0.y || t1.y == t0.y;
        int segment_height = second_half ? t2.y-t1.y : t1.y-t0.y;
        float alpha = (float)i/total_height;
        float beta = (float)(i - (second_half ? t1.y - t0.y : 0))/segment_height;
        
        Vec2i A = t0 + (t2-t0)* alpha;
        Vec2i B = second_half ? t1+ (t2-t1)*beta : t0 + (t1-t0)*beta;
        
        if(A.x > B.x) std::swap(A,B);
        for(int j = A.x; j <= B.x; j++)
        {
            image.set(j, t0.y+i, color);
        }
    }
}
//Intensity of illumination is equal to the scalar product of the light vector and the normal to the given triangle
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
    Vec3f light_dir(0.f,0.f,-1.f);
    for(int i = 0; i < model->nFaces(); i++)
    {
        std::vector<int> face = model->face(i);
        Vec2i screen_coords[3];
        Vec3f world_coords[3];
        for(int j = 0; j < 3; j++)
        {
            Vec3f v = model->vert(face[j]);
            screen_coords[j] = Vec2i((v.x+1.f)*width/2.f, (v.y+1.f)*height/2.f);
            world_coords[j] = v;
        }
        Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); //Finding the normal by calculating the cross product of both sides
        n.normalize();
        float intensity = n * light_dir;
        if(intensity > 0)
        {
            triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255));
        }
        
    }
    
    image.flip_vertically();
    image.write_tga_file("output.tga");
    delete model;
    return 0;
    
}
