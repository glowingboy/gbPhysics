// fundamental types

#pragma once

#include "physicsNS.h"
#include <cassert>
#include <array>
#include <cfloat>
#include <cstdint>
#include <cmath>
#include <limits>
#include <vector>
#include <type_traits>
#include <cstring>
GB_PHYSICS_NS_BEGIN

//**************** fundamental type ****************

// ref: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
union Float
{
    float f;
#if ! defined(NDEBUG)
    struct			// little endian
    {
	std::uint32_t mantissa : 23;
	std::uint32_t exponent : 8;
	std::uint32_t sign : 1;
    }parts;
#endif
    template <typename T>
    constexpr Float(const T f_) noexcept : f((float)f_){}
    
    Float():f(.0f) {}
    // relative epsilon method
    inline bool operator==(const Float& o) const
	{
	    float diff = std::abs(this->f - o.f);
	    if(diff <= FLT_EPSILON)	// for the diff every close to zero
		return true;

	    float a = std::abs(this->f);
	    float b = std::abs(o.f);

	    float largest = a > b? a : b;
	    if(diff <= largest * FLT_EPSILON)
		return true;

	    return false;
	}
    template<typename T>
    bool operator==(const T& o) const
    	{
    	    return operator==(Float(o));
    	}

    inline bool operator!=(const Float& o) const
	{
	    return ! operator==(o);
	}
    template<typename T>
    bool operator!=(const T& o) const
    	{
    	    return operator!=(Float(o));
    	}
    
    inline bool operator <=(const Float & o) const
	{
	    return (this->f < o.f) || (this->f == o.f);
	}
    template<typename T>
    bool operator <=(const T & o) const
	{
	    return operator<=(Float(o));
	}
    
    inline bool operator >=(const Float & o) const
	{
	    return (this->f > o.f) || (this->f == o.f);
	}
    template <typename T>
    bool operator >=(const T& o) const
	{
	    return operator>=(Float(o));
	}
    
    inline constexpr operator const float& () const
    	{
    	    return f;
    	}
    inline operator float& ()
	{
	    return f;
	}
};

template <typename T>
class is_Float : public std::false_type{} ;

template<> class is_Float<Float> : public std::true_type {};
//**************** end of fundamental type ****************

class bit_vector
{
public:
    inline bit_vector() :
	_data(nullptr),
	_curSize(0),
	_capacity(0)
	{}
    ~bit_vector();
    void reserve(const std::size_t capacity);
    void insert(const std::size_t beginIdx, const std::size_t size, const std::uint8_t bitVal);
    std::uint8_t operator[](const std::size_t index) const;
    void clear();
private:
    std::uint8_t* _data;
    std::size_t _curSize;//bit size
    std::size_t _capacity;//bit capacity
private:
    //save from right to left( the higher index the more left-shift)
    static inline void _set_byte_uint(std::uint8_t& byte, const std::uint8_t index, const std::uint8_t bitVal)
	{
	    assert(index <= 7);
	    assert(bitVal <= 1);

	    byte |= bitVal << index;
	}

    static inline std::uint8_t _get_byte_uint(const std::uint8_t& byte, const std::uint8_t index)
	{
	    assert(index <= 7);

	    const std::uint8_t mask = 1 << index;
	    return (byte & mask) >> index;
	}

};
/*
 *@brief, a static 2d array type
 */
template<typename T>
class array_2d
{
private:
    struct _proxy
    {
	_proxy(T* data, const std::uint32_t col) :
	    _data(data),
	    _col(col)
	    {}
	T& operator[](const std::uint32_t colIdx)
	    {
		assert(colIdx < _col);
		return _data[colIdx];
	    }

	T* const _data;
	std::uint32_t _col;
    };

public:
    ~array_2d()
	{
	    delete[] _data;
	    _data = nullptr;
	}
    array_2d() :
	row(0),
	col(0),
	_data(nullptr)
	{}
    array_2d(const std::uint32_t row_, const std::uint32_t col_) :
	row(row_),
	col(col_),
	_data(new T[row_ * col_]{ 0 })
	{}

