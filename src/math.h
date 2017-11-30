#pragma once
#include <cmath>
#include <cstring>
namespace gb
{
  namespace math
  {
      template<typename T, std::uint8_t dimension>
      T square_distance(const T* a, const T* b)
      {
	  T ret;
	  memset(&ret, 0, sizeof(T));

	  for(int i = 0; i < dimension; i++)
	  {
	      ret += std::pow(a[i] - b[i], 2);
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
		  _scale = (interval_b_r - interval_b_l) / ( interval_a_r - interval_a_l);
	      }
	  B map(const B value)
	      {
		  return _interval_b_l + (value - _interval_a_l) * _scale;
	      }
      private:
	  const A _interval_a_l;
	  const B _interval_b_l;
	  float _scale;
      };

  };
};
