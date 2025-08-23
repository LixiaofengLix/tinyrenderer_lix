#ifndef __GL_H__
#define __GL_H__
#include "tgaimage.h"
#include "geometrylix.h"

mat<4,4,float> ModelView;
mat<4,4,float> Projection;
mat<4,4,float> Viewport;

void set_modelview(Vec3f camera_pos, Vec3f center, Vec3f up) {
    Vec3f z = normalized(camera_pos - center);
    Vec3f x = normalized(cross(up, z));
    Vec3f y = normalized(cross(z, x));

    ModelView = mat<4,4,float>({{x.x, x.y, x.z, 0}, {y.z, y.y, y.z, 0}, {z.x, z.y, z.z, 0}, {0,0,0,1}}) * mat<4,4,float>({{1,0,0,-camera_pos.x}, {0,1,0,-camera_pos.y}, {0,0,1,-camera_pos.z}, {0,0,0,1}});
}

void set_projection(float coeff) {
    Projection = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0,coeff,1}}};
}

void set_viewport(int x, int y, int w, int h) {
    Viewport = {{{w/2.f, 0, 0, x+w/2.f}, {0, h/2.f, 0, y+h/2.f}, {0,0,1,0}, {0,0,0,1}}};
}

struct IShader {
    // virtual ~IShader();
    // 输入顶点模型坐标；返回屏幕坐标；顶点着色器的主要目标是变换顶点的坐标，次要目标是为片段着色器准备数据
    virtual Vec4f vertex(Vec3f vert, Vec3f normal, int ivert);     
    // 片段着色器的主要目标是确定当前像素的颜色，次要目标是我们可以通过返回 true 来丢弃当前像素        
    virtual bool fragment(Vec3f bc, Vec2f uv, TGAColor &color);
};

Vec3f barycentric(Vec4f p0, Vec4f p1, Vec4f p2, Vec3i p)
{
    Vec3f u = cross(Vec3f(p1.x - p0.x, p2.x - p0.x, p0.x - p.x), Vec3f(p1.y - p0.y, p2.y - p0.y, p0.y - p.y));

    return Vec3f(1 - (u.x + u.y) / u.z, u.x/u.z, u.y/u.z); 
}

void triangle(Vec4f *pts, Vec2f* uvs, IShader &shader, TGAImage &image, float* zbuffer) {
    // 三角面包围盒
    Vec2f minBox(image.width()-1,  image.height()-1);
    Vec2f maxBox(0, 0);
    minBox.x = std::min(minBox.x, pts[0].x);
    minBox.x = std::min(minBox.x, pts[1].x);
    minBox.x = std::min(minBox.x, pts[2].x);
    minBox.y = std::min(minBox.y, pts[0].y);
    minBox.y = std::min(minBox.y, pts[1].y);
    minBox.y = std::min(minBox.y, pts[2].y);
    
    maxBox.x = std::max(maxBox.x, pts[0].x);
    maxBox.x = std::max(maxBox.x, pts[1].x);
    maxBox.x = std::max(maxBox.x, pts[2].x);
    maxBox.y = std::max(maxBox.y, pts[0].y);
    maxBox.y = std::max(maxBox.y, pts[1].y);
    maxBox.y = std::max(maxBox.y, pts[2].y);

    Vec2f uv;
    Vec3i p;
    TGAColor color;
    for (p.x=minBox.x; p.x<=maxBox.x; p.x++)
    {
        for (p.y=minBox.y; p.y<=maxBox.y; p.y++)
        {
            Vec3f bc = barycentric(pts[0], pts[1], pts[2], p);
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) continue;         // 跳过在三角面外的像素

            float z = pts[0].z*bc.x + pts[1].z*bc.y + pts[2].z*bc.z;    
            float w = pts[0].w*bc.x + pts[1].w*bc.y + pts[2].w*bc.z;   
            int depth = z/w;
            if (zbuffer[int(p.x+ p.y*image.width())] > z) continue;

            uv = uvs[0]*bc.x + uvs[1]*bc.y + uvs[2]*bc.z;
            bool discard = shader.fragment(bc, uv, color);
            if (!discard) {
                zbuffer[int(p.x+ p.y*image.width())] = z;
                image.set(p.x, p.y, color);
            }
        }
    }
}

#endif //__GL_H__