    template<typename ... Args>
    array_2d(const std::uint32_t row_, const std::uint32_t col_, Args ... args) :
	row(row_),
	col(col_),
	_data(new T[row_ * col_]{ args ... })
	{}

    array_2d(array_2d&& other) :
	row(other.row),
	col(other.col),
	_data(other._data)
	{
	    other._data = nullptr;
	}

    void operator=(array_2d&& other)
	{
	    row = other.row;
	    col = other.col;
	    _data = other._data;
	    other._data = nullptr;
	}
    _proxy operator[](const std::uint32_t rowIdx)
	{
	    assert(rowIdx < row);
	    return _proxy(_data + rowIdx * col, col);
	}

    void realloc(const std::uint32_t row_, const std::uint32_t col_)
	{
	    std::uint32_t old_row = row;
	    std::uint32_t old_col = col;
	    row = row_;
	    col = col_;
	    T* tmp = new T[row * col]{ 0 };
	    if (_data != nullptr)
	    {
		if (old_row > row)
		    old_row = row;
		if (old_col > col)
		    old_col = col;

		for (std::uint32_t i = 0; i < old_row; i++)
		{
		    for (std::uint32_t j = 0; j < old_col; j++)
		    {
			tmp[i][j] = _data[i][j];
		    }
		}

		delete[] _data;
		_data = nullptr;
	    }

	    _data = tmp;
	}

    void assign(const std::uint32_t row_, const std::uint32_t col_, T* data)
	{
	    row = row_;
	    col = col_;
	    delete[] _data;
	    _data = data;
	}

    /*
     *@param, location, location[0]: col idx, location[1]: row idx.
     */
    void insert(const std::array<std::uint32_t, 2>& location, array_2d<T>& other)
	{
	    const std::uint32_t o_x = location[0];
	    const std::uint32_t o_y = location[1];
	    const std::uint32_t height = other.height;
	    const std::uint32_t width = other.width;

	    for (std::uint32_t i = 0; i < height; i++)
	    {
		for (std::uint32_t j = 0; j < width; j++)
		{
		    this->operator[](o_y + i)[o_x + j] = other[i][j];
		}
	    }
	}

    const T* data()const { return _data; }
    union
    {
	std::uint32_t row;
	std::uint32_t height;
	std::uint32_t y;
    };

    union
    {
	std::uint32_t col;
	std::uint32_t width;
	std::uint32_t x;
    };
private:
    T* _data;
};

template<typename T>
struct is_scalar : public std::false_type {};

template<>
struct is_scalar<signed char> : public std::true_type {};

template<>
struct is_scalar<unsigned char> : public std::true_type {};

template<>
struct is_scalar<int> : public std::true_type {};

template <>
struct is_scalar<unsigned int> : public std::true_type {};

template <>
struct is_scalar<float> : public std::true_type {};
    
template<>
struct is_scalar<Float> : public std::true_type {};


template<typename T>
struct vec2
{
    static_assert(std::is_signed<T>::value, "vec2<T>, T must be a signed type");
    union
    {
	struct { T x, y; };
	struct { T r, g; };
	struct { T s, t; };
    };
    vec2():x(0),y(0){}
    vec2(T x_, T y_):
	x(x_),
	y(y_)
	{}
    
    T& operator[](std::uint8_t idx)
	{
	    assert(idx < 2);
	    return ((&(this->x))[idx]);
	}

    const T& operator[](std::uint8_t idx)const
	{
	    assert(idx < 2);
	    return ((&(this->x))[idx]);
	}
    bool operator<(const vec2& o)
	{
	    return (x < o.x) && (y < o.y);
	}
    bool operator>(const vec2& o)
	{
	    return (x > o.x) && (y > o.y);
	}
    bool operator<=(const vec2& o)
	{
	    return (this->x <= o.x) && (this-y <= o.y);
	}
    bool operator>=(const vec2& o)
	{
	    return (this->x >= o.x) && (this->y >= o.y);
	}

