#include <iostream>
#include "kd_node_test.cpp"
#include "bit_vector_test.cpp"
#include "sptree_test.cpp"
#define test(testfunc, ...)					\
    if(testfunc(__VA_ARGS__) == 0)				\
	std::cout << #testfunc << " succeeded." << std::endl;	\
    else							\
	std::cout << #testfunc << " failed." << std::endl;

int main(int argc, char** argv)
{
    test(kd_node_test);
    test(bit_vector_test);
    test(sptree_test);
    return 0;
}
