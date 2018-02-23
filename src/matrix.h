#pragma once

#include "type.h"

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
	{0, 0, 0, diagonalValue}
    }
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
};

typedef mat4<Float> mat4F;

template <typename T>
mat4<T> scaleMat(const vec3<T>& scale)
{
    mat4<T> ret;
    for(std::uint8_t i = 0; i < 3; i ++)
    {
	ret[i][i] = scale[i];
    }
    return ret;
}

template<typename T>
mat4<T> rotateXAxisMat(const T float degree)
{
    
}



GB_PHYSICS_NS_END
