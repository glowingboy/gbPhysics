#pragma once
#include <cstdint>
#include <cmath>
#include <cstring>

#define GB_MATH_PI 3.141592f

namespace gb
{
  namespace math
  {
      template<typename T, std::uint8_t dimension>
      T square_distance(const T* a , const T* b)
      {
	  T ret;
	  memset(&ret, 0, sizeof(T));

	  for(std::uint8_t i = 0; i < dimension; i++)
	  {
	      ret += (T)std::pow(a[i] - b[i], 2);
	  }

	  return ret;
      }

      template<typename A, typename B>
      class interval_mapper
      {
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
  };
};
