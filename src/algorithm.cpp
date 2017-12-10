#include "algorithm.h"

using namespace gb::algorithm;

void bit_vector::reserve(const size_t capacity)
{
    if(_data != nullptr)
    {
	delete [] _data;
    }

    const size_t byteSize = capacity / 8 + (capacity % 8 == 0 ? 0 : 1);

    _data = new std::uint8_t[byteSize];
    memset(_data, 0, byteSize);

    _capacity = byteSize * 8;
    _curSize = 0;
}

void bit_vector::realloc(const size_t capacity)
{
    const size_t byteSize = capacity / 8 + (capacity % 8 == 0 ? 0 : 1);
    _capacity = byteSize * 8;
    std::uint8_t* newData = new std::uint8_t[byteSize];
    
    const size_t cpSize = _curSize <= capacity ? _curSize : capacity;
    _curSize = cpSize;
    
    if(cpSize != 0)
    {
	const size_t preFullByteSize = cpSize / 8 ;
	memcpy(newData, _data, preFullByteSize);

	//tail fragment bits
	std::uint8_t tailSize = cpSize % 8;

	std::uint8_t& tailByte_dst = *(newData + preFullByteSize);
	std::uint8_t& tailByte_src = *(_data + preFullByteSize);
	for(int i = 0; i < tailSize; i++)
	{
	    _set_byte_uint(tailByte_dst, i, _get_byte_uint(tailByte_src, i));
	}
    }

    if(_data != nullptr)
	delete [] _data;

    _data = newData;
}
void bit_vector::insert(const size_t beginIdx, const size_t size, const std::uint8_t bitVal)
{
    assert(bitVal <= 1);
    if(size == 0)
	return;
    
    if(beginIdx + size > _capacity)
	realloc( 2 * _capacity);

    size_t leftSize = size;
    size_t byteIdx = beginIdx / 8;
    
    //head fragment bits
    std::uint8_t head_bit_idx = beginIdx % 8;
    
    if(head_bit_idx != 0)
    {
	std::uint8_t head_bit_size = 8 - head_bit_idx;
	if(leftSize < head_bit_size)
	    head_bit_size = (std::uint8_t)leftSize;
	
	std::uint8_t& curByte = _data[byteIdx];
	for(int i = 0; i < head_bit_size; i ++)
	{
	    _set_byte_uint(curByte, head_bit_idx + i, bitVal);
	}

	leftSize = leftSize - head_bit_size;
	byteIdx += 1;
    }

    if(leftSize != 0)
    {
	const size_t fullByteSize = leftSize / 8;
	if(fullByteSize != 0)
	{
	    const std::uint8_t byteVal = (bitVal == 0 ? 0 : 0xff);
	    memset(_data + byteIdx, byteVal, fullByteSize);
	    byteIdx += fullByteSize;
	}

	//tail fragment bits
	const std::uint8_t tailBitSize = leftSize % 8;
	if(tailBitSize != 0)
	{
	    std::uint8_t& curByte = _data[byteIdx];
	    for(int i = 0; i < tailBitSize; i++)
	    {
		_set_byte_uint(curByte, i, bitVal);
	    }
	}
    }

    _curSize += size;
}

std::uint8_t bit_vector::operator[](const size_t index)const
{
    assert(index < _capacity);
    const size_t byteIdx = index / 8;
    return _get_byte_uint(_data[byteIdx], index % 8);
}
