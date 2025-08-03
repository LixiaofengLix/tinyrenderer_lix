#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {0, 0, 255, 255};
Model* model = NULL;
const int width  = 800;
const int height = 800;

void line_v1(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    for (float t=0; t<1.0; t+=0.01)
    {
        int x = x0 + (x1 - x0) * t;
        int y = y0 + (y1 - y0) * t;
        image.set(x, y, color);
    }
}

void line_v2(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    float t = (y1 - y0) / (float)(x1 - x0);
    for (int x = 0; x<(x1-x0); x+=1)
    {
        int y = y0 + (t * x);
        image.set(x0+x, y, color);
    }
}

void line_v3(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    if (x0 > x1)    // make it left-to-right
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    bool steep = false; 
    if ((x1 - x0) < (y1 - y0))  // if the line is steep, we transpose the image
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    float t = (y1 - y0) / (float)(x1 - x0);
    for (int x = 0; x<(x1-x0); x+=1)
    {
        int y = y0 + (t * x);
        if (steep)
            image.set(y, x0+x, color);  // if transposed, de-transpose
        else
            image.set(x0+x, y, color); 
    }
}

void line_v4(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    if (x0 > x1)    // make it left-to-right
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    bool steep = false; 
    if ((x1 - x0) < (y1 - y0))  // if the line is steep, we transpose the image
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    float dy = 0;
    int y = y0;
    float t = (y1 - y0) / (float)(x1 - x0);
    for (int x = 0; x<(x1-x0); x+=1)
    {
        // int y = y0 + (t * x);
        if (steep)
            image.set(y, x0+x, color);  // if transposed, de-transpose
        else
            image.set(x0+x, y, color);
        
        dy += t;
        if (dy > 0.5)
        {
            y += (y0 < y1) ? 1 : -1;
            dy -= 1;
        }
    }
}

void line_v5(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    bool steep = false; 
    if (std::abs(x1-x0) < std::abs(y1-y0))  // if the line is steep, we transpose the image
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if (x0 > x1)    // make it left-to-right
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    int dx = x1 - x0;
    int dy = y1 - y0;
    int y = y0;
    int derror = std::abs(dy) * 2;
    int error = 0;
    for (int x = 0; x<dx; x+=1)
    {
        // int y = y0 + (t * x);
        if (steep)
            image.set(y, x0+x, color);  // if transposed, de-transpose
        else
            image.set(x0+x, y, color);
        
        error += derror;
        if (error > dx)
        {
            y += (y0 < y1) ? 1 : -1;
            error -= dx * 2;
        }
    }
}

void draw_head()
{
    TGAImage image(width, height, TGAImage::RGB);

    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            Vec3f v1 = model->vert(face[(j+1)%3]);

            int x0 = (v0.x+1)*width/2;      // (v0.x)*width/2 + width/2
            int y0 = (v0.y+1)*height/2;     // (v0.y)*height/2 + height/2
            int x1 = (v1.x+1)*width/2;
            int y1 = (v1.y+1)*height/2;

            line_v5(x0, y0, x1, y1, image, white);
        }
    }
    image.write_tga_file("head.tga");
    delete model;
}

void draw_test() {

    TGAImage image(100, 100, TGAImage::RGB);

    line_v5(50, 50, 100, 50, image, white);
    line_v5(50, 50, 100, 75, image, red);
    line_v5(50, 50, 100, 100, image, white);
    line_v5(50, 50, 0, 25, image, red);

    line_v5(50, 50, 50, 100, image, white);
    line_v5(50, 50, 75, 100, image, red);
    line_v5(50, 50, 0, 100, image, white);
    line_v5(50, 50, 25, 0, image, red);

    line_v5(50, 50, 0, 50, image, white);
    line_v5(50, 50, 25, 100, image, red);
    line_v5(50, 50, 0, 0, image, white);
    line_v5(50, 50, 75, 0, image, red);

    line_v5(50, 50, 50, 0, image, white);
    line_v5(50, 50, 0, 75, image, red);
    line_v5(50, 50, 100, 0, image, white);
    line_v5(50, 50, 100, 25, image, red);

    image.write_tga_file("test.tga");
}

int main(int argc, char** argv)
{
    draw_head();
    draw_test();
}