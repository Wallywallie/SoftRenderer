#include "../include/tgaimage.h"
#include "../include/model.h"
#include "../include/geometry.h"
#include "../include/transformation.h"
#include <iostream>
#include <direct.h>
#include <algorithm>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
Model *model = nullptr;
const int width = 800;
const int height = 800;



void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy);
bool isinside(Vec3f p, Vec3f t0, Vec3f t1, Vec3f t2) ;
Vec3f barycentric(Vec3f P, Vec3f A, Vec3f B, Vec3f C);

//ModelView Matrix
Matrix ModelView(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f z = (eye-center).normalize();
    Vec3f x = (z^up).normalize();
    Vec3f y = (x^z).normalize();
    Matrix Minv = Matrix::identity(4);
    Matrix Tr   = Matrix::identity(4);
    for (int i=0; i<3; i++) {
        Minv[0][i] = x[i]; //这样写是因为旋转矩阵Minv是自己的逆矩阵的转置
        Minv[1][i] = y[i];
        Minv[2][i] = z[i];

        Tr[i][3] = -eye[i];
    }
    Matrix ModelView = Minv*Tr;
	return ModelView;
}
//perspective = orthro * pres->ortho
//这里假设相机在Z轴上，离原点距离为c，即一点透视
Matrix projection(float c) {
	Matrix persp = Matrix::identity(4);
	persp[3][2] = -1/c;
	return persp;
}

//bi-unit cube [-1,1]*[-1,1]*[-1,1] is mapped onto the screen cube [x,x+w]*[y,y+h]*[0,d].
Matrix viewport(Vec3f v , int w, int h) {
    Matrix m = Matrix::identity(4);
    m[0][3] = v.x+w/2.f;
    m[1][3] = v.y+h/2.f;
    m[2][3] = v.z/2.f;

    m[0][0] = w/2.f;
    m[1][1] = h/2.f;
    m[2][2] = v.z/2.f;
    return m;
}

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);

	//read model
	model = new Model("../obj/african_head.obj");
	//read texture
	TGAImage tex;
	if (tex.read_tga_file("../obj/african_head_diffuse.tga")){
		std::cout << "texture has been loaded." << std::endl;
		//由于图像翻转了，texture也要跟着翻转
		tex.flip_vertically();
	} else {
		std::cerr << "Failed to read TGA image." << std::endl;
	}


	//zbuffer 初始化
	float* zbuffer = new float[width * height];
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = std::numeric_limits<float>::min();
	}
	
	Vec3f light_dir(0,0,1); // define light_dir
	Vec3f eye_pos(0, 0, 5);
	Vec3f center(0,0,0);
	Vec3f up(0,1,0);
	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		std::vector<int> vt = model->fvtex(i); //从obj的f 得到三个vt的索引
		Vec3f screen_coords[3]; 
		Vec3f world_coords[3]; 
		Vec3f tex_coords[3];//存储纹理坐标
		for (int j=0; j<3; j++) { 
			Vec3f v = model->vert(face[j]); //得到顶点坐标
			Matrix transformation = Transformation::viewPort(width, height, 255)* Transformation::projection(120, 2, -5, -10) * Transformation::modelView(eye_pos, center, up);
			Matrix mdoelv = transformation;

			std::cout << "-----------transformation-----------"<<std::endl;
			std::cout << mdoelv  <<std::endl;
			Matrix transformation2 = viewport(v,width, height)*projection(5)*ModelView(eye_pos, center, up);
			Matrix modelv2 = viewport(v,width, height);
			std::cout << "-----------transformation2-----------"<<std::endl;
			std::cout <<  transformation2 << std::endl;
		} 

	}
	

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete(model);
	return 0;
}

/**Bresenham算法：
 * 减少for循环里的浮点除法运算
 * float的运算速度要小于int的运算速度 
*/
void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color) { 
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

void triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy) {


	int xmin = std::min({pts[0].x, pts[1].x, pts[2].x});
	int ymin = std::min({pts[0].y, pts[1].y, pts[2].y});
	int xmax = std::max({pts[0].x, pts[1].x, pts[2].x});
	int ymax = std::max({pts[0].y, pts[1].y, pts[2].y});

	for (int i = std::floor(xmin); i <= std::ceil(xmax); i++) {
		for (int j = std::floor(ymin); j <= std::ceil(ymax); j++) {
			Vec3f p(i + 0.5, j + 0.5, 0);
			
			if (isinside(p, pts[0], pts[1], pts[2])) {
				Vec3f baryCoor = barycentric(p, pts[0], pts[1], pts[2]);

				p.z = baryCoor.x * pts[0].z + baryCoor.y * pts[1].z +  baryCoor.z * pts[2].z;
				if (p.z > zbuffer[i + j * width]) {
					zbuffer[i + j * width] = p.z;

					//对纹理进行插值，得到颜色
					
						
					Vec3f coor = (tex_coor[0] * baryCoor.x + tex_coor[1] * baryCoor.y + tex_coor[2] * baryCoor.z);

					//std:: cout << tex_coor[0].x << " " << tex_coor[0].y << " " << tex_coor[0].z << std::endl; 
 
					TGAColor color = tex.get(coor.x * tex.get_width(), coor.y * tex.get_height()) ; //纹理坐标得取值为[0, 1],映射到像素空间
					color = TGAColor(color.r * intensiy, color.g*intensiy, color.b * intensiy, color.a);
					image.set(i, j, color);
					
				}

				
			}
		}

	}

}

bool isinside(Vec3f p, Vec3f t0, Vec3f t1, Vec3f t2) {
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
Vec3f barycentric(Vec3f P, Vec3f A, Vec3f B, Vec3f C) {
	//(x, y) = alpha * A + bate * B + gamma * C
	//alpha = area(BCP) / area(ABC)
	//beta = area(ACP) / area(ABC)
	float area = ((B-A) ^ (C - A)).z ;
	float  alpha = ((C - B) ^ (P - B)).z  / area;
	float beta = ((P - A) ^ (C -A)).z /area;
	float gamma = 1 - alpha - beta;
	return Vec3f(alpha, beta, gamma);
}

