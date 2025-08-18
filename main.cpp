#include "tgaimage.h"
#include "model.h"
#include "geometrylix.h"

const TGAColor white = {255, 255, 255, 255};
const TGAColor red   = {0, 0, 255, 255};
const TGAColor green = {0, 255, 0, 255};
const TGAColor blue  = {255, 0, 0, 255};
Model* model = NULL;
const int width  = 800;
const int height = 800;
Vec3f light_dir(0,0,-1); // define light_dir

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
    Vec3f u = cross(Vec3f(p1.x - p0.x, p2.x - p0.x, p0.x - p.x), Vec3f(p1.y - p0.y, p2.y - p0.y, p0.y - p.y));
    return Vec3f(1 - (u.x + u.y) / u.z, u.x/u.z, u.y/u.z); 
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

void rasterize(Vec2i p0, Vec2i p1, TGAImage& image, TGAColor color, int ybuffer[])
{
    if (p0.x > p1.x) std::swap(p0, p1);

    for (int x=p0.x; x<=p1.x; x++)
    {
        float t = (x-p0.x)/(float)(p1.x-p0.x);
        int y = p0.y*(1.0-t) + p1.y*t;
        if (ybuffer[x] < y)
        {
            ybuffer[x] = y;
            image.set(x, 0, color);
        }
    }
}

Vec3f barycentric_v2(Vec3f p0, Vec3f p1, Vec3f p2, Vec3f p)
{
    Vec3f u = cross(Vec3f(p1.x - p0.x, p2.x - p0.x, p0.x - p.x), Vec3f(p1.y - p0.y, p2.y - p0.y, p0.y - p.y));
    return Vec3f(1 - (u.x + u.y) / u.z, u.x/u.z, u.y/u.z); 
}

void triangle_v3(Vec3f p0, Vec3f p1, Vec3f p2, float* zbuffer, TGAImage& image, TGAColor color)
{
    Vec2f minBox(image.width()-1,  image.height()-1);
    Vec2f maxBox(0, 0);
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

    Vec3f p;
    for (p.x=minBox.x; p.x<=maxBox.x; p.x++)
    {
        for (p.y=minBox.y; p.y<=maxBox.y; p.y++)
        {
            Vec3f bc = barycentric_v2(p0, p1, p2, p);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            p.z = p0.z*bc.x + p1.z*bc.y + p2.z*bc.z;
            if (zbuffer[int(p.x + p.y*width)] < p.z)
            {
                zbuffer[int(p.x + p.y*width)] = p.z;
                image.set(p.x, p.y, color);
            }
        }
    }
}

