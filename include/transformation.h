#ifndef __TRANSFORMATION__
#define __TRANSFORMATION__
#include "../include/geometry.h"

class Transformation {
public:
    Transformation() {}

    static Matrix viewPort(int width, int height, float depth);

    static Matrix modelView(Vec3f& eye, Vec3f& center, Vec3f& up);

    static Matrix projection(float eye_fov, float aspect_ratio, float zNear, float zFar);
};

#endif