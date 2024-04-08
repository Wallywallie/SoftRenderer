#ifndef __SHADER__
#define __SHADER__
#include "../include/geometry.h"


class Shader{
public:
    virtual Vec3f vertex();

    virtual Vec3f fragment();

};


class FlatShading : public Shader {
    
    virtual Vec3f vertex() override {

    }

};

class GouraudShading : public Shader{

};

class PhongShading : public Shader{

};


#endif __SHADER__