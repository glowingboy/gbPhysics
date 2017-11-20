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
  };
};
