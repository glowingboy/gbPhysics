#include <iostream>
#include "kd_node_test.cpp"
#include "bit_vector_test.cpp"

#define test(test_func, ...)					\
    if(test_func(__VA_ARGS__) == 0)				\
	std::cout << #test_func << " succeeded" << std::endl;	\
    else							\
	std::cout << #test_func << " failed" << std::endl; 
int main(int argc, char** argv)
{
    test(kd_node_test);
    test(bit_vector_test);
    
    return 0;
}
