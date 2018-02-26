#pragma once

#include "type.h"
#include "math.h"
// column major order

GB_PHYSICS_NS_BEGIN

template <typename T>
struct mat3
{
    typedef vec3<T> col_type;
    col_type value[3];
    const vec3<T>& operator[](const std::uint8_t idx) const
	{
	    assert(idx <= 2);
	    return value[idx];
	}
    
    vec3<T>& operator[](const std::uint8_t idx)
	{
	    assert(idx <= 2);
	    return value[idx];
	}
};

template <typename T>
struct mat4
{
    typedef vec4<T> col_type;
    col_type value[4];
    
    mat4(T diagonalValue = 1):
	value{
	{diagonalValue, 0, 0, 0},
	    {0, diagonalValue, 0, 0},
		{0, 0, diagonalValue, 0},
		    {0, 0, 0, diagonalValue}}
    {}
    
    mat4(const col_type & col0, const col_type & col1, const col_type & col2, const col_type & col3):
	value{col0, col1, col2, col3}
    {}
    const vec4<T>& operator[](const std::uint8_t idx) const
	{
	    assert(idx <= 3);
	    return value[idx];
	}
    vec4<T>& operator[](const std::uint8_t idx)
	{
	    assert(idx <= 3);
	    return value[idx];
	}

    vec4<T> operator * (const vec4<T> & v) const
    {
	vec4<T> ret;
	
	ret[0] = value[0][0] * v[0] + value[1][0] * v[1] + value[2][0] * v[2]  + value[3][0] * v[3];
	ret[1] = value[0][1] * v[0] + value[1][1] * v[1] + value[2][1] * v[2]  + value[3][1] * v[3];
	ret[2] = value[0][2] * v[0] + value[1][2] * v[1] + value[2][2] * v[2]  + value[3][2] * v[3];
	ret[3] = value[0][3] * v[0] + value[1][3] * v[1] + value[2][3] * v[2]  + value[3][3] * v[3];

	return ret;
    }
    mat4 operator * (const mat4 & o) const
    {
	const mat4& self = *this;

	return mat4(self * o[0], self * o[1] , self * o[2], self * o[3]);
    }
    void operator *= (const mat4 & o)
    {
	*this = (*this) * o;
    }
};

typedef mat4<Float> mat4F;

template <typename T>
mat4<T> scaleMat(const vec3<T>& scale)
{
    mat4<T> ret;

    ret[0][0] = scale[0];
    ret[1][1] = scale[1];
    ret[2][2] = scale[2];
    
    return ret;
}

template <typename T>
mat4<T> rotateXAxisMat(const float degree)
{
    mat4<T> ret; 
    float radian = gb::math::degree2radian(degree);

    T sin = std::sin(radian);
    T cos = std::cos(radian);

    ret[1][1] = cos;
    ret[2][1] = -sin;
    ret[1][2] = sin;
    ret[2][2] = cos;

    return ret;
}

template <typename T>
mat4<T> rotateYAxisMat(const float degree)
{
    mat4<T> ret; 
    float radian = gb::math::degree2radian(degree);

    T sin = std::sin(radian);
    T cos = std::cos(radian);

    ret[0][0] = cos;
    ret[2][0] = sin;
    ret[0][2] = -sin;
    ret[2][2] = cos;

    return ret;
}

template <typename T>
mat4<T> rotateZAxisMat(const float degree)
{
    mat4<T> ret; 
    float radian = gb::math::degree2radian(degree);

    T sin = std::sin(radian);
    T cos = std::cos(radian);

    ret[0][0] = cos;
    ret[1][0] = -sin;
    ret[0][1] = sin;
    ret[1][1] = cos;

    return ret;
}

template <typename T>
mat4<T> translateMat(const vec3<T>& v)
{
    mat4<T> ret;

    ret[3][0] = v[0];
    ret[3][1] = v[1];
    ret[3][2] = v[2];
    
    return ret;
}

GB_PHYSICS_NS_END
