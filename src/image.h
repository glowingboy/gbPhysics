#include "sptree.h"
#include "math.h"

#include <vector>
using namespace gb::math;
using namespace gb::physics;

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

	template<typename Side>
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
	    typedef typename binay_bin_packing_node_data<Side>::axis axis;
	    
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
	    binary_bin_packing_node* _new_branch(const Sprite& sprite, binary_bin_packing_node* parent_)
	    {
		binary_bin_packing_node* y = new binary_bin_packing_node(axis::y, sprite.height, parent_);
		y->l = new binary_bin_packing_node(axis::x, sprite.width, y);
		y->l->l = new binary_bin_packing_node(((binary_bin_packing_node*)y->l));
		return y;
	    }
	    
	    void _walking_up(const Sprite& sprite, Side (&boundary)[2], Side (&location)[2])
	    {
		/*walk up to different branch. because of always traversing left child first, 
		  so parent's right child is always untraversed branch, unless i am at traversing
		  this branch now.
		  So, should always find a node is left child of it's parent.
		    */

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
				const axis split_axis = parentNode->data.split_axis;
				const Side split_value = parentNode->data.split_value;
				if(split_axis == axis::x)
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
			    assert(false);
		    }
		    //found, go to right
		    assert(parentNode->data.split_axis == axis::y);//X's left never be traversed
		    //find boundary's y bottom
		    binary_bin_packing_node* bottomYChildNode = parentNode;
		    binary_bin_packing_node* bottomYNode = bottomYChildNode->parent;
		    Side bottomY = std::numeric_limits<Side>::max();
		    for(;;)
		    {
			if(bottomYNode == nullptr)//hitted the root
			    break;
			else if(bottomYChildNode == bottomYNode->l)
			{
			    //X's left never be traversed
			    assert(bottomYNode->data.split_axis == axis::y);
			    bottomY = bottomYNode->data.split_value;
			    break;
			}
			
			bottomYChildNode = bottomYNode;
			bottomYNode = bottomYChildNode->parent;
		    }
		    binary_tree_node*& rightNode = parentNode->r;
		    const Side split_value = parentNode->data.split_value;
		    boundary[1] = bottomY - split_value;
		    location[1] += split_value;
		    if(sprite.width <= boundary[0] && sprite.height <= boundary[1])
		    {
		    if(rightNode != nullptr)
			((binary_bin_packing_node*)rightNode)->insert(sprite, boundary, location);
		    else
			rightNode = _new_branch(sprite, parentNode);
		    }
		    else//back from right
		    {
			location[1] -= split_value;
			parentNode->_walking_up(sprite, boundary, location);
		    }
			
	    }
	public:
	    void insert(const Sprite& sprite, Side (&boundary)[2], Side (&location)[2])
	    {
		if(data.split_axis == axis::x)
		{
		    //left always be occupied, so only right shuld be checked

		    //new boundary of right
		    boundary[0] = boundary[0] - data.split_value;
		    if(sprite.width <= boundary[0] && sprite.height <= boundary[1])
		    {
			location[0] += data.split_value;
			if(r != nullptr)
			    ((binary_bin_packing_node*)r)->insert(sprite, boundary, location);
			else
			    r = _new_branch(sprite, this);
		    }
		    else//walking up
		    {
			boundary[0] += data.split_value;
			_walking_up(sprite, boundary, location);
		    }
			
		}
		else if(data.split_axis == axis::y)
		{
		    //left child boundary
		    Side old_boundary_y = boundary[1];
		    boundary[1] = data.split_value;
		    if(sprite.width <= boundary[0] && sprite.height <= boundary[1])//go to left
		    {
			assert(l != nullptr);
			((binary_bin_packing_node*)l)->insert(sprite, boundary, location);
		    }
		    else//go to right
		    {
			boundary[1] = old_boundary_y;
			boundary[1] = boundary[1] - data.split_value;

			if(sprite.width <= boundary[0] && sprite.height <= boundary[1])
			{
			    location[1] += data.split_value;
			    if(r != nullptr)
				((binary_bin_packing_node*)r)->insert(sprite, boundary, location);
			    else
				r = _new_branch(sprite, this);
			}
			else
			{
			    boundary[1] += data.split_value;
			    _walking_up(sprite, boundary, location);
			}

		    }
		}

	    }

	    Side bin_height()const
	    {
		assert(data.split_axis == axis::y);
		Side rChildHeight = 0;
		if(r != nullptr)
		    rChildHeight = ((binary_bin_packing_node*)r)->bin_height();

		return rChildHeight + data.split_value;
	    }

	    binary_bin_packing_node* parent;//parent
	    binay_bin_packing_node_data<Side> data;
	};

	template<typename Sprite, typename Pixel>
	array_2d<Pixel> packing(std::vector<Sprite>& sprites, const std::uint32_t fixedWidth = 0)
	{
	    //pre. sort sprites by height?
	    auto height_compare = [](const Sprite& l, const Sprite& r)
		{
		    return r.height < l.height;
		};
	    
	    std::sort(sprites.begin(), sprites.end(), height_compare);

	    std::uint32_t width = fixedWidth;
	    std::uint32_t maxWidth = width;
	    std::uint64_t area(0);
	    if(fixedWidth == 0)
	    {
		std::for_each(sprites.begin(),
			      sprites.end(),
			      [&maxWidth, &area](Sprite& sprite)
				  {
				      const std::uint32_t width = sprite.width;
				      if(maxWidth < width)
					  maxWidth = width;
				      area += width * sprite.height;
				  });
			      width = std::sqrt(area);
			      if(width < maxWidth)
				  width = maxWidth;
	    }

	    if(width == 0)
		throw "width == 0";
	    
	    //1. build binary bin packing tree
	    binary_bin_packing_node<std::uint32_t, Sprite> root(
		binay_bin_packing_node_data<std::uint32_t>::axis::x,
		0,
		nullptr);//all children will be at right of root
	    
	    
	    std::vector<std::array<std::uint32_t, 2>> locations;
	    std::for_each(sprites.begin(),
			  sprites.end(),
			  [width, &root, &locations](Sprite& sprite)
			  {
			      std::uint32_t location[2]{0, 0};
			      std::uint32_t boundary[2] =
				  {width, std::numeric_limits<std::uint32_t>::max()};

			      root.insert(sprite, boundary, location);
			      locations.push_back({{location[0], location[1]}});
			  });

	    std::uint32_t height = ((binary_bin_packing_node<std::uint32_t, array_2d<Sprite>>*)(root.r))->bin_height();

	    array_2d<Pixel> bin(height, width);
	    
	    //2.fill up bin
	    for(int i = 0; i < sprites.size(); i++)
	    {
		Sprite& sprite = sprites[i];
		std::array<std::uint32_t, 2>& location = locations[i];
		bin.insert(location, sprite.data());
		const float left = location[0];
		const float top = location[1];
		sprite.uv_l = left / width;
		sprite.uv_t = top / height;
		sprite.uv_r = (left + sprite.width) / width;
		sprite.uv_b = (top + sprite.height) / height;
	    }

	    return bin;
	}
    };
};
