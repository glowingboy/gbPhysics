#pragma once

#include "plane.h"

GB_PHYSICS_NS_BEGIN

template < typename T>
struct ray
{
    ray(){}
    
    ray(const vec3<T>& origin_point):
	origin(origin_point)
	{}
    
    ray(const vec3<T>& origin_point, const vec3<T>& other_point):
	origin(origin_point),
	direction(other_point - origin_point)
	{
	}

    void update(const vec3<T>& other_point)
	{
	    direction = other_point - origin;
	}

    bool intersect_plane(const plane<T>& dst, float & t)
	{
	    /*
	      suppose, I is intersection point, O is ray's origin, D is ray's direction
	      N is plane's normal vector, P is a point in plane
	      I = O + tD;
	      N dot (I - P) = 0, plugging I with I = O + tD
	      => t = N(P-O)/ND, (when ND != 0)
	    */
	    const T dp =  dot(dst.normal, direction);
	    if(dp != 0)
	    {
		t = dot(dst.normal, dst.point - origin) / dp;
		return true;
	    }
	    else		// direction is perpendicular to normal
	    {
		if(dot(dst.normal, dst.point - origin) == 0) // ray is in the plane
		{
		    t = 0;
		    return true;
		}
		else		// ray missed the plane
		    return false;
	    }
	}

    bool is_same_side(const vec3<T>& p1, const vec3<T>& p2)
	{
	    const vec3<T> p1X = cross(direction, p1 - origin);
	    const vec3<T> p2X = cross(direction, p2 - origin);

	    return dot(p1X, p2X) < 0 ? false : true;
	}

    bool intersect_obb()

    bool intersect
    vec3<T> origin;
    vec3<T> direction;
};

typedef ray<Float> rayF;


GB_PHYSICS_NS_END
