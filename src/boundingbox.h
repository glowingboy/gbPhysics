#pragma once

#include "algorithm.h"

namespace gb
{
    namespace physics
    {
	using namespace gb::algorithm;
	
	template<typename T = float>
	struct spherebb
	{
	    spherebb():
		centre(0),
		radius(0)
		{}
	    spherebb(const vec3<T>& centre_, const T radius_):
		centre(centre_),
		radius(radius_)
		{}
		
	    vec3<T> centre;
	    T radius;
	};
	
#define GB_PHYSICS_DIAGONAL_LOWER_IDX 0
#define GB_PHYSICS_DIAGONAL_UPPER_IDX 1
	
	template<typename T = float>
	struct aabb
	{
	    aabb():
		diagonal{0},
		lenSide(0)
		{}

	    aabb(const vec3<T>& lower, const vec3<T>& upper):
		diagonal{lower, upper},
		lenSide(upper.x - lower.x)
		{}

	    bool isCollided(const aabb & o) const
		{
		    const vec3<T> (&o_diagonal)[2] = o.diagonal;
		    for(std::uint8_t i = 0; i < 3; i++)
		    {
			if(diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX][i] < o_diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX][i])
			    return false;
			if(diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX][i] > o_diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX][i])
			    return false;
		    }
		    return true;
		}
	    bool isInside(const aabb& o)const
		{
		    if(o._lenSide > lenSide)
			return false;
		    else
		    {
			const vec3<T> (&o_diagonal) [2] = o.diagonal;

			for(std::uint8_t i = 0; i < 3; i++)
			{
			    // lower point
			    if(diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX][i] < o_diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX][i])
				return false;
			    // upper point
			    if(diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX][i] > o_diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX][i])
				return false;
			}
			return true;
		    }
		}
	    bool isInside(const spherebb<T>& o) const
		{
		    if((o.radius * 2) > lenSide)
			return false;
		    const vec3<T> sbb_lower(o.centre - o.radius);
		    const vec3<T> sbb_upper(o.centre + o.radius);

		    for(std::uint8_t i = 0; i < 3; i ++)
		    {
			if(sbb_lower[i] < diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX][i])
			    return false;
			if(sbb_upper[i] > diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX][i])
			    return false;
		    }
		    return true;
		}
	    vec3<T> diagonal[2];
	    T lenSide;
	};
    }
}
