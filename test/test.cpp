#include <iostream>
#include "sptree_test.cpp"
#include "type_test.cpp"
#include "matrix_test.cpp"

#define test(testfunc, ...)					\
    if(testfunc(__VA_ARGS__) == 0)				\
  	std::cout << #testfunc << " succeeded." << std::endl;	\
    else							\
 	std::cout << #testfunc << " failed." << std::endl;

int main(int argc, char** argv)
{
    test(type_test);
    test(sptree_test);
    test(matrix_test);
    
    return 0;
}
