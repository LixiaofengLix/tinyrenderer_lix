#include "tgaimage.h"
#include "model.h"
#include "geometry.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {0, 0, 255, 255};
const TGAColor green   = {0, 255, 0, 255};
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

void line(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color)
{
    bool steep = false; 
    if (std::abs(p1.x-p0.x) < std::abs(p1.y-p0.y))  // if the line is steep, we transpose the image
    {
        std::swap(p0.x, p0.y);
        std::swap(p1.x, p1.y);
        steep = true;
    }

    if (p0.x > p1.x)    // make it left-to-right
    {
        std::swap(p0, p1);
    }

    int dx = p1.x - p0.x;
    int dy = p1.y - p0.y;
    int y = p0.y;
    int derror = std::abs(dy) * 2;
    int error = 0;
    for (int x = 0; x<dx; x+=1)
    {
        // int y = y0 + (t * x);
        if (steep)
            image.set(y, p0.x+x, color);  // if transposed, de-transpose
        else
            image.set(p0.x+x, y, color);
        
        error += derror;
        if (error > dx)
        {
            y += (dy > 0) ? 1 : -1;
            error -= dx * 2;
        }
    }
}

void triangle_v1(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    if (p0.y > p1.y)
        std::swap(p0, p1);
    if (p1.y > p2.y)
        std::swap(p1, p2);
    if (p0.y > p1.y)
        std::swap(p0, p1);
    
    int max_height = p2.y - p0.y;

    for (int y=p0.y; y<p1.y; y++)
    {
        int segment_height = p1.y - p0.y + 1;
        float a = (float)(y - p0.y) / segment_height;
        Vec2i A = p0 + ((p1 - p0) * a);
        image.set(A.x, A.y, green);

        float b = (float)(y - p0.y) / max_height;
        Vec2i B = p0 + ((p2 - p0) * b);
        image.set(B.x, B.y, red);

        if (A.x > B.x) std::swap(A, B);
        for (int x=A.x; x<B.x+1; x++)
        {
            image.set(x, y, color);
        }
    }

    for (int y=p1.y; y<p2.y; y++)
    {
        int segment_height = p2.y - p1.y + 1;
        float a = (float)(y - p1.y) / segment_height;
        Vec2i A = p1 + ((p2 - p1) * a);
        image.set(A.x, A.y, green);

        float b = (float)(y - p0.y) / max_height;
        Vec2i B = p0 + ((p2 - p0) * b);
        image.set(B.x, B.y, red);

        if (A.x > B.x) std::swap(A, B);
        for (int x=A.x; x<B.x+1; x++)
        {
            image.set(x, y, color);
        }
    }
}

Vec3f barycentric(Vec2i p0, Vec2i p1, Vec2i p2, Vec2i p)
{
    Vec3f u = Vec3f(p1.x - p0.x, p2.x - p0.x, p0.x - p.x) ^ Vec3f(p1.y - p0.y, p2.y - p0.y, p0.y - p.y);
    return Vec3f(u.x/u.z, u.y/u.z, 1 - (u.x + u.y) / u.z); 
}

void triangle_v2(Vec2i p0, Vec2i p1, Vec2i p2, TGAImage& image, TGAColor color)
{
    Vec2i minBox(image.width()-1,  image.height()-1);
    Vec2i maxBox(0, 0);
    minBox.x = std::min(minBox.x, p0.x);
    minBox.x = std::min(minBox.x, p1.x);
    minBox.x = std::min(minBox.x, p2.x);
    minBox.y = std::min(minBox.y, p0.y);
    minBox.y = std::min(minBox.y, p1.y);
    minBox.y = std::min(minBox.y, p2.y);
    
    maxBox.x = std::max(maxBox.x, p0.x);
    maxBox.x = std::max(maxBox.x, p1.x);
    maxBox.x = std::max(maxBox.x, p2.x);
    maxBox.y = std::max(maxBox.y, p0.y);
    maxBox.y = std::max(maxBox.y, p1.y);
    maxBox.y = std::max(maxBox.y, p2.y);

    Vec2i p;
    for (p.x=minBox.x; p.x<=maxBox.x; p.x++)
    {
        for (p.y=minBox.y; p.y<=maxBox.y; p.y++)
        {
            Vec3f bc = barycentric(p0, p1, p2, p);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;
            image.set(p.x, p.y, color);
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

void draw_head_v2()
{
    TGAImage image(width, height, TGAImage::RGB);

    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        Vec2i screen_coords[3]; 
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            screen_coords[j] = Vec2i((v0.x+1)*width/2, (v0.y+1)*height/2); 
        }
        TGAColor color = {rand()%255, rand()%255, rand()%255, 255};
        triangle_v2(screen_coords[0], screen_coords[1], screen_coords[2], image, color);
    }
    image.write_tga_file("head_v2.tga");
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
    TGAImage image(200, 200, TGAImage::RGB);
    Vec2i t0[3] = {Vec2i(10, 70),   Vec2i(50, 160),  Vec2i(70, 80)}; 
    Vec2i t1[3] = {Vec2i(180, 50),  Vec2i(150, 1),   Vec2i(70, 180)}; 
    Vec2i t2[3] = {Vec2i(180, 150), Vec2i(120, 160), Vec2i(130, 180)}; 
    triangle_v2(t0[0], t0[1], t0[2], image, red); 
    triangle_v2(t1[0], t1[1], t1[2], image, white); 
    triangle_v2(t2[0], t2[1], t2[2], image, green);

    image.write_tga_file("triangle_v2.tga");

    draw_head_v2();
}