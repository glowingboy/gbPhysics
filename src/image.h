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


	array_2d<std::uint8_t> signed_distance_field(const bit_vector& img,
						     const std::uint32_t width,
						     const std::uint32_t height,
						     const std::uint32_t sampleScale = 1);
	
	template<typename Side, typename Sprite>
	struct binay_bin_packing_node_data
	{
	    Side range[2];
	    Sprite* sprite;
	};
	template<typename Side, typename Sprite>
	struct binary_bin_packing_node:
	    public binary_tree_node<binay_bin_packing_node_data<Side, Sprite>>
	{
	    binary_bin_packing_node(const Sprite* sprite, const Side (&range)[2])
	    {
		
	    }

	    void newbranch(const Sprite* sprite)
	    {
		
	    }
	    void insert(const Sprite* sprite)
	    {
		Side (&range)[2] = data.range;
		if(sprite.width <= range[0] && sprite.height <= range[1])//fit current node
		{
		    //then go to left?
		    if(l != nullptr)//go to left
			(binary_bin_packing_node*)l->insert(sprite);
		    else//at leaf now, go parent's right
		    {
			if(parent != nullptr)
			{
			    binary_bin_packing_node*& pr = parent->r;
			    if(pr != nullptr)
				pr->insert(sprite);
			    else//new shelf
				pr = 
			}
		    }
			
		}
	    }

	    binary_tree_node* p;//parent
	    
	    static binary_bin_packing_node build(const std::vector<Sprite>& sprites)
	    {
		binary_bin_packing_node root;

		
	    }
	};

	template<typename T>
	array_2d<T> packing(std::vector<array_2d<T> sprites)
	{
	    //pre. sort sprites by height?
	    
	    //1. build binary bin packing tree
	    binary_bin_packing_node<T, array_2d<T>>  node(sprites.data(), sprites.size());

	    
	};
    };
};
