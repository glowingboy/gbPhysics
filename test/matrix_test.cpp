#include "../src/matrix.h"

using namespace gb::physics;
#include <iostream>
struct stA
{
    template <typename T>
    void foo(const T a)
    	{
    	    std::cout << "T version" << std::endl;
    	}
    void foo(const stA& a) const
	{
	    std::cout << "const version" << std::endl;
	}
};


int matrix_test()
{
    // mat3<Float> a{{{12, 6, -4}, {-51, 167, 24}, {4, -168, -41}}};
    // mat3<Float> ev = a.eigenvectors();
    
    stA().foo(stA());
    stA a;
    a.foo(stA());

    const stA b;
    b.foo(stA());
    return 0;
}
