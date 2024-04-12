#include "../include/transformation.h"
#include "../include/geometry.h"
#include "../include/tgaimage.h"
#include <vector>
#include <iostream>

const float PI = 3.1415926535;

//bi-unit cube [-1,1]*[-1,1]*[-1,1] is mapped onto the screen cube [x,x+w]*[y,y+h]*[0,d].
Matrix Transformation::viewPort(int width, int height){
    Matrix viewPort = Matrix::identity(4);
    //trans
    viewPort[0][3] = width / (float)2; 
    viewPort[1][3] = height / (float)2;


    //scale
    viewPort[0][0] = width/(float)2;
    viewPort[1][1] = height / (float)2;
    return viewPort;

}


//modelView Matrix
//eye_position  look_at_direction  up_direction
//up_direction is not y_axis, but a vector to define the plane 
//z_axis points outside the screen
Matrix Transformation::modelView(Vec3f& eye, Vec3f& center, Vec3f& up) {
    Vec3f z_axis = (eye - center).normalize();
    Vec3f x_axis = (z_axis^up).normalize();
    Vec3f y_axis = (x_axis^z_axis).normalize();
    Matrix trans = Matrix::identity(4);
    Matrix rotate = Matrix::identity(4);
    Matrix modelView = Matrix::identity(4);
    for (int i = 0; i < 3; i++) {
        rotate[i][0] = x_axis[i]; //旋转矩阵的特性：逆矩阵为矩阵的转置
        rotate[i][1] = y_axis[i];
        rotate[i][2] = z_axis[i];

        trans[i][3] = -eye[i];
    }
    
    modelView = rotate * trans * modelView;

    return modelView;
}


//projection matrix
//aspect_ratio = width / height
//TODO: zfar-znear分别代表什么？
//注意这里使用的是右手系，屏幕里是负值，越远值越小
Matrix Transformation::projection(float eye_fov, float aspect_ratio, float zNear, float zFar){
    Matrix pers_ortho(4,4);
    Matrix orthro = Matrix::identity(4);
    Matrix projection = Matrix::identity(4);
    float angle = eye_fov /(float)180 * PI;//角度转弧度
    float height = 2 * zNear * std::tan(angle / 2);
    float width = height * aspect_ratio;
    zNear = -zNear;
    zFar = -zFar;
    pers_ortho[0][0] = zNear;
    pers_ortho[1][1] = zNear;
    pers_ortho[2][2] = zNear + zFar;
    pers_ortho[2][3] = -zFar * zNear;
    pers_ortho[3][2] = 1;


    Matrix orthro_trans = Matrix::identity(4);
    Matrix orthro_scale = Matrix::identity(4);
    orthro_trans[2][3] = -(zNear + zFar) / (float)2;

    orthro_scale[0][0] = 2 / width;
    orthro_scale[1][1] = 2 / height;
    orthro_scale[2][2] = 2 / (zNear - zFar);

    orthro = orthro_scale  * orthro_trans;

    projection = orthro * pers_ortho * projection;
    return projection;
}

/**Bresenham算法：
 * 减少for循环里的浮点除法运算
 * float的运算速度要小于int的运算速度 
*/
void Transformation::line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
    bool steep = false; 
    if (std::abs(x0-x1)<std::abs(y0-y1)) { 
		// if the line is steep, we transpose the image
		//flip along y=x 
        std::swap(x0, y0); 
        std::swap(x1, y1); 
        steep = true; 
    } 
    if (x0>x1) { // make it left−to−right 
        std::swap(x0, x1); 
        std::swap(y0, y1); 
    } 
	/**如果斜率小于1，下一个像素点只会在右边或者右上方
	 * 如果斜率大于1， 下一个像素点只会在上方或者右上方
	*/
	int dx = x1 - x0; 
	int dy = y1 - y0;
	//float dk = std::abs(dy / (float)dx); dk描述的是当x向右移动1，y向上移动多少
	int dk2 = std::abs(dy) * 2;
	float k2 = 0;
	int y = y0; // 这里相当于每涂色一个像素，都把被涂色的像素看作是新的当前的像素


    for (int x=x0; x<=x1; x++) { 

        if (steep) { 
            image.set(y, x, color); // if transposed, de−transpose 
        } else { 
            image.set(x, y, color); 
        } 

		k2 += dk2; 

		//为了再一次避免浮点除法，k>0.5 => Σdy/dx>0.5 => 2*∑dy>dx,令dk2=2*dy => k2 > dx
		if (k2 > dx){
			y += (y1>y0?1:-1); 
			k2 -= dx*2;
		}

		/**
		k += dk; // 这里的k描述的是x向右移动1后，y相比于y0向上移动了多少
		if (k > 0.5){
			y += (y1>y0?1:-1); 
			k -= 1.0; 
			// y上移1后重新开始清空k重新开始计数
		}
		//这里k是0.5的原因：希望移动的区间从[0, 1]滑动到[-0.5, 0.5]，
		//让线条更靠近像素点的中心而不是像素的底部
		*/

    } 

}

/**Another way to check if a point is inside of a triangle:
 * barycentric coordinates 重心坐标
 * P = (1-u-v) * A + u * B + v * C
 * [u v 1] [AB.x] = 0
 * 			AC.x
 * 			PA.x
 * [u v 1] [AB.y] = 0
 * 			AC.y
 * 			PA.y
 * 所以[u v 1] =	[AB.x] ^[AB.y]
 * 				 	 AC.x	 AC.y
 * 					 PA.x	 PA.y
*/ 
Vec3f Transformation::barycentric(Vec3f P, Vec3f A, Vec3f B, Vec3f C) {
	//(x, y) = alpha * A + bate * B + gamma * C
	//alpha = area(BCP) / area(ABC)
	//beta = area(ACP) / area(ABC)
	float area = ((B-A) ^ (C - A)).z ;
	float  alpha = ((C - B) ^ (P - B)).z  / area;
	float beta = ((P - A) ^ (C -A)).z /area;
	float gamma = 1 - alpha - beta;
	return Vec3f(alpha, beta, gamma);
}


bool Transformation::isinside(Vec3f p, Vec3f t0, Vec3f t1, Vec3f t2) {
	Vec3f t0p = p - t0;
	Vec3f t1p = p - t1;
	Vec3f t2p = p - t2;

	Vec3f t0t1 = t1 - t0;
	Vec3f t1t2 = t2 - t1;
	Vec3f t2t0 = t0 - t2;

	if ( ((t0t1 ^ t0p).z <= 0 && (t1t2 ^ t1p).z <= 0 && (t2t0 ^ t2p).z <= 0) || ((t0t1 ^ t0p).z >= 0 && (t1t2 ^ t1p).z >= 0 && (t2t0 ^ t2p).z >= 0) ) {
		return true;
	}
	return false;

}