#include <iostream>
#include "../src/gbAlgorithm.h"
#include <cstdlib>
struct testData:public gb::algorithm::kd_key<int, 2>
{

};


int main(int argc, char** argv)
{
    const unsigned int count = 100000;
    testData data[count];

    for(int i = 0; i < count; i ++)
    {
	data[i].key[0] = rand() % count;
	data[i].key[1] = rand() % count;
    }
    
    int i = 0;
    data[i].key[0] = 2; data[i++].key[1] = 3;
    data[i].key[0] = 5; data[i++].key[1] = 4;
    data[i].key[0] = 9; data[i++].key[1] = 6;
    data[i].key[0] = 4; data[i++].key[1] = 7;
    data[i].key[0] = 8; data[i++].key[1] = 1;
    data[i].key[0] = 7; data[i++].key[1] = 2;

    std::cout<< std::endl << "********************************" << std::endl;
    gb::algorithm::kd_node<testData> node(data, count);
    testData sp;
    sp.key[0] = 4;
    sp.key[1] = 1;
    gb::algorithm::kd_node<testData>* ret = node.nearest_neighbour_search(sp);
    std::cout << "ret: " << "(" << ret->data.key[0] << ", " << ret->data.key[1] << ")" << std::endl;
    std::cout<< std::endl << "********************************" << std::endl;

    std::uint8_t bitArray[count];

    for(int i = 0; i < count; i++)
    {
	bitArray[i] = rand() % 2;
	std::cout << (int)bitArray[i] << ", ";
    }
    std::cout << std::endl;

    gb::algorithm::bit_vector bitVec;
    bitVec.reserve(count);

    std::uint8_t curVal = 0;
    size_t curValSize = 0;
    size_t curValIdx = 0;
    for(int i = 0; i < count; i++)
    {
	if(curVal != bitArray[i])
	{
	    bitVec.insert(curVal, curValIdx, curValSize);
	    curVal = bitArray[i];
	    curValSize = 1;
	    curValIdx = i;
	}
	else
	{
	    curValSize ++;
	}

    }

    bitVec.insert(curVal, curValIdx, curValSize);
    
    for(int i = 0; i < count; i++)
    {
	std::cout << (int)bitVec[i] << ", ";
	if(bitVec[i] != bitArray[i])
	    std::cout << "***:" << i << "***";
    }
    std::cout << std::endl;
    
    return 0;
}
