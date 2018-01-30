#include "../src/sptree.h"
#include <string>
#include <algorithm>
using namespace gb::physics;
using namespace gb::algorithm;
struct sptt
{
    sptt(const vec3<> & centre, const float radius):
	sbb(centre, radius)
	{}
    struct intersect
    {
	bool operator()(const sptt*& sptt, const aabb<>& o)
	    {
		return o.intersect(sptt->sbb);
	    }
    };

    const spherebb<>& GetSphere()	const
	{
	    return sbb;
	}
    spherebb<> sbb;
};

std::string vec3_tostring(const vec3& v)
{
    char buf[64] = {'\0'};
    sprintf(buf, "%f, %f, &%f", v.x, v.y, v.z);
    return std::string(buf);
}
std::string float_tostring(const float f)
{
    char buf[64] = {'\0'};
    sprintf(buf, "%f", f);
    return std::string(buf);
}
std::string sbb_tostring(const spherebb& sbb)
{
    std::string ret = "sbb:\n";
    ret += "centre: ";
    ret += vec3_tostring(sbb.centre);
    ret += "\t radius: ";
    ret += float_tostring(sbb.radius);
    ret += "\n";
    
    return ret;
}

std::string aabb_tostring(const aabb& aabb)
{
    std::string ret = "aabb:\n";
    ret += "lower: ";
    const vec3 (&dia)[2] = aabb.diagonal;
    ret += vec3_tostring(dia[0]);
    ret += "\t upper: ";
    ret += vec3_tostring(dia[1]);
    ret += "\t lenSide: ";
    ret += float_tostring(aabb.lenSide);
    ret += "\n";
    
    return ret;
}
int sptree_test(const std::uint32_t count = 100)
{
    octree<sptt*> oct(vec3(0, 0, 0), vec3(count, count, count));
    
    for(std::uint32_t i = 0; i < count; i++)
    {
	sptt* s = new sptt(vec3(rand() % count, rand() % count, rand() % count), rand() % count);
	octree<sptt*>* ret;
	oct.insert(s, ret);
	std::cout << "sptt: " << sbb_format(s->sbb) << std::endl;
	const aabb& octbb = ret->GetBB();
	const std::set<sptt*>& eles = ret->GetElements();
	std::cout << "octbb: " << aabb_tostring(octbb) << std::endl;
	std::cout << "siblings: ";
	std::for_each(eles.begin(), eles.end(), [](sptt* & ele)
		      {
			  std::cout << " s@ " << sbb_tostring(ele->sbb);
		      });

	std::cout << std::endl;
    }
    
    return 0;
}
