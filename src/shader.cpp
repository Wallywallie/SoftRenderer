#include "../include/shader.h"
#include "../include/tgaimage.h"
#include "../include/transformation.h"
#include <algorithm>




void FlatShading::triangle(Vec3f *pts,float *zbuffer, TGAImage &image,TGAImage &tex, Vec3f *tex_coor, float intensiy, int width) {


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
