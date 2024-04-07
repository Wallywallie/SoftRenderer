#ifndef __TRANSFORMATION__
#define __TRANSFORMATION__
#include "../include/geometry.h"
#include <vector>

const float PI = 3.1415926535;

class Transformation{
public:
    Transformation(){}

    //bi-unit cube [-1,1]*[-1,1]*[-1,1] is mapped onto the screen cube [x,x+w]*[y,y+h]*[0,d].
    static Matrix viewPort(int width, int height, float depth){
        Matrix viewPort = Matrix::identity(4);
        //trans
        viewPort[0][3] = width / (float)2; 
        viewPort[1][3] = height / (float)2;
        viewPort[2][3] = depth / 2;

        //scale
        viewPort[0][0] = width/(float)2;
        viewPort[1][1] = height / (float)2;
        viewPort[2][2] = depth / 2;
        return viewPort;

    }


    //modelView Matrix
    //eye_position  look_at_direction  up_direction
    //up_direction is not y_axis, but a vector to define the plane 
    static Matrix modelView(Vec3f& eye, Vec3f& center, Vec3f& up) {
        Vec3f z_axis = (center - eye).normalize();
        Vec3f x_axis = (z_axis^up).normalize();
        Vec3f y_axis = (x_axis^z_axis).normalize();
        Matrix trans = Matrix::identity(4);
        Matrix rotate = Matrix::identity(4);
        Matrix modelView = Matrix::identity(4);
        for (int i = 0; i < 3; i++) {
            rotate[i][0] = x_axis.x;
            rotate[i][1] = x_axis.y;
            rotate[i][2] = x_axis.z;

            trans[i][3] = -eye[i];
        }
        //旋转矩阵的特性：逆矩阵为矩阵的转置
        rotate.transpose();
        Matrix modelView = rotate * trans * modelView;
        return modelView;
    }


    //projection matrix
    //aspect_ratio = width / height
    //TODO: zfar-znear分别代表什么？
    //注意这里使用的是左手系，屏幕里是正值，越远值越大
    static Matrix projection(float eye_fov, float aspect_ratio, float zNear, float zFar){
        Matrix pers_ortho(4,4);
        Matrix orthro = Matrix::identity(4);
        Matrix projection = Matrix::identity(4);
        float angle = eye_fov /(float)180 * PI;//角度转弧度
        float height = 2 * zNear * std::tan(angle / 2);
        float width = height * aspect_ratio;

        pers_ortho[0][0] = zNear;
        pers_ortho[1][1] = zNear;
        pers_ortho[2][2] = zNear + zFar;
        pers_ortho[2][3] = -zFar * zNear;
        pers_ortho[3][2] = 1;

        Matrix orthro_trans = Matrix::identity(4);
        Matrix orthro_scale = Matrix::identity(4);
        orthro_trans[3][2] = -(zNear + zFar) / (float)2;

        orthro_scale[0][0] = 2 / width;
        orthro_scale[1][1] = 2 / height;
        orthro_scale[2][2] = 2 / (zFar - zNear);

        orthro = orthro_scale  * orthro_trans;

        projection = orthro * pers_ortho * projection;
        return projection;

    }

};

#endif __TRANSFORMATION__