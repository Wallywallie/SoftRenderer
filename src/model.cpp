#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../include/model.h"

Model::Model(const char *filename) : verts_(), faces_(), vtexs_(), f_vtex_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.is_open()) {
        std::cout << "File has been opened" << std::endl;
    }
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } else if (!line.compare(0, 2, "f ")) {
            std::vector<int> f;
            std::vector<int> f_v;
            int itrash, idx, idx_tex;
            iss >> trash;
            while (iss >> idx >> trash >> idx_tex >> trash >> itrash) {
                idx--; // in wavefront obj all indices start at 1, not zero
                idx_tex--;
                f.push_back(idx);
                f_v.push_back(idx_tex);
            }
            faces_.push_back(f);
            f_vtex_.push_back(f_v);

            
        } else if (!line.compare(0, 3, "vt ")) {
            //相等时，line.compare()返回0

            iss >> trash >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            vtexs_.push_back(v);
            

        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

std::vector<int> Model::fvtex(int idx) {
    return f_vtex_[idx];
}

Vec3f Model::vtex(int i) {
    return vtexs_[i];
}

