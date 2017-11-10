#pragma once
#include <cmath>

namespace gb
{
  namespace math
  {
      template<typename T, std::uint8_t dimension>
      T square_distance(const T* a, const T* b)
      {
	  T ret;

	  for(int i = 0; i < dimension; i++)
	  {
	      ret += std::pow(a[i] - b[i], 2);
	  }

	  return ret;
      }
  };
};
