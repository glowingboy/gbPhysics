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
    std::cout<< std::endl << "********************************" << std::endl;
    for(int i = 0; i < count; i++)
    {
	data[i].key = rand()%100;
	std::cout << data[i].key << ", ";
    }

    std::cout<< std::endl << "********************************" << std::endl;
    
    gb::Algorithm::quickSelectMost<testData, testData::compare>(data, 0, 99, 10);
//    gb::Algorithm::quickSelectMost(data, 0, 100, 10);
    for(int i = 0 ; i < count; i++)
    {
	std::cout << data[i].key << ", ";
    }
    
    std::cout<< std::endl << "********************************" << std::endl;
    return 0;
}
