#pragma once

#include "type.h"
#include "math.h"

/*
  COLUMN MAJOR ORDER

  since, row * A = row[0] * A.rows[0] + row[1] * A.rows[1] ... row[n] * A.rows[n] = ROW(linear combination of A's rows)
  A * col = col[0] * A.cols[0] + col[1] * A.cols[1] ... col[n] * A.cols[n] = COL(linear combination of A's cols)
  and column asscessing is the most convenient operation.
  so, treat all matrix multiplication as A * col case.
*/

GB_PHYSICS_NS_BEGIN

template<typename T>
struct mat2
{
    typedef vec2<T> col_type;
    col_type value[2];
    mat2(const T diagonalValue):
	value{
	{diagonalValue, 0},
	    {0, diagonalValue}
    }
    {}
    const col_type& operator[](const std::uint8_t idx) const
    {
	assert(idx <= 1);
	return value[idx];
    }
    col_type& operator[](const std::uint8_t idx)
    {
	assert(idx <=1);
	return value[idx];
    }
};
template<typename T>
mat2<T>vec2ColMultiplyRow(const vec2<T>& col, const vec2<T>& row)
{
    mat2<T> ret;

    ret[0] = col * row[0];
    ret[1] = col * row[1];

    return ret;
}

template <typename T>
struct mat3
{
    typedef vec3<T> col_type;
    col_type value[3];
    mat3(const T diagonalValue = 1):
	value
    {
	{diagonalValue, 0, 0},
	    {0, diagonalValue, 0},
		{0, 0, diagonalValue}
    }
    {}
    const col_type& operator[](const std::uint8_t idx) const
	{
	    assert(idx <= 2);
	    return value[idx];
	}

    col_type& operator[](const std::uint8_t idx)
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

    mat3 eigenvectors() const
	{
	    /*
	      QR algorithm ref@https://en.wikipedia.org/wiki/QR_decomposition
	      A = Q * R, Q is othogonal matrix, R is upper triangluar matrix
	      let, Ak = Qk * Rk
	      then form Ak+1 = Rk * Qk = Qk(-1) * Qk * Rk * Qk = Qk(-1) * Ak * Qk
	      so, Ak+1 is similar to Ak
	      keep iterating until Ak is converged to a triangluar matrix

	      1st. decomposite A with Q and R
	      2nd. iterating

	      qr_decomposition(Using Householder reflection):
					    |
	       	       	       	       	X1 /|
				          / |
				         /  |
	       	       	       	        o---+----------
					 \  |
				       X1'\ |
					   \|
					    |V
		       +--+--+--+--+--...------+
		       |X +--+  |  |           .
		       |1 |  |  |  |	       .
		       |  |X +--+  |   	       .
		       |  |2 |  |  | 	       |
		       |  |  |X +--+--...------+
		       |  |  |3 |X | 	       |
		       |  |  |	|4 | 	    +--+
		       |  |  |	|  | 	    |Xn|

              Xi is ith column of A
	      X1'(|X1|* e1) is X1's reflection about the plane which orthogonal with Vector V(Unit vector)
	      e1 is (1, 0, 0..., 0), e2 is (1, 0, 0... 0), en is (1)
		     |		 |	    |	       |	  |
		     +----n------+	    +----n-1---+	  1

	      let Xi be the column of A, if Xi' is transformed to be the basis of the A, then
	      A is transformed to be a triangular matrix.

	      let X1' = |X1|e1, I is identity matrix, and V is point to X1, then V = (-X1' + X1)/|V| = (X1 - |X1|e1)/|V|
	      X1' = X1 - 2 * cos<X1, V> * |X1| * V
	          = X1 - 2 * (X1 dot V / |X1||V|) * |X1| * V
		  = X1 - 2 * (X1 dot V) * V / |V| (V is unit vecotr, so |V| is 1)
		  = X1 - V * 2(X1 dot V) (convert to matrix multiply)
		  = X1 - V . 2(Vt . X1) = I . X1 - 2 . V . Vt . X1 = (I - 2.V.Vt) . X1
		  
	      let H = I - 2VVt,
              then X1' = H1 . X1
	      ...
	      
	      eventually Hn ... H3.H2.H1 . A = R(triangluar matrix)
	      ...
	      A = H1(-1).H2(-1).H3(-1)...Hn(-1) . R
	      let Q = H1(-1)H2(-1)H3(-1)...Hn(-1)
	      because Hn is othogonal matrix
	      then
	      Q = H1tH2t...Hnt
	      
	    */

	    std::function<void(const mat3<T>&, mat3<T>&, mat3<T>&)> qr_decomposition = [](const mat3<T>& A, mat3<T>& Q, mat3<T>& R)
		{
		    // 1st col
		    mat3<T> H1;
		    
		    vec3<T>& col1 = A[0];
		    if(col1[1] != 0 || col2[2] !=0)
			{
			    vec3<T> e1{1, 0, 0};
			    vec3<T> V = col1 - col1 * (col1.module());
			    V = V / V.module();

			    mat3<T> I(1);
			    H1 = I - vec3ColMultiplyRow(V, V) * 2;
			}
		    
		    // 2nd col
		    mat3<T> H2;

		    vec2<T>& col2{A[1][1], A[1][2]};
		    if(col2[1] != 0)
			{
			    vec2<T> e2{1, 0};
			    vec2<T> V = col2 - col2 * (col2.module());
			    V = V / module();

			    mat2<T> I{{1}};
			    mat2<T> h2 = I - vec2ColMultiplyRow(V, V) * 2;

			    H2[1][1] = h2[0][0];
			    H2[1][2] = h2[0][1];
			    H2[2][1] = h2[1][0];
			    H2[2][2] = h2[1][1];
			}

		    R = H2 * H1;
		    Q = H1.transpose() * H2.transpos();
		}

	    mat3<T> A = *this;

	    mat3<T> Q, R;
	    while(A[0][1] != 0 || A[0][2] == 0 || A[1][2] == 0)
		{
		    qr_decomposition(A, Q, R);
		    A = R * Q;
		}

	    return 
	}
};

template <typename T>
mat3<T> vec3ColMultiplyRow(const vec3<T>& col, const vec3<T>& row)
{
    mat3<T> ret;
    ret[0] = col * row[0];
    ret[1] = col * row[1];
    ret[2] = col * row[2];

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
	return value[0] * v[0] + value[1] * v[1] + value[2] * v[2] + value[3] * v[3];
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
