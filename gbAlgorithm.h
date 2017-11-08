#pragma once

namespace gb
{
  namespace Algorithm
  {
      /*
       *@brief quick select most n large/small, and store them in origin array from idx:0 to idx:n
       *@tparam Compare a function object type, 
       *possible like:
       *compFunc(const Data& l, const Data& r){ return l <= r; }
       *@tparam Swap exchange two elements' position
       *@param data input array
       *@param count how many elements you want to select
       */
      template<typename Data, typename Compare>
      static void quickSelectMost(Data* data, const std::uint32_t leftIdx, const std::uint32_t rightIdx, const std::uint32_t count)
      {
	  if(leftIdx >= rightIdx)
	      return;
	  if(count == 0)
	      return;
	  Compare cmpr;
	  std::uint32_t pivotIdx = leftIdx;
	  Data pivot = data[pivotIdx];
	  
	  std::uint32_t i = leftIdx;
	  std::uint32_t j = rightIdx;

	  for(;;)
	  {
	      for(; i < j; j--)
	      {
		  if(!cmpr(pivot, data[j]))
		  {
		      data[i++] = data[j];
		      break;
		  }

	      }

	      for(; i < j; i++)
	      {
		  if(!cmpr(data[i], pivot))
		  {
		      data[j--] = data[i];
		      break;
		  }
	      }

	      if( i == j)
		  break;
	  }

	  data[i] = pivot;

	  const std::uint32_t curCount = i - leftIdx + 1;
	  
	  if( curCount >= count)
	      return;
	  else
	  {
	      quickSelectMost<Data, Compare>(data, i + 1, rightIdx, count - curCount);
	  }
      }
  }
};
