#include <iostream>
#include "../gbAlgorithm.h"
#include <cstdlib>
struct testData
{
    struct getkey
    {
	int operator()(const testData& data, const std::uint8_t d)
	    {
		return data.key[d];
	    }
    };
    
    int key[2];


};


int main(int argc, char** argv)
{
    const unsigned int count = 10;
    testData data[count];
    int intArray[count];
    std::cout<< std::endl << "********************************" << std::endl;
    for(int i = 0; i < count; i++)
    {
	data[i].key[0] = rand()%count;
	data[i].key[1] = rand()%count;
	std::cout << "(" << data[i].key[0] << "," << data[i].key[1] << ") ";
    }

    std::cout<< std::endl << "********************************" << std::endl;


    gb::algorithm::kd_node<testData, 2, gb::algorithm::kd_compare<testData, testData::getkey>> node(data, count);

    
    for(int i = 0; i < count; i++)
    {
	std::cout << "(" << data[i].key[0] << "," << data[i].key[1] << ") ";
    }

    std::cout<< std::endl << "********************************" << std::endl;

    std::cout << "tree size: " <<  node.tree_size() << std::endl;
    std::cout << "l size: " << node.l->tree_size() << std::endl;
    std::cout << "r size: " << node.r->tree_size() << std::endl;
    //std::nth_element<int*>(intArray, intArray + 50, intArray + 99);
    return 0;
}
