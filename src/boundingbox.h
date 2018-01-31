#pragma once

#include "algorithm.h"
#include <limits>
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
		radius(0),
		sqRadius(0)
		{}
	    spherebb(const vec3<T>& centre_, const T radius_):
		centre(centre_),
		radius(radius_),
		sqRadius(std::pow(radius_, 2))
		{
		    assert(sqrtMaxR >= radius_);
		}
		
	    vec3<T> centre;
	    T radius;
	    T sqRadius;
#if ! defined(NDEBUG)
	    static constexpr T sqrtMaxR;
#endif
	};
#if ! defined(NDEBUG)
	template<typename T>
	constexpr T spherebb::sqrtMaxR = std::sqrt(std::numeric_limits<T>::max());
#endif
	
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

	    bool intersect(const aabb & o) const
		{
		    const vec3<T> (&o_diagonal)[2] = o.diagonal;
		    return (diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX] < o_diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX])
			&&(diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX] > o_diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX]);
		}
		bool interset(const spherebb<T> o)
		{
		    // if centre of sphere inside of bb
		    if(o.centre >= )
		}
	    bool contain(const aabb& o)const
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
	    bool contain(const spherebb<T>& o) const
		{
		    // if centre inside bb
		    if()
		}
	    vec3<T> diagonal[2];
	    T lenSide;
	};
    }
}
