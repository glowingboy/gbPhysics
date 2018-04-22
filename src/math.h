#pragma once
#include <cstdint>
#include <cmath>
#include <cstring>
#include <type_traits>
#include <limits>
//#include <stdexcept>

#define GB_MATH_PI 3.141592f

namespace gb
{
    namespace number_conversion
    {
	// ref: http://en.cppreference.com/w/cpp/language/implicit_conversion
	
	template<typename dst_t, typename src_t>
	dst_t integer2unsigned(const src_t src)
	{
	    static_assert(std::is_integral<src_t>::value, "cast2unsigned(const src_t src), src_t must be an integer");
	    /*
	      well-defined in standard, 
	      dst = src mod 2^n, where n is number of bits in dst
	    */
	    return src;
	}

	template<typename dst_t, typename src_t>
	dst_t clamp2(const src_t src)
	{
	    static_assert(std::is_signed<src_t>::value, "clampcast(const src_t src), src_t must be an signed type");
#ifdef _MSC_VER
#undef max
#undef min
#endif
	    if(src > 0)
		return src <= std::numeric_limits<dst_t>::max() ? src : std::numeric_limits<dst_t>::max();
	    else
		return src >= std::numeric_limits<dst_t>::min() ? src : std::numeric_limits<dst_t>::min();
	}


	template<typename dst_t, typename src_t>
	dst_t integer2signed(const src_t src)
	{
	    /*
	      if src can be represented in dst_t, then dst = src,
	      else it's implemention-defined
	    */
	    static_assert(std::is_integral<src_t>::value, "cast2signed(const src_t src), src_t must be an integer type");
	    
	    return clamp2<dst_t>(src);
	}

	template<typename dst_t, typename src_t>
	dst_t floatpoint2floatpoint(const src_t src)
	{
	    /*
	      if src can be represented in dst_t, then dst = src,
	      else if between of two representable value,
	      then dst = one of two, still implemention-defined,
	      otherwise, it's undefined behavior
	    */
	    static_assert(std::is_float_point<src_t>::value, "floatpoint2floatpoint(const src_t src), src_t must be a float point type");

	    return clamp2<dst_t>(src);
	}

	template<typename dst_t, typename src_t>
	dst_t floatpoint2integer(const src_t src)
	{
	    /*
	      if src fits into dst_t, then fractional part is discarded, else it's undefined behavior
	    */
	    static_assert(std::is_float_point<src_t>::value, "floatpoint2integer(const src_t src), src_t must be a float point type");
	    
	    return clamp2<dst_t>(src);
	}

	template<typename dst_t, typename src_t>
	dst_t integer2floatpoint(const src_t src)
	{
	    /*
	      if src fits into dst_t, then src will be rounded into dst, otherwise it's implemention-defined
	    */
	    
	    static_assert(std::is_integral<src_t>::value, "integer2floatpoint(const src_t src), src_t must be an integer type");

	    return clamp2<dst_t>(src);
	}
	    
    }

    namespace math
    {
	template<typename T, std::uint8_t dimension>
	T square_distance(const T* a , const T* b)
	{
	    static_assert(std::is_signed<T>::value, "math::square_distance(const T* a , const T* b), T must be a signed type");
	    T ret;
	    memset(&ret, 0, sizeof(T));

	    for(std::uint8_t i = 0; i < dimension; i++)
	    {
		const T dist = a[i] - b[i];
		ret += (dist * dist);
	    }

	    return ret;
	}

	template<typename A, typename B>
	class interval_mapper
	{
	    static_assert(std::is_signed<A>::value && std::is_signed<B>::value, "math::interval_mapper, both A and B must be a signed type");
	public:
	    interval_mapper() = delete;
	    interval_mapper(const A interval_a_l, const A interval_a_r, const B interval_b_l, const B interval_b_r):
		_interval_a_l(interval_a_l),
		_interval_b_l(interval_b_l)
		{
		    assert((interval_a_r - interval_a_l) != 0);
		    _scale = (double)(interval_b_r - interval_b_l) / ( interval_a_r - interval_a_l);
		}
	    B map(const A value)
		{
		    return _interval_b_l + (B)((value - _interval_a_l) * _scale);
		}
	private:
	    const A _interval_a_l;
	    const B _interval_b_l;
	    double _scale;
	};

	inline float degree2radian(float degree)
	{
	    return GB_MATH_PI * (degree / 180.0f);
	}
    }
}
