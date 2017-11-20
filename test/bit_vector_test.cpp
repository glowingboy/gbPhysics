#include <gbPhysics/algorithm.h>

int bit_vector_test(const unsigned int count = 1000)
{
    std::uint8_t bitArray[count];

    for(int i = 0; i < count; i++)
    {
	bitArray[i] = rand() % 2;
    }

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
	if(bitVec[i] != bitArray[i])
	    return 1;
    }
    std::cout << std::endl;

    return 0;
}
