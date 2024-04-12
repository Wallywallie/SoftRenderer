#ifndef __SHADER__
#define __SHADER__
#include "../include/geometry.h"
#include "../include/tgaimage.h"


class Shader{
public:
    virtual Vec3f vertex();

    virtual Vec3f fragment();
    virtual void triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy, int width);

};


class FlatShading : public Shader {
    
    virtual Vec3f vertex() override;
    void triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy, int width) override;

};

class GouraudShading : public Shader{

};

class PhongShading : public Shader{

};

class WireFrame : public Shader{

    
};


#endif