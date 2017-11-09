#pragma once
#include <string>
#include <algorithm>
#include <cassert>
namespace gb
{
  namespace algorithm
  {
      template<typename Data>
      struct tree_node
      {
	  tree_node* l;
	  tree_node* r;
	  Data data;
	  size_t tree_size()
	      {
		  size_t size = 1;
		  if(l != nullptr)
		      size += l->tree_size();
		  if(r != nullptr)
		      size += r->tree_size();

		  return size;
	      }
      };
      
      template<typename Data, typename GetKey>
      struct kd_compare
      {
	  bool operator()(const Data& l, const Data& r)
	      {
		  return getkey(l, d) <= getkey(r, d);
	      }

	  kd_compare(const std::uint8_t d_):
	      d(d_)
	      {}
	  kd_compare(const kd_compare& other):
	      d(other.d)
	      {}
	  void operator=(const kd_compare& other)
	      {
		  d = other.d;
	      }
	  std::uint8_t d;
	  GetKey getkey;
      };
      
      template<typename Data, std::uint8_t k_, typename KDCompare>
      struct kd_node: public tree_node<Data>
      {
	  static_assert(k_ !=0, "kd_node k can't be 0");
	  kd_node(Data* data_, size_t size, size_t depth = 0)
	      {
		  assert((data_ != nullptr && size != 0));

		  d = depth % k_;

		  //selecte median
		  size_t halfSize = size / 2;
		  std::nth_element<Data*,  KDCompare>(data_,
							    /*medianIdx = halfSize -1 + 1, for both odd and even size*/
							    data_ + halfSize,
							    data_ + size - 1, KDCompare(d));

		  this->data = data_[halfSize];
		  
		  depth++;
		  
		  if(halfSize > 0)
		      this->l = new kd_node<Data, k_, KDCompare>(data_, halfSize, depth);
		  else
		      this->l = nullptr;

		  const size_t rSize = size - 1 - halfSize;
		  if( rSize > 0)
		      this->r = new kd_node<Data, k_, KDCompare>(data_ + halfSize + 1,
								 rSize, depth);
		  else
		      this->r = nullptr;
	      }
	  std::uint8_t d;
      };
  }
}
