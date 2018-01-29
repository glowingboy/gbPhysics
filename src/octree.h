#pragma once
#include <set>
#include "boundingbox.h"
namespace gb
{
    namespace physics
    {
	template <typename _Ele, typename _SphereGetter, typename _Comp = std::less<_Ele>, typename _BB_Unit = float>
	class octree
	{
	public:
	    void insert(T const ele)
		{
		    
		}
	private:
	    std::set<_Ele, _Comp> _values;
	    aabb<_BB_Unit> _bb[8];
	    octree<_Ele, _BB_Unit>* _octans[8];
	};	
    }
}
