#include <iostream>
#include "../gbAlgorithm.h"
#include <cstdlib>
struct testData:public gb::algorithm::kd_key<int, 2>
{

};


int main(int argc, char** argv)
{
    const unsigned int count = 19;
    testData data[count];
    int intArray[count];
    std::cout<< std::endl << "********************************" << std::endl;
    for(int i = 0; i < count; i++)
    {
//	intArray[i] = rand()%count;
	data[i].key[0] = rand()%count;
	data[i].key[1] = rand()%count;
	std::cout << "(" << data[i].key[0] << "," << data[i].key[1] << ") ";
    }

    std::cout<< std::endl << "********************************" << std::endl;


    gb::algorithm::kd_node<testData> node(data, count);

    
    for(int i = 0; i < count; i++)
    {
	std::cout << "(" << data[i].key[0] << "," << data[i].key[1] << ") ";
    }

    std::cout<< std::endl << "********************************" << std::endl;

    std::cout << "tree size: " <<  node.size() << std::endl;
    std::cout << "l size: " << node.l->size() << std::endl;
    std::cout << "r size: " << node.r->size() << std::endl;
    //std::nth_element<int*>(intArray, intArray + 50, intArray + 99);

    std::cout << std::endl;

    for(int i = 0; i < count ; i++)
    {
	std::cout << intArray[i] << ", ";
    }

    std::cout << std::endl;

    intArray[0] = 5;
    intArray[1] = 3;
    std::nth_element<int*>(intArray, intArray + 0, intArray + 2);
    for(int i = 0; i < count ; i++)
    {
	std::cout << intArray[i] << ", ";
    }

    std::cout << std::endl;
    return 0;
}
