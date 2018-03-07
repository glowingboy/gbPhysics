#include "../src/matrix.h"

using namespace gb::physics;

int matrix_test()
{
     mat3<Float> a{{{12, 6, -4}, {-51, 167, 24}, {4, -68, -41}}};
     mat3<Float> ev = a.eigenvectors();

    return 0;
}
