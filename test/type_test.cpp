#include "../src/type.h"
#include <iostream>

using namespace gb::physics;


int type_test()
{
    Float c = 0.0f;
    Float d = FLT_EPSILON;
    return c == d? 0 : 1;
}    
