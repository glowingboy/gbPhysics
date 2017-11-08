#include <iostream>
#include "../gbAlgorithm.h"
#include <cstdlib>
struct testData
{
    struct compare
    {
	bool operator()(const testData& l, const testData& r)const
	    {
		return l.key <= r.key;
	    }
    };
    
    struct compare_ptr
    {
	bool operator()(const testData* l, const testData* r)const
	{
	    return l->key <= r->key;
	}
    };
    
    int key;
};

int main(int argc, char** argv)
{
    const unsigned int count = 100;
    testData data[count];
    int intArray[count];
    std::cout<< std::endl << "********************************" << std::endl;
    for(int i = 0; i < count; i++)
    {
	data[i].key = rand()%100;
	std::cout << data[i].key << ", ";
    }

    std::cout<< std::endl << "********************************" << std::endl;

    const unsigned int mostCount = 50;
    gb::Algorithm::quickSelectMost<testData, testData::compare>(data, 0, 99, mostCount);
//    gb::Algorithm::quickSelectMost(data, 0, 100, 10);
    for(int i = 0 ; i < count; i++)
    {
	std::cout << data[i].key << ", ";
	if( i == (mostCount -1))
	    std::cout << "****, ";
    }
    
    std::cout<< std::endl << "********************************" << std::endl;


    for(int i = 0; i < count; i++)
    {
	intArray[i] = rand()%100;
	std::cout << intArray[i] << ", ";
    }
    std::cout<< std::endl << "********************************" << std::endl;

    gb::Algorithm::quickSelectMost<int>(intArray, 0, count - 1, mostCount);

    for(int i = 0; i < count; i++)
    {
	std::cout << intArray[i] << ", ";
	if(i == (mostCount - 1))
	    std::cout << "****, ";
    }

    std::cout<< std::endl << "********************************" << std::endl;

    return 0;
}
