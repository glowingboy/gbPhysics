#include "../src/sptree.h"
#include <string>
#include <algorithm>
#include <iostream>
using namespace gb::physics;

// kd-tree
struct testData:public kd_key<int, 2>
{

};

void kd_node_test(const unsigned int count = 1000)
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
    kd_node<testData> node(data, count);
    testData sp;
    sp.key[0] = 4;
    sp.key[1] = 1;
    kd_node<testData>* ret;
    node.nearest_neighbour_search(sp, ret);
    assert(ret != nullptr);
    std::cout << "ret: " << "(" << ret->data.key[0] << ", " << ret->data.key[1] << ")" << std::endl;
    std::cout<< std::endl << "********************************" << std::endl;

    delete[] data;
}

// octree
struct sptt
{
    sptt(const vec3F & centre, const float radius):
	sbb(centre, radius)
	{}
    struct contain
    {
	bool operator()(sptt* const & sptt, const aabb<>& o)
	    {
		return o.contain(sptt->sbb);
	    }
    };

    const spherebb<>& GetSphere() const
	{
	    return sbb;
	}
    spherebb<> sbb;
};

std::string vec3_tostring(const vec3F& v)
{
    char buf[64] = {'\0'};
    sprintf(buf, "%f, %f, %f", (float)(v.x), (float)(v.y), (float)(v.z));
    return std::string(buf);
}

std::string float_tostring(const float f)
{
    char buf[64] = {'\0'};
    sprintf(buf, "%f", f);
    return std::string(buf);
}
std::string sbb_tostring(const spherebb<>& sbb)
{
    std::string ret = "sbb:\n";
    ret += "centre: ";
    ret += vec3_tostring(sbb.centre);
    ret += "\t radius: ";
    ret += float_tostring(sbb.radius);
    ret += "\n";
    
    return ret;
}

std::string aabb_tostring(const aabb<>& aabb)
{
    std::string ret = "aabb:\n";
    ret += "lower: ";
    const vec3F (&dia)[2] = aabb.diagonal;
    ret += vec3_tostring(dia[0]);
    ret += "\t upper: ";
    ret += vec3_tostring(dia[1]);
    ret += "\t lenSide: ";
    ret += vec3_tostring(aabb.lenSide);
    ret += "\n";
    
    return ret;
}
int sptree_test(const std::uint32_t count = 100)
{
    // kd-tree test
    kd_node_test(count);
    
    // octree test
    octree<sptt*, sptt::contain> oct(aabb<>(vec3F(0, 0, 0), vec3F(count, count, count)));
    
    for(std::uint32_t i = 0; i < count; i++)
    {
	sptt* s = new sptt(vec3F(rand() % count, rand() % count, rand() % count), rand() % 20 * 0.01f);
	octree<sptt*, sptt::contain>* ret;
	oct.insert(s, ret);
	std::cout << "sptt: " << sbb_tostring(s->sbb) << std::endl;
	const aabb<>& octbb = ret->GetBB();
	const std::set<sptt*>& eles = ret->GetElements();
	std::cout << "octbb: " << aabb_tostring(octbb) << std::endl;
	std::cout << "siblings: ";
	std::for_each(eles.begin(), eles.end(), [](sptt* const & ele)
		      {
			  std::cout << " s@ " << sbb_tostring(ele->sbb);
		      });

	std::cout << std::endl;
    }

    ttt<int> x;
    x.func();
    return 0;
}
