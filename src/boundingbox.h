#pragma once
#include "matrix.h"
#include "plane.h"

#include <limits>

GB_PHYSICS_NS_BEGIN
	
template<typename T = float>
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

    spherebb operator *(const mat4<T>& mat) const
	{
	    T maxScale = mat[0][0];
	    T tmp = mat[1][1];
	    if(tmp > maxScale)
		maxScale = tmp;
	    tmp = mat[2][2];
	    if(tmp > maxScale)
		maxScale = tmp;
	    
	    return spherebb((vec3<T>)(mat * vec4<T>(centre)), radius * maxScale);
	}

    void operator *= (const mat4<T>& mat)
	{
	    centre = (vec3<T>)(mat * vec4<T>(centre));
	    
	    T maxScale = mat[0][0];
	    T tmp = mat[1][1];
	    if(tmp > maxScale)
		maxScale = tmp;
	    tmp = mat[2][2];
	    if(tmp > maxScale)
		maxScale = tmp;
	    
	    radius *= maxScale;
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
	
template<typename T = float>
struct aabb
{
    aabb():
	diagonal{vec3<T>(0)},
	lenSide(0)
	{}

    aabb(const vec3<T>& lower, const vec3<T>& upper):
	diagonal{lower, upper},
	lenSide((upper - lower).abs())
	{}

    void set(const vec3<T>& lower, const vec3<T>& upper)
	{
	    diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX] = lower;
	    diagonal[GB_PHYSICS_DIAGONAL_UPPER_IDX] = upper;

	    lenSide = (upper - lower).abs();
	}
    
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

template<typename T>
spherebb<T> genSphereBB(const vec3<T>* data, const std::size_t count)
{
    assert(data != nullptr);
    
    const mat3<T> axes = naturalAxes(data, count);

    const vec3<T>& primaryAxis = axes[0];

    // projA-B = |A| * cos<A,B> = |A| * (A dot B) / (|A||B|) = A dot B / |B|
    T projMin = dot(*data, primaryAxis);
    T projMax = projMin;
    
    const vec3<T>* min = data;
    const vec3<T>* max = min;
    
    for(std::size_t i = 1; i < count; i++)
    {
	const vec3<T>* cur = data + i;

	const T curProj = dot(*cur, primaryAxis);

	if(projMin > curProj)
	{
	    projMin = curProj;
	    min = cur;

	    continue;
	}

	if(projMax < curProj)
	{
	    projMax = curProj;
	    max = cur;
	}
    }

    return spherebb<T>(*min/2 + *max/2, projMax/2 - projMin/2);
}

template <typename T = float>
struct obb
{
    struct slab
    {
	vec3<T> normal;
	vec3<T> points[2];

	bool is_between_slab(const vec3<T> & point) const
	    {
		if(!plane(normal, points[0]).is_same_side(point, points[1]))
		    return false;
		
		return plane(normal, points[1]).is_same_side(point, points[0]);
	    }
    };
    slab slabs[3];	
};

template <typename T>
obb<T> genOrientedBB(const vec3<T>* data, const std::size_t count)
{
    assert(data != nullptr);
    
    obb<T> ret;
    
    const mat3<T> axis = naturalAxes(data, count);

    for(std:uint8_t i = 0; i < 3; i++)
    {
	ret.slabs[i].normal = axis[i].normalize();
    }

    T extend[3][2] = {{0}};

    for(std::uint8_t i = 0; i < 3; i ++)
    {
	obb<T>::slab & curSlab = ret.slabs[i];
	const vec3<T> & normal = curSlab.normal;
	vec3<T> & minP = curSlab.points[0];
	vec3<T> & maxP = curSlab.points[1];
	T & minE = extend[i][0];
	T & maxE = extend[i][1];

	/*
	  for each slab.normal, minP is the point which has smallest
	  projection on the normal, and maxP has the largest.
	      
	  proj = P * P * N / (|P||N|)
	  since |N| = 1, so proj = ((P^T dot P) / |P|) * N;
	*/

	minP = data[0];
	maxP = data[0];
	minE = dot(minP, minP) / minP.magnitude();
	maxE = minE;
	
	for(std::size_t j = 1; j < count; j++)
	{
	    const vec3 & P = data[j];
	    const T projScalar = dot(P, P) / P.magnitude();
	    if(minE > projScalar)
	    {
		minE = projScalar;
		minP = P;
	    }

	    if(maxE < projScalar)
	    {
		maxE = projScalar;
		maxP = P;
	    }
	}
    }
}


GB_PHYSICS_NS_END
