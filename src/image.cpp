#include "image.h"

using namespace gb::image;

array_2d<std::uint8_t> gb::image::signed_distance_field(const bit_vector& img,
					     const std::uint32_t width,
					     const std::uint32_t height,
					     const std::uint32_t sampleScale)
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
	    
    for(std::uint32_t jImg = 0; jSdf < sdfH; jImg += sampleScale)
    {
	for(std::uint32_t iImg = 0; iSdf < sdfW; iImg += sampleScale)
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
