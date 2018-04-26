#pragma once

#include "plane.h"
#include "boundingbox.h"

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

    bool intersect_obb(const obb<T> & dst, float (&t)[2])
    {
	// slab method
	// ref https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

	/*
	  using method to check every view of three views
	 */
	
	// check other two slabs which are parallel to current slab's normal
	const obb<T>::slab & slab0 = dst.slabs[0];
	const obb<T>::slab & slab1 = dst.slabs[1];
	const obb<T>::slab & slab2 = dst.slabs[2];
	// 0
	float t[2] = {0};
	if(intersect_plane(plane{slab1.normal, slab1.points[0]}, t[0]))
	    {
		if(t[0] == 0)	// ray is in the plane
		    return true;
		else
		    {
			if(intersect_plane(plane(slab1.normal, slab1.points[1]), t[1]))
			    {
				assert(t[1] != 0);
				
			    }
			else
			    assert(false);
		    }
	    }

    }

    bool intersect
    vec3<T> origin;
    vec3<T> direction;
};

typedef ray<Float> rayF;


GB_PHYSICS_NS_END
