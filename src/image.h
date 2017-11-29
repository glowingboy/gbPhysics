#include "algorithm.h"
using namespace gb::math;
using namespace gb::algorithm;

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
	    return contour;
	}


	array_2d<std::uint8_t> signed_distance_field(const bit_vector& img, const std::uint32_t width, const std::uint32_t height, const std::uint32_t sampleScale = 1)
	{
	    typedef kd_key<std::uint32_t, 2> contour_coord;

	    std::vector<contour_coord> contour = binary_img_contour<contour_coord>(img, width, height);

	    kd_node<contour_coord> kd_contour(contour.data(), contour.size());

	    array_2d<std::uint8_t> sdf(width / sampleScale, height / sampleScale);
	    
	    const std::uint32_t sdfW = sdf.width;
	    const std::uint32_t sdfH = sdf.height;
	    
	    std::int64_t sqMaxDist = std::pow(sdfW, 2) + std::pow(sdfH, 2);

	    interval_mapper<std::int64_t, std::uint8_t> mapper(-sqMaxDist, sqMaxDist, 0, 255);
	    
	    std::uint32_t iSdf = 0, jSdf = 0;
	    
	    for(int jImg = 0; jSdf < sdfH; jImg += sampleScale)
	    {
		for(int iImg = 0; iSdf < sdfW; iImg += sampleScale)
		{
		    //1. get nn dist
		    kd_node<contour_coord>* nn;
		    std::int64_t sqNDist = kd_contour.nearest_neighbour_search(contour_coord(iImg, jImg), nn);
		    //2. get sign
		    if(img[jImg * width + iImg] != 0)
			sqNDist = -sqNDist;

		    //3.map from [-sqMaxdist, sqMaxdist] to [0, 255]
		    sdf[jSdf][iSdf] = mapper.map(sqNDist);
		    
		    iSdf++;
		}
		jSdf++;
	    }
	    return sdf;
	}
    };
};
