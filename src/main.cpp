#include "../include/tgaimage.h"
#include "../include/model.h"
#include "../include/geometry.h"
#include "../include/shader.h"
#include <iostream>
#include <direct.h>
#include <algorithm>
#include <vector>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
const TGAColor green   = TGAColor(0, 255,   0,   255);
Model *model = nullptr;
int width = 800;
const int height = 800;

Vec3f light_dir(0,0,1); // define light_dir



/**
 * 
 * struct Light{
	Vec3f intensity;
	Vec3f light_dir;
	Light(Vec3f dir, Vec3f itst = Vec3f(255, 255, 255)) : light_dir(dir), intensity(itst){}
};
*/




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

	//std::vector<std::vector<float>> zbuffer(height, std::vector<float>(width, std::numeric_limits<float>::lowest()));
	std::vector<float> zbuffer (width * height, std::numeric_limits<float>::lowest());
	
	//TODO: segmentation fault in zbuffer
	//TODO: the image facing downwards
	//TODO: corret parameter for camera
	//TODO: wandering in the scene

	FlatShading shader;
	Camera camera;
	
	shader.set_transformation(camera, width, height);
	for (int i=0; i<model->nfaces(); i++) { 
		Vec3f screen_coords[3]; 
		for (int j=0; j<3; j++) { 
			screen_coords[j] = shader.vertex(model, i, j);
		} 
		shader.triangle(screen_coords, shader, image, zbuffer, tex);				
	} 

	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image
	image.write_tga_file("output.tga");
	delete(model);
	return 0;
}









