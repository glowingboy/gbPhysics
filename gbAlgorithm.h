#pragma once

namespace gb
{
  namespace Algorithm
  {
      /*
       *@brief default compare function object type for basic type
       */
      template<typename DataType>
      struct DefaultCompare
      {
	  bool operator()(const DataType& l, const DataType& r)
	      {
		  return l <= r;
	      }
      };
      
      /*
       *@brief quick select most n large/small, and store them in origin array from idx:0 to idx:n
       *@tparam Compare a function object type, 
       *possible like:
       *compFunc(const Data& l, const Data& r){ return l <= r; }
       *@tparam Swap exchange two elements' position
       *@param data input array
       *@param count how many elements you want to select
       *@warning Data assignment operator will be used to 
       *assign value to pivot variable
       */
	  template<typename DataType, typename Compare = DefaultCompare<DataType>>
      static void quickSelectMost(DataType* data, const std::uint32_t leftIdx, const std::uint32_t rightIdx, const std::uint32_t count)
      {
	  if(leftIdx >= rightIdx)
	      return;
	  if(count == 0)
	      return;
	  Compare cmpr;
	  std::uint32_t pivotIdx = leftIdx;
	  DataType pivot = data[pivotIdx];
	  
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
	  
	  if( curCount < count)
	      quickSelectMost<DataType, Compare>(data, i + 1, rightIdx, count - curCount);
	  else
	  {
	      quickSelectMost<DataType, Compare>(data, leftIdx, i, count);
	  }
      }

      template<typename DataType, typename Compare>
      void midian();
      
  }
}
