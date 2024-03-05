#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;
	std::vector<Vec3f> vtexs_;
	std::vector<std::vector<int>> f_vtex_;
public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	int nvtex();
	Vec3f vert(int i);
	Vec3f vtex(int i);
	std::vector<int> face(int idx);
	std::vector<int> fvtex(int idx);
};

#endif //__MODEL_H__
