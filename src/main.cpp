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


void triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy) {


	int xmin = std::min({pts[0].x, pts[1].x, pts[2].x});
	int ymin = std::min({pts[0].y, pts[1].y, pts[2].y});
	int xmax = std::max({pts[0].x, pts[1].x, pts[2].x});
	int ymax = std::max({pts[0].y, pts[1].y, pts[2].y});

	for (int i = std::floor(xmin); i <= std::ceil(xmax); i++) {
		for (int j = std::floor(ymin); j <= std::ceil(ymax); j++) {
			Vec3f p(i + 0.5, j + 0.5, 0);
			if (Transformation::isinside(p, pts[0], pts[1], pts[2])) {

				Vec3f baryCoor = Transformation::barycentric(p, pts[0], pts[1], pts[2]);

				p.z = baryCoor.x * pts[0].z + baryCoor.y * pts[1].z +  baryCoor.z * pts[2].z;
				
				//std::cout << p.z << std::endl;
				//std::cout << zbuffer[i + j * width] << std::endl;
				int x = i + j * width;
				if (p.z > zbuffer[i + j * width]) {
					zbuffer[i + j * width] = p.z;

					//对纹理进行插值，得到颜色
					
						
					Vec3f coor = (tex_coor[0] * baryCoor.x + tex_coor[1] * baryCoor.y + tex_coor[2] * baryCoor.z);

					//std:: cout << tex_coor[0].x << " " << tex_coor[0].y << " " << tex_coor[0].z << std::endl; 
 
					TGAColor color = tex.get(coor.x * tex.get_width(), coor.y * tex.get_height()) ; //纹理坐标得取值为[0, 1],映射到像素空间
					color = TGAColor(color.r * intensiy, color.g*intensiy, color.b * intensiy, color.a);
					image.set(i, j, color);
					//std::cout << "i: " << i << "j: " << j <<"color: " << color.val << std::endl;
					
				}

				
			}
		}

	}

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
		zbuffer[i] = std::numeric_limits<float>::lowest();//std::numeric_limits<float>::min()返回的是 float 类型的最小正数值
	}
	
	Vec3f light_dir(0,0,-1); // define light_dir
	Vec3f eye_pos(0, 0,3);
	Vec3f center(0,0,0);
	Vec3f up(0,1,0);
	Matrix transformation = Transformation::viewPort(width, height)* Transformation::projection(45, 1, 0.1, 50) * Transformation::modelView(eye_pos, center, up);
	//TODO: segmentation fault in zbuffer
	//TODO: the image facing downwards
	//TODO: corret parameter for camera
	//TODO: wandering in the scene
	Matrix test = Transformation::projection(45, 1, 0.1, 50);
	std::cout << test << std::endl;

	for (int i=0; i<model->nfaces(); i++) { 
		std::vector<int> face = model->face(i); 
		std::vector<int> vt = model->fvtex(i); //从obj的f 得到三个vt的索引
		Vec3f screen_coords[3]; 
		Vec3f world_coords[3]; 
		Vec3f tex_coords[3];//存储纹理坐标
		for (int j=0; j<3; j++) { 
			Vec3f v = model->vert(face[j]); //得到顶点坐标
			Vec3f tex = model->vtex(vt[j]); // 得到纹理坐标
			screen_coords[j] = Vec3f(transformation * Matrix(v));
			world_coords[j]  = v; 
			tex_coords[j] = tex;	
			if (screen_coords[j][0] > 800 || screen_coords[j][0] < 0) {std::cout << "out of range" << std::endl;}
		} 
		Vec3f n = (world_coords[2]-world_coords[0])^(world_coords[1]-world_coords[0]); 
		n.normalize(); 
		float intensity = n*light_dir; 
		if (intensity>0) { //back-face culling 

			triangle(screen_coords, zbuffer, image, tex, tex_coords, intensity);				
		} 		
	} 
		
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete(model);
	return 0;
}









