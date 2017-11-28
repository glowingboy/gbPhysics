#include "algorithm.h"
using gb::algorithm::bit_vector;
using gb::algorithm::kd_key;
using gb::algorithm::kd_node;
namespace gb
{
    namespace image
    {
	template<typename T>
	std::vector<T> binary_img_contour(const bit_vector& img, const std::uint32_t width, const std::uint32_t height)
	{
	    std::vector<T> contour;

	    for(std::uint32_t i = 0; i < height; i++)
	    {
		for(std::uint32_t j = 0; j < width; j++)
		{
		    if(img[i * width + j] != 0)
		    {
			if( i == 0 || i == (height - 1) || j == 0 || j == (width - 1))
			    contour.push_back(T(j, i));
			else if(img[i * width + j - 1] == 0//left
				|| img[(i - 1) * width + j - 1] == 0//left top
				|| img[(i - 1) * width + j] == 0//top
				|| img[(i - 1) * width + j + 1] == 0 //right top
				|| img[i * width + j + 1] == 0//right
				|| img[(i + 1) * width + j + 1] == 0//right bottom
				|| img[(i + 1) * width + j] == 0//bottom
				|| img[(i + 1) * width + j - 1] == 0//left bottom
			    )
			    contour.push_back(T(j, i));
		    }
		}
	    }
	}

	void signed_distance_field(const bit_vector& img, const std::uint32_t width, const std::uint32_t height, std::uint8_t* sdfBuffer, const std::uint32_t sampleScale = 0)
	{
	    assert(sdfBuffer != nullptr);

	    struct kd_node_data:public kd_key<std::uint32_t, 2>
	    {
		kd_node_data(const std::uint32_t x, const std::uint32_t y):
		    key{x,y}
		    {}
	    }
	}
    };
};
