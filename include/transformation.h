#ifndef __TRANSFORMATION__
#define __TRANSFORMATION__
#include "../include/geometry.h"
#include "../include/tgaimage.h"

class Transformation {
public:

    static Matrix viewPort(int width, int height);

    static Matrix modelView(Vec3f& eye, Vec3f& center, Vec3f& up);

    static Matrix projection(float eye_fov, float aspect_ratio, float zNear, float zFar);
    static void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
    static Vec3f barycentric(Vec3f P, Vec3f A, Vec3f B, Vec3f C);
    static bool isinside(Vec3f p, Vec3f t0, Vec3f t1, Vec3f t2);
};

#endif