    vec2 operator - (const vec2 & o) const
	{
	    return vec2(x - o.x, y - o.y);
	}
    void operator -=(const vec2 & o)
	{
	    x -= o.x;
	    y -= o.y;
	}
    template <typename S>
    vec2 operator*(const S scalar) const
	{
	    return vec2(x * scalar, y * scalar);
	}

    template <typename S>
    void operator*=(const S scalar)
	{
	    x *= scalar;
	    y *= scalar;
	}

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec2>::type operator /(const S scalar) const
	{
	    return vec2(x / scalar, y / scalar);
	}
    
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec2&>::type operator /=(const S scalar)
	{
	    x /= scalar;
	    y /= scalar;

	    return *this;
	}
    void identitylization()
	{
	    operator/=(module());
	}
    vec2 identity() const
	{
	    return operator/(module());
	}
    T module() const
	{
	    return std::sqrt(x * x + y * y);
	}
};

typedef vec2<Float> vec2F;

template<typename T>
struct vec3
{
    static_assert(std::is_signed<T>::value, "vec3<T>, T must be a signed type");
    union
    {
	struct { T x, y, z; };
	struct { T r, g, b; };
	struct { T s, t, u; };
    };
    vec3():x(0),y(0), z(0){}
    vec3(T x_, T y_, T z_):
	x(x_),
	y(y_),
	z(z_)
	{} 
    
    constexpr vec3(const T& o):
	x(o),
	y(o),
	z(o)
	{}

    vec3(const typename std::conditional<is_Float<T>::value, std::vector<float>, std::vector<T>>::type& v):
	x(v[0]),
	y(v[1]),
	z(v[2])
	{
	    assert(v.size() >= 3);
	}

    void operator=(const typename std::conditional<is_Float<T>::value, std::vector<float>, std::vector<T>>::type& v)
	{
	    assert(v.size() >= 3);
	    std::memcpy(this, v.data(), 3 * sizeof(T));
	}
    
    T sqDistance(const vec3 & o) const
	{
	    return std::pow(x - o.x, 2) + std::pow(y - o.y, 2) + std::pow(z - z.z, 2);
	}
    T distance(const vec3 & o) const
	{
	    return std::sqrt(std::pow(x - o.x, 2) + std::pow(y - o.y, 2) + std::pow(z - z.z, 2));
	}
    T sqMagnitude() const
	{
	    return x * x + y * y + z * z;
	}
    T magnitude() const
	{
	    return std::sqrt(sqMagnitude());
	}

    vec3 normalize() const
	{
	    const T mag = magnitude();
	    return vec3(x / mag, y / mag, z / mag);
	}
    T& operator[](std::uint8_t idx)
	{
	    assert(idx < 3);
	    return ((&(this->x))[idx]);
	}

    const T& operator[](std::uint8_t idx)const
	{
	    assert(idx < 3);
	    return ((&(this->x))[idx]);
	}
    vec3 operator+(const vec3& o) const
	{
	    return vec3(x + o.x, y + o.y, z + o.z);
	}
    vec3 & operator +=(const vec3& o)
	{
	    x += o.x;
	    y += o.y;
	    z += o.z;

	    return *this;
	}
    vec3 operator-(const vec3& o) const
	{
	    return vec3(x - o.x, y - o.y, z - o.z);
	}
    void operator-=(const vec3 & o)
	{
	    x -= o.x;
	    y -= o.y;
	    z -= o.z;
	}
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec3>::type operator*(const S scalar) const
	{
	    return vec3(x * scalar, y * scalar, z * scalar);
	}
    template <typename S>
    typename std::enable_if<is_scalar<S>::value, vec3&>::type operator *=(const S scalar)
	{
	    x *= scalar;
	    y *= scalar;
	    z *= scalar;

	    return *this;
	}
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec3>::type operator/(const S scalar) const
	{
	    return vec3(x / scalar, y / scalar, z / scalar);
	}

    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec3&>::type operator/=(const S scalar)
	{
	    x /= scalar;
	    y /= scalar;
	    z /= scalar;
	    return *this;
	}
    void identitylization()
	{
	    operator/=(module());
	}
    vec3 identity() const
	{
	    return operator/(module());
	}
    bool operator<(const vec3& o) const
	{
	    return (x < o.x) && (y < o.y) && (z < o.z);
	}
    bool operator<=(const vec3& o) const
	{
	    return (x <= o.x) && (y < o.y) && (z < o.z);
	}
    bool operator>(const vec3& o) const
	{
	    return (x > o.x) && (y > o.y) && (z > o.z);
	}
    bool operator >=(const vec3& o) const
	{
	    return (x >= o.x) && (y > o.y) && (z >= o.z);
	}

