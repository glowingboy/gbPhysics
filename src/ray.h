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

    bool is_same_side(const vec2<T>& p1, const vec2<T>& p2)
	{
	    const vec2<T> p1X = cross(direction, p1 - origin);
	    const vec2<T> p2X = cross(direction, p2 - origin);

	    return dot(p1X, p2X) < 0 ? false : true;
	}

    bool intersect_obb(const obb<T> & dst/*, float (&t)[2]*/)
    {
	// slab method
	// ref https://www.siggraph.org/education/materials/HyperGraph/raytrace/rtinter3.htm

	/*
	  1st:
	  pick one pair slabs, find solution of intersection of ray with slabs.
	  The solution will result in 3 cases:
	  a. both intersected

	       | 	 X		    		       |     |	          /
	       |  	/|		    		       |     |	         /
	       |       / |		    		       |     |	        /
	 ------+------X--+---------	    	    -----------+-----+---------X------
	       |     /	 |		    		       |     |	      /
	       |    /	 |		    		       |     | 	     /
	       |   /	 |		    		       |     |      /
	-------+--X------+---------  	    	    -----------+-----+-----X----------
	       | /       |		       		       |     |    /
	       |/        |		       		       |     | 	 /
	       X	 |		       			      	

	       intersected, repeat 1st                           missed, return false

	  b. only one intersected(including ray is in some one plane of slabs)

	      |		|			  |	   |
	------+--A------+---------	     -----+--------+--------->
	      |	        |			  |	   |
       	------+---------+-------->	     -----+--------+----------
       	      |	        |			  |	   |
       	------+-----B---+---------	     -----+--------+----------
	      |		|			  |	   |
	      |		|			  |	   |

	      intersected continue                 missed, return false

	  c. both missed

	    |	   |				 |     | o
	----+------+----------- 	     ----+-----+------
	    |	   |				 |     |
	    |  o   |				 |     |
	----+------+-----------		     ----+-----+------
	    |	   |				 |     |
	    |	   |				 |     |

	    intersected, return true              missed, return false

	  2nd:
	  a: if Max_near > Min_far, then pick an other pair of left two, repeat 1st step,
	  else return false.
	  b: if A, B(both on the slab planes) at the same side of plane which is parallel to slab 
	  and through origin of the ray, then check the other two of three slabs except this slab, 
	  else return false.
	  c: same idea as "is point in convex polygon" 

	 */

	static constexpr std::int8_t missed = -1;
	// 0, 1 for caseB
	static constexpr std::int8_t caseA = 2;
	static constexpr std::int8_t caseC = 3;
	const auto slabCheck = [&](const obb<T>::slab & slabA,
				      const obb<T>::slab & slabB) -> std::int8_t
	    {

		const auto internalCheck = [this](const obb<T>::slab s, float (&t)[2]) -> bool
		{
		    const bool ret0 = intersect_plane(plane{s.normal, s.points[0]}, t[0]);
		    const bool ret1 = intersect_plane(plane{s.normal, s.points[1]}, t[1]);
		    if(ret0 && ret1)
			{
			    assert(t[0] == 0 || t[1] == 0);// if both of two planes are planar?
			    if(t[0] > t[1])
				std::swap(t[0], t[1]);
			    return true;
			}
		    else
			return false;
		    // if(ret0)
		    // 	{
		    // 	    if(t[0] == 0)
		    // 		return false
		    // 	    else if(ret1)
		    // 		{
		    // 		    assert(t[1] == 0);
		    // 		    if(t[0] > t[1])
		    // 			std::swap(t[0], t[1]);

		    // 		    return true;
		    // 		}
		    // 	    else
		    // 		{
		    // 		    assert(false);
		    // 		}
		    // 	}
		    // else if(ret1)
		    // 	{
		    // 	    return caseB;
		    // 	}
		    // else
		    // 	return missed;
		};
		
		float t0[2] = {0.0f};
		const std::int8_t ret0 = internalCheck(slab0, t0);
		
		float t1[2] = {0.0f};
		const bool ret1 = internalCheck(slab1, t1);

		if(ret0 && ret1) // case A
		    {
			if(t0[0] < t1[0])
			    {
				if(t0[1] < t1[0])
				    return missed;
			    }

			return caseA;
		    }
		else if(ret0)	// case B
		    {
			if(plane{slab1.normal, origin}.is_same_side(slab1.points[0], slab1.points[1], false))
			    return missed;
			else
			    return 0;			
		    }
		else if(ret1)	// caseB
		    {
			if(plane{slab0.normal, origin}.is_same_side(slab0.points[0], slab0.points[1], false))
			    return missed;
			else
			    return 0;
		    }
		    return 1;
		else		// case C
		    {
			if(slab0.is_between_slab(origin) && slab1.is_between_slab(origin))
			    return caseC;
			else
			    return missed;
		    }
	    };

	const obb<T>::slab (&slabs) [3] = dst.slabs;
	
	// round 0
	std::int8_t ret = slabCheck(slabs[0], slabs[1]);
	if(ret == missed)
	    return false;
	else if(ret == caseA)
	    {
		ret = slabCheck(slab2, slab0);
		if(ret == missed)
		    return false;
		else
		    return true;
	    }
	else if(ret == 0)
	    {
		ret = slabCheck(slabs[1], slabs[2]);
		if(ret == missed)
		    return false;
		else
		    return true;
	    }
	else if(ret == 1)
	    {
		ret = slabCheck(slabs[0], slabs[2]);
		if(ret == missed)
		    return false;
		else
		    return true;
	    }
	else if(ret == caseC)
	    return true;
    }

    bool intersect_sphere(const spherebb<T>& sbb, float (&t)[2])
    {
	/* 
	   suppose ray: O + tD, 
	   spherebb: (P - C)^2 = R^2, (P is a point on the sphere, C is centre of sphere, and R is radius)
	   plugging ray equation into spherebb
	   (O + tD - C)^2 - R^2 = 0
	   D^2 * t^2 + 2(O-C)D * t + (O-C)^2 - R^2 = 0

	   if t has 0 solution, then no intersection
	   if t has 1 solution, then 1 intersection, which means ray is tangent to the sphere
	   if t has 2 solution, then 2 intersection
	*/
	vec3<T> OminusC = origin - sbb.centre;
	
	gb::math::quadratic_equation qe{dot(direction, direction),
		2 * dot(OminusC, direction),
		dot(OminusC, OminusC) - dot(sbb.radius, sbb.radius)};

	if(qe.discriminant() < 0)
	    return false;
	else
	    return true;
    }

    bool intersect
    vec3<T> origin;
    vec3<T> direction;
};

typedef ray<Float> rayF;


GB_PHYSICS_NS_END
