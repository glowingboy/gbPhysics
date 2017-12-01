#include "algorithm.h"
#include <array>
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

	/*
	  inserting sprite A with w width, h height into
	  a region with fixed width W and infinite height,
	  will split the space into 3 regions,
	  1 occupied, and 2 empty.
	  this can be viewed as using line to split a space twice,
	  firstly, using a horizontal line to split the space into two space,
	  one is above, the other one bellow.
	  secondly, using a vertical line to split the space, one is right,
	  the other one is left.
	  using a binary tree to denote this procedure will like bellow.
	 
       	  +-----------------W--------------------+
       	+-+-----------------+--------------------+
	| |	     	    |	             	 |
        h |A  occupied	    |empty           	 |
        | |                 |                    |
        +-+-----------------+--------------------+
	  +--------w--------+                    |
       	  |empty                             	 |
	  |			             	 |
	  |			             	 |
	  |                                      |
	  |                                      |
	  +--------------------------------------+

          		     a(axis:Y, value h)
	  		    /
			   /
			  b(axis:X, value w)
	  		 /
			/
		       A

	 TREE'S PROPERTIES
	 all children of node with Y direction split line and value h, 
	 will inherit node's boundary[X].
	 left child of node will have boundary[Y] = h,
	 and right will have boundary[Y] = node.boundary[Y] - h
	 and node with X direcion is the same as Y, except an addition property
	 which is left child always occupied.

	 a.l.boundary[X] = a.r.boundary[X] = a.boundary[X]
	 a.l.boundary[Y] = h
	 a.r.boundary[Y] = a.boundary[Y] -h

	 b.l.boundary[Y] = b.r.boundary[Y] = b.boundary[Y]
	 b.l.boundary[X] = w
	 b.r.boundary[X] = b.boundary[X] - w

	 INIT STATE
	 creating a node with boundary, boundary[X] = W(fixed width), boundary[Y] = infinite large,
	 spliting direcion can be either Y or X(using Y will always insert to the left child, 
	 using X will always insert to the right child).

	 */

	template<typename Side, typename Sprite>
	struct binay_bin_packing_node_data
	{
	    enum axis
	    {
		x = 0, y, NA
	    };

	    binay_bin_packing_node_data(axis split_axis_, Side split_value_):
		split_axis(split_axis_),
		split_value(split_value_)
		//sprite(nullptr)
		{}
	    binay_bin_packing_node_data():
		split_axis(axis::NA),
		split_value(Side(0))
		//sprite(sprite_)
		{}
	    
	    const axis split_axis;
	    const Side split_value;
	    
	    //Sprite* sprite;
	};

	template<typename Side, typename Sprite>
	struct binary_bin_packing_node:
	    public binary_tree_node
	{
	    typedef typename binay_bin_packing_node_data<Side, Sprite>::axis axis;
	    
	    binary_bin_packing_node(const axis split_axis,
				    const Side side,
				    binary_bin_packing_node* parent_):
		data(split_axis, side),
		parent(parent_)
	    {}

	    binary_bin_packing_node(binary_bin_packing_node* parent_):
		parent(parent_)
	    {}

	private:
	    binary_bin_packing_node* _new_branch(const Sprite& sprite)
	    {
		binary_bin_packing_node* y = new binary_bin_packing_node(axis::y, sprite.height, this);
		y->l = new binary_bin_packing_node(axis::x, sprite.width, y);
		y->l->l = new binary_bin_packing_node(((binary_bin_packing_node*)y->l));
		return y;
	    }
	    
	    void _try_endup(const Sprite& sprite, Side (&boundary)[2], std::uint32_t (&location)[2])
	    {
		if(sprite.width <= boundary[0] && sprite.height <= boundary[1])//ended here?
		    parent->r = _new_branch(sprite);
		else/*walk up to different branch. because of always traversing left child first, 
		      so parent's right child is always untraversed branch, unless i am at traversing
		      this branch now.
		      So, should always find a node is left child of it's parent.
		    */
		{
		    //finding a node at left child of it's parent
		    binary_bin_packing_node* childNode = this;
		    binary_bin_packing_node* parentNode = parent;
		    for(;;)
		    {
			if(parentNode != nullptr)
			{
			    if(childNode == parentNode->l)
				break;
			    else//childNode is at right of parentNode
			    {
				const axis split_axis = childNode->data.split_axis;
				const Side split_value = childNode->data.split_value;
				boundary[split_axis] += split_value;
				location[split_axis] -= split_value;
				childNode = parentNode;
				parentNode = childNode->parent;
			    }
			}
			else/*never needed to walk up to root,
			      if root is Y direction, then root's left child is 
			      infinite large(with fixed width) region always has
			      enough space to hold a sprite.
			      if root is X direcion, then all nodes are in the right
			      of the root
			    */
			    assert(true);
		    }
		    //found, do insert again
		    childNode->insert(sprite, boundary, location);
		    
		}

	    }
	public:
	    void insert(const Sprite& sprite, Side (&boundary)[2], std::uint32_t (&location)[2])
	    {
		if(data.split_axis == axis::x)
		{
		    //left always be occupied, so only right shuld be checked

		    //new boundary of right
		    boundary[0] = boundary[0] - data.split_value;
		    location[0] += data.split_value;
		    if(r != nullptr)
			((binary_bin_packing_node*)r)->insert(sprite, boundary, location);
		    else//should be ended here?
			_try_endup(sprite, boundary, location);
		}
		else if(data.split_axis == axis::y)
		{

		    if(sprite.width <= boundary[0] && sprite.height <= boundary[1])//go to left
		    {
			assert(l != nullptr);
			boundary[1] = data.split_value;
			((binary_bin_packing_node*)l)->insert(sprite, boundary, location);
		    }
		    else//go to right
		    {
			boundary[1] = boundary[1] - data.split_value;
			location[1] += data.split_value;
			if(r != nullptr)
			    ((binary_bin_packing_node*)r)->insert(sprite, boundary, location);
			else
			    _try_endup(sprite, boundary, location);
		    }
		}

	    }

	    Side bin_height(Side height = 0)const
	    {
		assert(data.split_axis == axis::y);
		if(r != nullptr)
		    height += ((binary_bin_packing_node*)r)->bin_height();
		else
		    return data.split_value;;
	    }

	    binary_bin_packing_node* parent;//parent
	    binay_bin_packing_node_data<Side, Sprite> data;
	};

	template<typename T>
	array_2d<T> packing(std::vector<array_2d<T>>& sprites, const T fixedWidth = 0)
	{
	    //pre. sort sprites by height?
	    auto height_compare = [](const array_2d<T>& l, const array_2d<T>& r)
		{
		    return l.height < r.height;
		};
	    
	    std::sort(sprites.begin(), sprites.end(), height_compare);

	    T width = fixedWidth;
	    T maxWidth = width;
	    T area(0);
	    if(fixedWidth == (T)0)
	    {
		std::for_each(sprites.begin(),
			      sprites.end(),
			      [&maxWidth, &area](array_2d<T>& sprite)
				  {
				      const T width = sprite.width;
				      if(maxWidth < width)
					  maxWidth = width;
				      area += width * sprite.height;
				  });
			      width = std::sqrt(area);
			      if(width < maxWidth)
				  width = maxWidth;
	    }
	    
	    //1. build binary bin packing tree
	    binary_bin_packing_node<T, array_2d<T>> root(nullptr);
	    T boundary[2] = {width, std::numeric_limits<T>::max()};
	    
	    std::vector<std::array<std::uint32_t, 2>> locations;
	    std::for_each(sprites.begin(),
			  sprites.end(),
			  [&root, &boundary, &locations](array_2d<T>& sprite)
			  {
			      std::uint32_t location[2]{T(0), T(0)};
			      root.insert(sprite, boundary, location);
			      locations.push_back({location[0], location[1]});
			  });

	    T height = root.bin_height();

	    array_2d<T> bin(width, height);
	    
	    //2.fill up bin
	    for(int i = 0; i < sprites.size(); i++)
	    {
		bin.insert(locations[i].data(), sprites[i]);
	    }

	    return bin;
	}
    };
};
