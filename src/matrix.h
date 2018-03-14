#pragma once

#include "type.h"
#include "math.h"
#include <utility>

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
    static mat2 make_identity()
	{
	    return mat2{{{1, 0}, {0, 1}}};
	}

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
    mat2 operator - (const mat2 & o)
	{
	    mat2 ret = *this;
	    ret -= o;
	    return ret;
	}
    mat2 & operator -= (const mat2 & o)
    {
	const col_type (&o_val) [2] = o.value;

	value[0] -= o_val[0];
	value[1] -= o_val[1];

	return *this;
    }

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat2>::type operator * (const S scalar)
    {
	mat2 ret = *this;

	ret *= scalar;

	return ret;
    }

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat2&>::type operator *= ( const S scalar)
    {
	value[0] *= scalar;
	value[1] *= scalar;

	return *this;
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
    static mat3 make_identity()
	{
	    return mat3{{{1, 0, 0}, {0, 1, 0}, {0, 0, 1}}};
	}
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

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat3&>::type operator *= (const S scalar )
    {
	value[0] *= scalar;
	value[1] *= scalar;
	value[2] *= scalar;

	return *this;
    }

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat3>::type operator * (const S scalar)
	{
	    mat3 ret = *this;
	    ret *= scalar;
	    return ret;
	}
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat3&>::type operator /= (const S scalar)
	{
	    value[0] /= scalar;
	    value[1] /= scalar;
	    value[2] /= scalar;

	    return *this;
	}
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, mat3>::type operator / (const S scalar)
	{
	    mat3 ret = *this;
	    ret /= scalar;
	    return ret;
	}

    mat3 & operator -= (const mat3 & o)
    {
	const col_type (&o_val) [3] = o.value;

	value[0] -= o_val[0];
	value[1] -= o_val[1];
	value[2] -= o_val[2];

	return *this;
    }

    mat3 operator - (const mat3 & o)
	{
	    mat3 ret = *this;
	    ret -= o;
	    return ret;
	}

    col_type operator * (const col_type & col) const
    {
	return value[0] * col[0] + value[1] * col[1] + value[2] * col[2];
    }

    mat3 operator * (const mat3 & o) const
    {
	mat3 ret;
	col_type (&ret_col)[3] = ret.value;
	const col_type (&o_col)[3] = o.value;
	ret_col[0] = operator*(o_col[0]);
	ret_col[1] = operator*(o_col[1]);
	ret_col[2] = operator*(o_col[2]);

	return ret;
    }

    mat3& operator *= (const mat3 & o)
    {
	mat3 ret = operator*(o);

	*this = ret;

	return *this;
    }
    void operator += (const mat3 & o)
	{
	    const col_type (&o_val) [3] = o.value;
	    value[0] += o_val[0];
	    value[1] += o_val[1];
	    value[2] += o_val[2];
	}

    void transposelization()
    {
	// if is symmetric, just return?
	col_type & col0 = value[0];
	col_type & col1 = value[1];
	col_type & col2 = value[2];

	std::swap(col0[1], col1[0]);
	std::swap(col0[2], col2[0]);
	std::swap(col2[1], col1[2]);
    }

    mat3 transpose() const
    {
	mat3 ret(*this);

	col_type (&ret_cols)[3] = ret.value;
	col_type & ret_col0 = ret_cols[0];
	col_type & ret_col1 = ret_cols[1];
	col_type & ret_col2 = ret_cols[2];

	std::swap(ret_col0[1], ret_col1[0]);
	std::swap(ret_col0[2], ret_col2[0]);
	std::swap(ret_col2[1], ret_col1[2]);

	return ret;
    }

    mat3 eigenvectors() const
	{
	    /*
	      TODO qr algorithm with shifts ref@http://people.inf.ethz.ch/arbenz/ewp/Lnotes/chapter4.pdf

	      basic QR algorithm ref@https://en.wikipedia.org/wiki/QR_decomposition
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
		       |X'+--+  |  |           .
		       |1 |  |  |  |	       .
		       |  |X'+--+  |   	       .
		       |  |2 |  |  | 	       |
		       |  |  |X'+--+--...------+
		       |  |  |3 |X'| 	       |
		       |  |  |	|4 | 	   +---+
		       |  |  |	|  |   	   |X'n|

              Xi is ith column of A
	      X1'(|X1|* e1) is X1's reflection about the plane which orthogonal with Vector V(Unit vector)
	      e1 is (1, 0, 0..., 0), e2 is (1, 0, 0... 0), en is (1)
		     |		 |	    |	       |	  |
		     +----n------+	    +----n-1---+	  1

	      let Xi be the column of A, if Xi' is transformed to be the basis of the A, then
	      A is transformed to be a triangular matrix.

	      let X1' = |X1|e1, I is identity matrix, and V is point to X1,
	      then V = (-X1' + X1)/|V| = (X1 - sign(-X1[1])*|X1|e1)/|V|
	      NOTE!!!: sign(-X1[1]) is for avoding X1 - X1' cancellation

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

	    bool bSymmetric = (value[0][1] == value[1][0] && value[0][2] == value[2][0] && value[1][2] && value[2][1]);

	    std::function<void(const mat3<T>&, mat3<T>&, mat3<T>&)> qr_decomposition = [](const mat3<T>& A, mat3<T>& Q, mat3<T>& R)
		{
		    // 1st col
		    mat3<T> H1 = mat3<T>::make_identity();

		    const vec3<T>& col1 = A[0];
		    if(col1[1] != 0 || col1[2] != 0)
			{
			    const vec3<T> e1{1, 0, 0};
			    vec3<T> V = col1 - e1 * (col1.module()) * sign(-col1[0]);
			    V.identitylization();

			    H1 = mat3<T>::make_identity() -= vec3ColMultiplyRow(V, V) *= 2;
			}

		    // 2nd col
		    const mat3<T> A_prime = H1 * A; //H2 . (H1 . A) = R !!!

		    mat3<T> H2 = mat3<T>::make_identity();

		    const vec2<T> col2{A_prime[1][1], A_prime[1][2]};
		    if(col2[1] != 0)
			{
			    const vec2<T> e2{1, 0};
			    vec2<T> V = col2 - e2 * (col2.module()) * sign(-col2[0]);
			    V.identitylization();

			    mat2<T> h2 = mat2<T>::make_identity() -= vec2ColMultiplyRow(V, V) *= 2;

			    H2[1][1] = h2[0][0];
			    H2[1][2] = h2[0][1];
			    H2[2][1] = h2[1][0];
			    H2[2][2] = h2[1][1];
			}

		    R = H2 * A_prime;	// R = H2 . H1 . A = H2 . A'
		    Q = H1.transpose() * H2.transpose();
		};

	    mat3<T> A = *this;
	    mat3<T> ret = mat3<T>::make_identity();
	    mat3<T> Q, R;
	    while(A[0][1] != 0 || A[0][2] != 0 || A[1][2] != 0)
		{
		    qr_decomposition(A, Q, R);
		    A = R * Q;
		    if(bSymmetric)
			ret *= Q;
		}

	    if(!bSymmetric)
		{
		    // TODO
		}
	    return ret;
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

    vec3<T> p_ep;
    for(std::size_t i = 0; i < count; i++)
    {
	p_ep = data[i] - mean;
	ret += (vec3ColMultiplyRow(p_ep, p_ep) / count);
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
    return covM.eigenvectors();
}

template <typename T>
struct mat4
{
    typedef vec4<T> col_type;
    col_type value[4];
    static mat4 make_identity()
	{
	    return mat4{{{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
	}

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

	return mat4{{self * o[0], self * o[1] , self * o[2], self * o[3]}};
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
    mat4<T> ret = mat4<T>::make_identity();

    ret[0][0] = scale[0];
    ret[1][1] = scale[1];
    ret[2][2] = scale[2];

    return ret;
}

template <typename T>
mat4<T> rotateXAxisMat(const float degree)
{
    mat4<T> ret = mat4<T>::make_identity();
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
    mat4<T> ret = mat4<T>::make_identity();
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
    mat4<T> ret = mat4<T>::make_identity();
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
    mat4<T> ret = mat4<T>::make_identity();

    ret[3][0] = v[0];
    ret[3][1] = v[1];
    ret[3][2] = v[2];

    return ret;
}

GB_PHYSICS_NS_END
