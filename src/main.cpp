#include "../include/tgaimage.h"
#include "../include/model.h"
#include "../include/geometry.h"
#include <iostream>
#include <direct.h>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
Model *model = nullptr;
const int width = 800;
const int height = 800;

void line(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line2(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);
void line3(int x0, int y0, int x1, int y1, TGAImage &image, TGAColor color);

int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);


	model = new Model("../obj/african_head.obj");
	

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

