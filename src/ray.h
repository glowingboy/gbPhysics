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

    bool intersect_obb(const obb<T> & dst, float (&t)[2])
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
	static constexpr std::int8_t caseA = 2;
	static constexpr std::Int8_t caseB = 3;
	static constexpr std::int8_t caseC = 4;
	const auto slabCheck = [&](const obb<T>::slab & slabA,
				      const obb<T>::slab & slabB,
				      float (&t)[2]) -> std::int8_t
	    {

		const auto internalCheck = [this](const obb<T>::slab s, float (&t)[2]) -> std::int8_t
		{
		    const bool ret0 = intersect_plane(plane{s.normal, s.points[0]}, t[0]);
		    const bool ret1 = intersect_plane(plane{s.normal, s.points[1]}, t[1]);
		    if(ret0)
			{
			    if(t[0] == 0)
				return caseB;
			    else if(ret1)
				{
				    assert(t[1] == 0);
				    if(t[0] > t[1])
					std::swap(t[0], t[1]);

				    return caseA;
				}
			    else
				{
				    assert(false);
				}
			}
		    else if(ret1)
			{
			    return caseB;
			}
		    else
			return missed;
		};
		
		float tA[2] = {0.0f};
		const std::int8_t retA = internalCheck(slabA, tA);
		
		float tB[2] = {0.0f};
		const bool retB = internalCheck(slabB, tB);

		if(retA == caseA && retB == caseA)
		    {
			if(tA[0] > tB[0])
			    {
				if(tA[1] > tB[1])
				    return missed;
			    }
			if(tB[0] > tA[0])
			    {
				if(tB[1] > tA[1])
				    return missed;
			    }
			return caseA;
		    }

		if(retA == missed && retB == missed)
		    {
			if(slabA.is_between_slab(origin) && slabB.is_between_slab(origin))
			    return caseC;
			else
			    return missed;
		    }

		if(retA == caseB)
		    {
			if(plane{slabB.normal, origin}.is_same_side(slabB.points[0], slabB.points[1], false))
			    return missed;
			else
			    return 0; // slabA
		    }
		else
		    {
			assert(retB == caseB)
			if(plane{slabA.normal, origin}.is_same_side(slabA.points[0], slabA.points[1], false))
			    return missed;
			else
			    return 1; // slabB
		    }
	    };

	
	const obb<T>::slab & slab0 = dst.slabs[0];
	const obb<T>::slab & slab1 = dst.slabs[1];
	const obb<T>::slab & slab2 = dst.slabs[2];
	// 0
	std::int8_t ret = slabCheck(slab1, slab2);
	if(ret == caseA)
	    {
		ret = slabCheck(slab2, slab0);
		if(ret == missed)
		    return false;
		else
		    return true;
	    }

	else if(ret == missed)
	    return missed;
	else if(ret == caseC)
	    return true;
	else
	    {
		assert(ret == 0 || ret == 1);
		const std::int8_t finalRet = slabCheck(slab0, slabs[ret]);
		if(ret != missed)
		    return true;
		else
		    return false;
	    }
    }

    bool intersect
    vec3<T> origin;
    vec3<T> direction;
};

typedef ray<Float> rayF;


GB_PHYSICS_NS_END
