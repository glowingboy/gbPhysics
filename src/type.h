// fundamental types
#include "physicsNS.h"
#include <cfloat>
#include <cstdint>
#include <cstdlib>
GB_PHYSICS_NS_BEGIN

// ref: https://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
union float_t
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
    float_t(const float f_):f(f_) {}
    float_t():f(.0f) {}
    
    // relative epsilon method
    inline bool operator==(const float_t& o) const
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

    inline bool operator <=(const float_t & o) const
    {
	return (this->f < o.f) || (this->f == o.f);
    }
    inline bool operator >=(const float_t & o) const
    {
	return (this->f > o.f) || (this->f == o.f);
    }

    inline operator float&()
    {
	return f;
    }

};

GB_PHYSICS_NS_END

