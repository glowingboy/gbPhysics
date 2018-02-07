#pragma once
#include "type.h"
#include "math.h"

#include <limits>

GB_PHYSICS_NS_BEGIN
	
template<typename T = Float>
struct spherebb
{
    spherebb():
	centre(0),
	radius(0)//,
//	sqRadius(0)
	{}
    spherebb(const vec3<T>& centre_, const T radius_):
	centre(centre_),
	radius(radius_)//,
//	sqRadius(std::pow(radius_, 2))
	{
//	    assert(sqrtMaxR >= radius_);
	}

    T sqDistance(const spherebb & o) const
	{
	    return centre.sqDistance(o.centre);
	}
    T distance(const spherebb & o) const
	{
	    return centre.distance(o.centre);
	}
    bool intersect(const spherebb& o) const
	{
	    return sqDistance(o) <= std::pow(radius + o.radius, 2);
	}
    bool contain(const spherebb & o) const
	{
	    //return (distance(o) + o.radius) <= radius;
	    //=> distance(o) <= (radius - o.radius)
	    //=>if radius >=o.radius then sqrDist(o) <= sqr(radius - o.radius)
	    //  else return false
	    if(radius >= o.radius)
		return sqDistance(o) <= std::pow(radius - o.radius, 2);
	    else
		return false;
	}
		
    vec3<T> centre;
    T radius;
//     T sqRadius;
// #if ! defined(NDEBUG)
//     static constexpr T sqrtMaxR = std::sqrt(std::numeric_limits<T>::max());
// #endif
};
	
#define GB_PHYSICS_DIAGONAL_LOWER_IDX 0
#define GB_PHYSICS_DIAGONAL_UPPER_IDX 1
	
template<typename T = Float>
struct aabb
{
    aabb():
	diagonal{vec3<T>(0)},
	lenSide(0)
	{}

    aabb(const vec3<T>& lower, const vec3<T>& upper):
	diagonal{lower, upper},
	lenSide(upper - lower)
	{}

    bool intersect(const aabb & o) const
	{
	    const vec3<T> (&o_diagonal)[2] = o.diagonal;
	    return (diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX] < o_diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX])
		&&(diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX] > o_diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX]);
	}
    bool intersect(const spherebb<T> o) const
	{
	    vec3<T> exterior_dia[2] = {diagonal[0], diagonal[1]};
	    const vec3<T> delta = o.radius;
	    exterior_dia[0] = exterior_dia[0] - delta;
	    exterior_dia[1] = exterior_dia[1] + delta;
	    return (o.centre >= exterior_dia[GB_PHYSICS_DIAGONAL_LOWER_IDX])
		&& (o.centre <= exterior_dia[GB_PHYSICS_DIAGONAL_UPPER_IDX]);
	}
    bool contain(const aabb& o)const
	{
	    if(o.lenSide > lenSide)
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
	    vec3<T> interior_dia[2] = {diagonal[0], diagonal[1]};
	    const vec3<T> delta = o.radius;
	    interior_dia[0] = interior_dia[0] + delta;
	    interior_dia[1] = interior_dia[1] - delta;
	    if(interior_dia[GB_PHYSICS_DIAGONAL_LOWER_IDX]
	       > interior_dia[GB_PHYSICS_DIAGONAL_UPPER_IDX])
		return false;
	    else
		return (o.centre >= interior_dia[GB_PHYSICS_DIAGONAL_LOWER_IDX])
		    && (o.centre <= interior_dia[GB_PHYSICS_DIAGONAL_UPPER_IDX]);
	}
    vec3<T> diagonal[2];
    vec3<T> lenSide;
};

GB_PHYSICS_NS_END