    T module() const
	{
	    return std::sqrt(x*x + y*y + z*z);
	}
};

typedef vec3<Float> vec3F;

template<typename T>
vec3<T> meanVec3(const vec3<T> * data, const std::size_t count)
{
    vec3<T> ret;
    for(std::size_t i = 0; i < count; i++)
    {
	ret += data[i] / count;
    }

    return ret;
}

template <typename T>
T dot(const vec3<T>& l, const vec3<T>& r)
{
    return l.x * r.x + l.y * r.y + l.z * r.z;
}


template <typename T>
struct vec4
{
    static_assert(std::is_signed<T>::value, "vec4<T>, T must be a signed type");
    union
    {
	struct { T x, y, z, w; };
	struct { T r, g, b, a; };
	struct { T s, t, p, q; };
    };

    vec4(): x(0), y(0), z(0), w(0){}
    vec4(T x_, T y_, T z_, T w_): x(x_), y(y_), z(z_), w(w_){}
    
    vec4(const vec3<T>& o):
	x(o.x), y(o.y), z(o.z), w(1)
	{}
    
    vec4(const typename std::conditional<is_Float<T>::value, std::vector<float>, std::vector<T>>::type& v):
	x(v[0]),
	y(v[1]),
	z(v[2]),
	w(v[3])
    {
	assert(v.size() >= 4);
    }

    void operator=(const typename std::conditional<is_Float<T>::value, std::vector<float>, std::vector<T>>::type& v)
    {
	assert(v.size() >= 4);
	std::memcpy(this, v.data(), 4 * sizeof(T));
    }
    
    explicit operator vec3<T>() const
	{
	    return vec3<T>(x, y, z);
	}
    void operator=(const vec3<T>& v3)
	{
	    std::memcpy(this, &v3, 3 * sizeof(T));
	}

    const T & operator[](const std::uint8_t idx) const
	{
	    assert(idx <=3);
	    return (&(this->x))[idx];
	}
    T & operator[] (const std::uint8_t idx)
	{
	    assert(idx <=3);
	    return (&(this->x))[idx];
	}

    vec4<T> operator + (const vec4<T>& o) const
	{
	    return vec4( x + o.x, y + o.y, z + o.z, w + o.w);
	}
    template<typename S>
    typename std::enable_if<is_scalar<S>::value, vec4<T>>::type operator * (const S scalar) const
	{
	    return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
	}
};

typedef vec4<Float> vec4F;

template<typename T>
std::int8_t sign(const T val)
{
    if(val < 0)
	return -1;
    else
	return 1;
}

// some fancy types
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
    vec3<T> origin;
    vec3<T> direction;
};
GB_PHYSICS_NS_END

// std::numeric_limits extension
namespace std
{
    template <>
    class numeric_limits<gb::physics::Float>
    {
    public:
#ifdef _MSC_VER
#undef min
#undef max
#endif
	inline static constexpr gb::physics::Float min() noexcept
	    {
		return gb::physics::Float(std::numeric_limits<float>::min());
	    }
	inline static constexpr gb::physics::Float max() noexcept
	    {
		return gb::physics::Float(std::numeric_limits<float>::max());
	    }
    };
}