void triangle_v4(Vec3f* ps, Vec2f* uvs, float* zbuffer, TGAImage& image)
{
    Vec2f minBox(image.width()-1,  image.height()-1);
    Vec2f maxBox(0, 0);
    minBox.x = std::min(minBox.x, ps[0].x);
    minBox.x = std::min(minBox.x, ps[1].x);
    minBox.x = std::min(minBox.x, ps[2].x);
    minBox.y = std::min(minBox.y, ps[0].y);
    minBox.y = std::min(minBox.y, ps[1].y);
    minBox.y = std::min(minBox.y, ps[2].y);
    
    maxBox.x = std::max(maxBox.x, ps[0].x);
    maxBox.x = std::max(maxBox.x, ps[1].x);
    maxBox.x = std::max(maxBox.x, ps[2].x);
    maxBox.y = std::max(maxBox.y, ps[0].y);
    maxBox.y = std::max(maxBox.y, ps[1].y);
    maxBox.y = std::max(maxBox.y, ps[2].y);

    Vec2f uv;
    Vec3f p;
    for (p.x=minBox.x; p.x<=maxBox.x; p.x++)
    {
        for (p.y=minBox.y; p.y<=maxBox.y; p.y++)
        {
            Vec3f bc = barycentric_v2(ps[0], ps[1], ps[2], p);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;

            p.z = ps[0].z*bc.x + ps[1].z*bc.y + ps[2].z*bc.z;

            uv = uvs[0]*bc.x + uvs[1]*bc.y + uvs[2]*bc.z;

            if (zbuffer[int(p.x + p.y*width)] < p.z)
            {
                zbuffer[int(p.x + p.y*width)] = p.z;
                TGAColor color = model->diffuse(uv);
                image.set(p.x, p.y, color);
            }
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
        TGAColor color = {
            static_cast<uint8_t>(rand() % 255),
            static_cast<uint8_t>(rand() % 255),
            static_cast<uint8_t>(rand() % 255),
            static_cast<uint8_t>(255)  // 255也需要转换（虽然字面量允许，但保持一致性）
        };
        triangle_v2(screen_coords[0], screen_coords[1], screen_coords[2], image, color);
    }
    image.write_tga_file("head_v2.tga");
    delete model;
}

void draw_head_v3()
{
    TGAImage image(width, height, TGAImage::RGB);

    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        Vec2i screen_coords[3]; 
        Vec3f world_coords[3]; 
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            world_coords[j] = v0;
            screen_coords[j] = Vec2i((v0.x+1)*width/2, (v0.y+1)*height/2); 
        }

        // 计算三角面的法线，注意：顶点是逆时针顺序
        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        // Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        normalized(n);

        float intensity = n*light_dir;
        if (intensity > 0)
        {
            TGAColor color = {
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(255)  // 255也需要转换（虽然字面量允许，但保持一致性）
            };
            triangle_v2(screen_coords[0], screen_coords[1], screen_coords[2], image, color);
        }
    }
    image.write_tga_file("head_v2.tga");
    delete model;
}

void draw_head_v4()
{
    TGAImage image(width, height, TGAImage::RGB);
    float* zbuffer = new float[height*width];
    for (int i=0; i<height*width; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        Vec3f screen_coords[3]; 
        Vec3f world_coords[3]; 
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            world_coords[j] = v0;
            // 这里必须转成int
            screen_coords[j] = Vec3f((int)((v0.x+1.)*width/2.), (int)((v0.y+1.)*height/2.), v0.z);
        }

        // 计算三角面的法线，注意：顶点是逆时针顺序
        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        // Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        normalized(n);

        float intensity = n * light_dir;
        if (intensity > 0)
        {
            TGAColor color = {
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(intensity * 255),
                static_cast<uint8_t>(255)  // 255也需要转换（虽然字面量允许，但保持一致性）
            };
            triangle_v3(screen_coords[0], screen_coords[1], screen_coords[2], zbuffer, image, color);
        }
    }
    image.write_tga_file("head_v4.tga");
    delete model;
}

void draw_head_v5()
{
    TGAImage image(width, height, TGAImage::RGB);
    float* zbuffer = new float[height*width];
    for (int i=0; i<height*width; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    TGAImage diffuse;
    diffuse.read_tga_file("../obj/african_head_diffuse.tga");
    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        Vec3f uv_coords[3];
        Vec3f screen_coords[3]; 
        Vec3f world_coords[3]; 
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            world_coords[j] = v0;
            // 这里必须转成int
            screen_coords[j] = Vec3f((int)((v0.x+1.)*width/2.), (int)((v0.y+1.)*height/2.), v0.z);
        }

        // 计算三角面的法线，注意：顶点是逆时针顺序
        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        // Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        normalized(n);

        float intensity = n * light_dir;
        if (intensity > 0)
        {
            Vec2f uvfs[3];
            for (int k=0; k<3; k++) {
                uvfs[k] = model->uv(i,k);
            }
            triangle_v4(screen_coords, uvfs, zbuffer, image);
        }
    }
    image.write_tga_file("head_v5.tga");
    delete model;
}

