#ifndef __SHADER__
#define __SHADER__
#include "../include/geometry.h"


class Shader{
public:
    virtual Vec3f vertex();

    virtual Vec3f fragment();

};



#endif __SHADER__