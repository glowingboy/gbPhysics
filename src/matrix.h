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
    
    mat3 && operator / (const T scalar) &&
	{
	    value[0] /= scalar;
	    value[1] /= scalar;
	    value[2] /= scalar;

	    return std::move(*this);
	}
    
    void operator += (const mat3 & o)
	{
	    col_type (&o_val) [3] = o.value;
	    value[0] += o_val[0];
	    value[1] += o_val[1];
	    value[2] += o_val[2];
	}
};

template <typename T>
mat3<T> vec3ColMultiplyRow(const vec3<T>& col, const vec3<T>& row)
{
    mat3<T> ret;
    for(std::uint8_t i = 0; i < 3; i ++)
    {
	for(std::uint8_t j = 0; j < 3; j ++)
	{
	    ret[i][j] = col[i] * row[j];
	}
    }
    return ret;
}

template<typename T>
mat3<T> covarianceMat3(const vec3<T>* data, const std::size_t count)
{
    vec3<T> mean = meanVec3<T>(data, count);

    /* 
       C = E((P - E(P))(P - E(P))_T)
       C = SUM((P - E(P))(P - E(P))_T) / count
    */

    mat3<T> ret;

    for(std::size_t i = 0; i < count; i++)
    {
	ret += (vec3ColMultiplyRow(data[i], mean) / count);
    }

    return ret;
}

template <typename T>
mat3<T> naturalAxes(const vec3<T>* data, const std::size_t count)
{
    /*
      using PCA method(primary component analyze)
      calculating covariance_matrix(x, y, z) of set of points
      called C
      | row\col | 1   | 2   | 3   |
      |---------+-----+-----+-----|
      |       1 | x-x | x-y | x-z |
      |---------+-----+-----+-----|
      |       2 | y-x | y-y | y-z |
      |---------+-----+-----+-----|
      |       3 | z-x | z-y | z-z |
      
      if matrix A consisted of a new basis transform the points
      so that the C is diagonal matrix which means
      there will be no correlation between any two coordinates of the points
      and that new basis will be the natural axes
      
      let P be the set of points, 
      then
      C = E((P - E(P))(P - E(P))_T)
      so
      C' = E((A * P - E(A * P))( A * p - E(A * P ))_T)
         = E(A(P - E(P)) A_T(P - E(P))_T)
	 = A * E((P - E(P))(P - E(P))_T) * A_T
	 = A * C * A_T
      since C is sysmetric matrix, so the eigenvectors of C are orthogonal,
      so there exists a matrix B(consisted of eigenvectors) 
      which let B * C * B_1 = B * C * B_T = diagonalized(C) = C'
      so A need to be the B
      
      then the problem turns out to find eigenvectors of C
    */

    // covariance matrix
    mat3<T> covM = covarianceMat3<T>(data, count);

    // eigenvector
}

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