mat<4,4,float> view(Vec3i pos, Vec3i at, Vec3i up)
{
    Vec3i z = normalized(pos - at);
    Vec3i x = normalized(cross(up, z));
    Vec3i y = normalized(cross(z, x));

    return mat<4,4,float>({{x.x, x.y, x.z, 0}, {y.z, y.y, y.z, 0}, {z.x, z.y, z.z, 0}, {0,0,0,1}}) * mat<4,4,float>({{1,0,0,-pos.x}, {0,1,0,-pos.y}, {0,0,1,-pos.z}, {0,0,0,1}});
}

mat<4,4,float> projection(const float f) { // check https://en.wikipedia.org/wiki/Camera_matrix
    return {{{1,0,0,0}, {0,-1,0,0}, {0,0,1,0}, {0,0,-1/f,0}}};
}

mat<4,4,float> viewport(const int x, const int y, const int w, const int h) {
    return {{{w/2., 0, 0, x+w/2.}, {0, h/2., 0, y+h/2.}, {0,0,1,0}, {0,0,0,1}}};
}

void draw_head_v6()
{
    TGAImage image(width, height, TGAImage::RGB);
    float* zbuffer = new float[height*width];
    for (int i=0; i<height*width; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    // 相机位置
    Vec3i camera_pos(1, 1, 3);
    Vec3i camera_at(0, 0, 0);       // 从 (1, 1, 3) 看向 (0, 0, 0)
    Vec3i camera_up(0, 1, 0);

    mat<4,4,float> modelview = view(camera_pos, camera_at, camera_up);
    mat<4,4,float> _projection = projection(-1.f/norm(camera_pos-camera_at));
    mat<4,4,float> _viewport = viewport(width/8, height/8, width*3/4, height*3/4);

    TGAImage diffuse;
    diffuse.read_tga_file("../obj/african_head_diffuse.tga");
    model = new Model("../obj/african_head.obj");
    for (int i=0; i<model->nfaces(); i++)
    {
        Vec3f uv_coords[3];
        Vec3f screen_coords[3]; 
        Vec3f world_coords[3]; 
        std::vector<int> face = model->face(i);
        for (int j=0; j<3; j++)
        {
            Vec3f v0 = model->vert(face[j]);
            world_coords[j] = v0;
            // 这里必须转成int
            //screen_coords[j] = Vec3f((int)((v0.x+1.)*width/2.), (int)((v0.y+1.)*height/2.), v0.z);
            // Vec4f v = _viewport*_projection*modelview*Vec4f(v0.x, v0.y, v0.z, 1);
            Vec4f v = _viewport*Vec4f(v0.x, v0.y, v0.z, 1);
            Vec3f vv = (v/v.w).xyz();
            screen_coords[j] = {(int)vv.x, (int)vv.y, vv.z};
        }

        // 计算三角面的法线，注意：顶点是逆时针顺序
        Vec3f n = cross((world_coords[2] - world_coords[0]), (world_coords[1] - world_coords[0]));
        // Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
        normalized(n);

        float intensity = n * light_dir;
        if (intensity > 0)
        {
            Vec2f uv[3];
            for (int k=0; k<3; k++) {
                uv[k] = model->uv(i, k);
            }
            triangle_v4(screen_coords, uv, zbuffer, image);
        }
    }
    image.write_tga_file("head_v6.tga");
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

void draw_test3()
{
    TGAImage render(width, 16, TGAImage::RGB);
    int ybuffer[width];
    for (int i=0; i<width; i++) {
        ybuffer[i] = std::numeric_limits<int>::min();
    }
    rasterize(Vec2i(20, 34),   Vec2i(744, 400), render, red,   ybuffer);
    rasterize(Vec2i(120, 434), Vec2i(444, 400), render, green, ybuffer);
    rasterize(Vec2i(330, 463), Vec2i(594, 200), render, blue,  ybuffer);
    render.write_tga_file("test3.tga");
}

int main(int argc, char** argv)
{
    draw_head_v6();
}