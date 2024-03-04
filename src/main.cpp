#include "../include/tgaimage.h"
#include "../include/model.h"
#include "../include/geometry.h"
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
void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color);
bool isinside(Vec2i p, Vec2i t0, Vec2i t1, Vec2i t2);

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);


	model = new Model("../obj/african_head.obj");
	Vec3f light_dir(0,0,-1); // define light_dir

	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		Vec2i screen_coords[3]; 
		Vec3f world_coords[3]; 
		for (int j=0; j<3; j++) { 
			Vec3f v = model->vert(face[j]); 
			screen_coords[j] = Vec2i((v.x+1.)*width/2., (v.y+1.)*height/2.); 
			world_coords[j]  = v; 
		} 
		Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
		n.normalize(); 
		float intensity = n*light_dir; 
		if (intensity>0) { //back-face culling 
			triangle(screen_coords[0], screen_coords[1], screen_coords[2], image, TGAColor(intensity*255, intensity*255, intensity*255, 255)); 
		} 
	}

	
	/**
	for (int i=0; i<model->nfaces(); i++) { 
    	std::vector<int> face = model->face(i); 
		for (int j=0; j<3; j++) { 
			Vec3f v0 = model->vert(face[j]); 
			Vec3f v1 = model->vert(face[(j+1)%3]); 

			int x0 = (v0.x+1.)*width/2.; 
			int y0 = (v0.y+1.)*height/2.; 
			int x1 = (v1.x+1.)*width/2.; 
			int y1 = (v1.y+1.)*height/2.; 
			line(x0, y0, x1, y1, image, white); 
		} 
	}
	*/

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



void triangle(Vec2i t0, Vec2i t1, Vec2i t2, TGAImage &image, TGAColor color) {

	int xmin = std::min({t0.x, t1.x, t2.x});
	int ymin = std::min({t0.y, t1.y, t2.y});
	int xmax = std::max({t0.x, t1.x, t2.x});
	int ymax = std::max({t0.y, t1.y, t2.y});



	for (int i = xmin; i <= xmax; i++) {
		for (int j = ymin; j <= ymax; j++) {
			Vec2i p(i, j);
			if (isinside(p, t0,t1, t2)) {
				image.set(i, j, color);
			}
		}

	}

}

bool isinside(Vec2i p, Vec2i t0, Vec2i t1, Vec2i t2) {
	Vec2i t0p = p - t0;
	Vec2i t1p = p - t1;
	Vec2i t2p = p - t2;

	Vec2i t0t1 = t1 - t0;
	Vec2i t1t2 = t2 - t1;
	Vec2i t2t0 = t0 - t2;

	if ( (t0t1.cross_product(t0p) <= 0 && t1t2.cross_product(t1p) <= 0 && t2t0.cross_product(t2p) <= 0) || (t0t1.cross_product(t0p) >= 0 && t1t2.cross_product(t1p) >= 0 && t2t0.cross_product(t2p) >= 0)) {
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
Vec3f barycentric(Vec2i p, Vec2i A, Vec2i B, Vec2i C) {
	//(x, y) = alpha * A + bate * B + gamma * C
	//alpha = area(BCP) / area(ABC)
	//beta = area(ACP) / area(ABC)
	int area = -(A.x - B.x) * (C.y - B.y) + (A.y - B.y) * (C.x - B.x);
	float  alpha = (-(p.x - B.x) * (C.y - B.y) + (p.y - B.y) * (C.x - B.x)) / (float) area;
	float beta = -(p.x - C.x) * (A.y - C.y) + (p.y - C.y) * (A.x - C.x) / (float) area;
	float gamma = 1 - alpha - beta;
	return Vec3f(alpha, beta, gamma);
}