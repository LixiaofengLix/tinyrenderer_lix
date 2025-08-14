#include "gl.h"
#include "model.h"

Model *model     = NULL;
const int width  = 800;
const int height = 800;

Vec3i  light_dir(1,1,1);
Vec3f camera_pos(1,1,3);
Vec3f     center(0,0,0);
Vec3f         up(0,1,0);

struct GouraudShader : public IShader {
    Vec3f varying_intensity;

    virtual Vec4f vertex(Vec3f vert, Vec3f normal) {
        Vec4f v(vert.x, vert.y, vert.z, 1);
        return Viewport*Projection*ModelView*v;
    }

    virtual bool fragment(Vec2f uvf, TGAColor &color) {
        color = model->diffuse(uvf);
        return false;
    }
};

int main(int argc, char** argv) {

    if (argc == 2) {
        model = new Model(argv[1]);
    } else {
        model = new Model("../obj/african_head.obj");
    }

    set_modelview(camera_pos, center, up);                      // TODO 视图矩阵推导
    set_projection(-1.f/norm(camera_pos-center));               // TODO 透视矩阵推导
    set_viewport(width/8, height/8, width*3/4, height*3/4);     // TODO 视口矩阵推导

    light_dir = normalized(light_dir);

    TGAImage image(width, height, TGAImage::RGB);
    float* zbuffer = new float[height*width];
    for (int i=0; i<height*width; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    GouraudShader shader;
    for (int i=0; i<model->nfaces(); i++) {                 // 遍历三角面
        Vec4f screen_coords[3];
        for (int j=0; j<3; j++) {                           // 遍历三角面顶点
            screen_coords[j] = shader.vertex(               // 返回屏幕坐标
                model->vert(model->face(i)[j]),
                model->normal(i, j)
            );           
        }

        Vec2f uvfs[3];
        for (int k=0; k<3; k++) {
            uvfs[k] = model->uv(i, k);
        }
        triangle(screen_coords, uvfs, shader, image, zbuffer);    // 光栅化
    }

    image.flip_vertically();
    image.write_tga_file("out.tga");

    delete model;
    return 0;
}