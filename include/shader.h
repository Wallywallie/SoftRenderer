#ifndef __SHADER__
#define __SHADER__
#include "../include/geometry.h"
#include "../include/tgaimage.h"
#include "../include/model.h"
#include <vector>


struct Camera{


    float fov;
    float aspect_ratio;
    float zNear;
    float zFar;

	Vec3f eye_pos;
	Vec3f center;
	Vec3f up;    
    

	Camera(float f = 45, float ratio = 1, float near = 0.1, float far = 50, 
            Vec3f e = Vec3f(0, 0,3), Vec3f c = Vec3f(0, 0, 0), Vec3f u = Vec3f(0, 1, 0))
            : fov(f),aspect_ratio(ratio), zNear(near), zFar(far), eye_pos(e), center(c), up(u)
            {}

};


class Shader{//handle with only one triangle
public:

    //virtual ~Shader();
    static Matrix viewPort(int width, int height);
    static Matrix modelView(Vec3f& eye, Vec3f& center, Vec3f& up);
    static Matrix projection(float eye_fov, float aspect_ratio, float zNear, float zFar);
    static void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    static Vec3f barycentric(Vec3f P, Vec3f A, Vec3f B, Vec3f C);
    static bool isinside(Vec3f p, Vec3f t0, Vec3f t1, Vec3f t2);
    void set_transformation(Camera camera, int width, int height);

    virtual Vec3f vertex(Model* model, int ithFace, int jthVert) = 0;//从模型读顶点，返回顶点坐标
    virtual bool fragment(Vec3f baryCoor, TGAColor &color, TGAImage &tex) = 0; //处理插值
    void triangle(Vec3f *pts, Shader &shader, TGAImage &image, std::vector<float> &zbuffer, TGAImage& tex);
    
    

protected:
    Vec3f varying_intensity;//顶点着色器写入数据，片元着色器做插值,三角形每个顶点的法向量与光线方向的点乘
    Vec2f uv[3];
    Matrix transformation;
};


class FlatShading : public Shader {

public:    
    bool fragment(Vec3f baryCoor, TGAColor &color, TGAImage &tex) override; 
    Vec3f vertex(Model* model, int ithFace, int jthVert) override;
private:
    Vec3f world_coords[3];    
};

class GouraudShading : public Shader{

public:    
    bool fragment(Vec3f baryCoor, TGAColor &color, TGAImage &tex) override; 
    Vec3f vertex(Model* model, int ithFace, int jthVert) override;
};




class WireFrame : public Shader{

    
};


#endif