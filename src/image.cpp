#include "image.h"

using namespace gb::image;

array_2d<std::uint8_t> gb::image::signed_distance_field(const bit_vector& img,
							const std::uint32_t width,
							const std::uint32_t height,
							const std::uint32_t sampleScale)
{
    typedef kd_key<std::int64_t, 2> contour_coord;

    std::vector<contour_coord> contour = binary_img_contour<contour_coord>(img, width, height);

    kd_node<contour_coord> kd_contour(contour.data(), contour.size());

    array_2d<std::uint8_t> sdf(height / sampleScale, width / sampleScale);
	    
    const std::uint32_t sdfW = sdf.width;
    const std::uint32_t sdfH = sdf.height;
	    
    const double maxDist = std::sqrt((std::int64_t)width * width + (std::int64_t)height *  height);

    interval_mapper<double, std::int32_t> mapper(-maxDist, maxDist, 0, 255);
	    
    std::uint32_t iSdf = 0, jSdf = 0;
	    
    for(std::uint32_t jImg = 0; jSdf < sdfH; jImg += sampleScale)
    {
	iSdf = 0;
	for(std::uint32_t iImg = 0; iSdf < sdfW; iImg += sampleScale)
	{
	    //1. get nn dist
	    kd_node<contour_coord>* nn;
	    double nDist = std::sqrt(kd_contour.nearest_neighbour_search(contour_coord(iImg, jImg), nn));
	    
	    //2. get sign
	    if(img[jImg * width + iImg] != 0)
		nDist = -nDist;

	    //3.map from [-sqMaxdist, sqMaxdist] to [0, 255]
	    sdf[jSdf][iSdf] = mapper.map(nDist);
		    
	    iSdf++;
	}
	jSdf++;
    }
    return sdf;
}
