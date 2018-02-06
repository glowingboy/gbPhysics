#include "../src/type.h"
#include <iostream>

using namespace gb::physics;


int type_test(const unsigned int count = 1000)
{
    // Float test
    Float c = 0.0f;
    Float d = FLT_EPSILON;
    if(c != d)
	return 1;

    // bit_vector test
    std::uint8_t* bitArray = new std::uint8_t[count];

    for(unsigned int i = 0; i < count; i++)
    {
	bitArray[i] = rand() % 2;
    }

    bit_vector bitVec;
    bitVec.reserve(count);

    std::uint8_t curVal = 0;
    size_t curValSize = 0;
    size_t curValIdx = 0;
    for(unsigned int i = 0; i < count; i++)
    {
	if(curVal != bitArray[i])
	{
	    bitVec.insert(curValIdx, curValSize, curVal);
	    curVal = bitArray[i];
	    curValSize = 1;
	    curValIdx = i;
	}
	else
	{
	    curValSize ++;
	}

    }

    bitVec.insert(curValIdx, curValSize, curVal);
    
    for(unsigned int i = 0; i < count; i++)
    {
	if(bitVec[i] != bitArray[i])
	    return 1;
    }
    std::cout << std::endl;

    delete[] bitArray;

    return 0;
}    
