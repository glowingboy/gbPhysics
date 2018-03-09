#include "../src/matrix.h"

using namespace gb::physics;

int matrix_test()
{
     //mat3<Float> a{{{12, 6, -4}, {-51, 167, 24}, {4, -68, -41}}};
	//mat3<Float> a{ { { -1, -4, 1 },{ 1, 3, 0},{ 0, 0, 2 } } };
	//mat3<Float> a{ { { 2, 1, 0 },{ 1, 3, -1 },{ 0, -1, 6 } } };
	mat3<Float> a{ { { 3, 2, 4 },{ 2, 0, 2 },{ 4, 2, 3 } } };
     mat3<Float> ev = a.eigenvectors();

    return 0;
}
