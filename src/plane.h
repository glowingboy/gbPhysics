#pragma once

#include "type.h"

GB_PHYSICS_NS_BEGIN

template <typename T>
struct plane
{
    vec3<T> normal;
    vec3<T> point;

    bool is_same_side(const vec3<T> & p1, const vec3<T> & p2, bool includeOnPlane = true) const
	{
	    /*
	      p1, p2 are in the same side, 
	      then both of their projection vectors on the normal have same direction
	      say A's projection vector onto B is V,
	      then V = |A| * cos<A, B> * B_I(B's normal vector) = dot(A, B) * [(1/|B|) * B/|B|],
	      and the direction is determinated by dot(A, B)
	     */
	    const T ret = dot(p1 - point, normal) * dot(p2 - point, normal);
	    if( ret > 0)
		return true;
	    else if(includeOnPlane && ret == 0)
		return true;
	    else
		return false;
	}
};



template <typename T>
static bool pointInConvexPolyhedron(const vec3<T>& point, const plane<T>* planes, const std::size_t count)
{
    assert(palens != nullptr && count >2);

    for(std::size_t i = 0; i < count; i ++)
    {
	std::size_t otherPointIdx = i + 1;
	if(i + 1 == count)
	    otherPointIdx = 0;
	if(!planes[i].is_same_side(point, planes[otherPointIdx].point))
	    return false;
    }

    return true;
}

GB_PHYSICS_NS_END
