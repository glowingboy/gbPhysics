// spatial partitioning tree

#pragma once

#include "boundingbox.h"
#include <set>
#include <vector>
#include <functional>
namespace gb
{
    namespace physics
    {
	/*
	*@param _Intersect, bool _Intersect(_Ele, _bb).
	*/
	template <typename _Ele, typename _Intersect, typename _BB_Unit = float>
	class octree
	{
	public:
	    octree(const aabb<_BB_Unit>& aabb):
		_bb(aabb),
		_children{nullptr}
		{
		    const vec3<_BB_Unit> (&diagonal)[2] = aabb.diagonal;
		    
		    const vec3<_BB_Uint> centre = (diagonal[0] + diagonal[1]) * 0.5f;
		    const vec3<_BB_Uint> octanLen = (diagonal[1] - diagonal[0]) * 0.5f;
		    const vec3<_BB_Uint> lowers[2] = {diagonal[0], centre};

		    std::uint8_t idx = 0;
		    for(std::uint8_t i = 0; i < 2; i++)
		    {
			for(std::uint8_t j = 0; j < 2; j++)
			{
			    for(std::uint8_t k = 0; k < 2; k++)
			    {
				vec3<_BB_Unit> (&octanDia)[2] = _octanBB[idx++].diagonal;
				vec3<_BB_Unit>& lower = octanDia[0];
				lower.x = lowers[i].x;
				lower.y = lowers[j].y;
				lower.z = lowers[z].z;
				octanDia[1] = lower + octanLen;
			    }
			}
		    }
		}
	    ~octree()
		{
		    for(std::uint8_t i = 0; i < 8; i++)
		    {
			octree* & child = _children[i];
			delete child;
			child = nullptr;
		    }
		}
	public:
	    const aabb<_BB_Unit>& GetBB() const
		{
		    return _bb;
		}
	    const std::set<_Ele>& GetElements() const
		{
		    return _eles;
		}
	    void insert(T & ele)
		{
		    if(_ist(ele, _bb))
		    {
			for(std::uint8_t i = 0; i < 8; i++)
			{
			    const aabb<_BB_Unit>& octanBB = _octanBB[i];
			    if(_ist(ele, octanBB))
			    {
				if(_children[i] != nullptr)
				    _children->insert(ele);
				else
				{
				    _children[i] = new octree(octanBB);
				    _children->insert(ele);
				}
			    }
			    else
				_eles.insert(ele);
			}
		    }
		    else
			_eles.insert(ele);
		}
	    void insert(T & ele, octree* & oct)
		{
		    if(_ist(ele, _bb))
		    {
			for(std::uint8_t i = 0; i < 8; i++)
			{
			    const aabb<_BB_Unit>& octanBB = _octanBB[i];
			    if(_ist(ele, octanBB))
			    {
				octree* & child = _children[i];
				if(child != nullptr)
				    child->insert(ele);
				else
				{
				    child = new octree(octanBB);
				    child->insert(ele);
				}
				oct = child;
			    }
			    else
				_eles.insert(ele);
			}
		    }
		    else
			_eles.insert(ele);
		    
		    oct = this;
		}
	    
	    std::vector<_Ele> query_intersect(const aabb<_BB_Unit>& q) const
		{
		    std::vector<_Ele> ret;
		    if(q.intersect(_bb))
		    {
			ret.insert(ret.end(), _eles.begin(), _eles.end());

			for(std::uint8_t i = 0; i < 8; i++)
			{
			    octree*& child = _children[i];
			    if(child != nullptr)
			    {
				std::vector<_Ele> subRet = child->query_intersect(q);
				ret.insert(ret.end(), subRet.begin(), subRet.end());
			    }
			}
		    }
		    return ret;
		}
	private:
	    std::set<_Ele> _eles;
	    aabb<_BB_Unit> _bb;
	    aabb<_BB_Unit> _octanBB[8];
	    octree* _children[8];

	    static _Intersect _ist;
	};
	template <typename _Ele, typename _Intersect, typename _BB_Unit>
	_Intersect octree::_ist;
    }
}
