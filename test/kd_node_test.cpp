#include <gbPhysics/algorithm.h>
#include <initializer_list>
struct testData:public gb::algorithm::kd_key<int, 2>
{

};

int kd_node_test(const unsigned int count = 1000)
{
    
    testData* data = new testData[count];

    for(unsigned int i = 0; i < count; i ++)
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
    gb::algorithm::kd_node<testData>* ret;
    int dist = node.nearest_neighbour_search(sp, ret);
    assert(ret != nullptr);
    std::cout << "ret: " << "(" << ret->data.key[0] << ", " << ret->data.key[1] << ")" << std::endl;
    std::cout<< std::endl << "********************************" << std::endl;

	delete[] data;
    return 0;
}
