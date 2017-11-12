#pragma once
#include <string>
#include <algorithm>
#include <cassert>
#include <deque>
#include "gbMath.h"
namespace gb
{
    namespace algorithm
    {
	template<typename Data>
	struct tree_node
	{
	    tree_node* l;
	    tree_node* r;
	    Data data;
	    size_t size()
		{
		    size_t size = 1;
		    if(l != nullptr)
			size += l->size();
		    if(r != nullptr)
			size += r->size();

		    return size;
		}
	    virtual ~tree_node()
		{
		    if(l != nullptr)
		    {
			delete l;
			l = nullptr;
		    }

		    if( r!= nullptr)
		    {
			delete r;
			r = nullptr;
		    }
		}
	};

	template<typename Key, std::uint8_t k_>
	struct kd_key
	{
	    static_assert(k_ != 0, "k_ can't be 0");
	    typedef Key key_t;
	    struct compare
	    {
		bool operator()(const kd_key& l, const kd_key& r)
		    {
			return l.key[d] <= r.key[d];
		    }

		compare(const std::uint8_t d_):
		    d(d_)
		    {}
		compare(const compare& other):
		    d(other.d)
		    {}
		void operator=(const compare& other)
		    {
			d = other.d;
		    }
		
		std::uint8_t d;
	    };

	    key_t square_distance(const kd_key& other)
		{
		    return gb::math::square_distance<key_t, k_>(this->key, other.key);
		}
	    
	    key_t key[k_];
	    static std::uint8_t k;
	};
      	template<typename Key, std::uint8_t k_>
	std::uint8_t kd_key<Key, k_>::k = k_;

	template<typename Data>
	struct kd_node: public tree_node<Data>
	{
	    kd_node(Data* data_, size_t size, size_t depth = 0)
		{
		    assert((data_ != nullptr && size != 0));

		    d = depth % Data::k;

		    //selecte median
		    const size_t medianIdx = size / 2;
		    std::nth_element<Data*,  typename Data::compare>(data_,
							 data_ + medianIdx,
							     data_ + size, typename Data::compare(d));

		    this->data = data_[medianIdx];
		  
		    depth++;

		    const size_t lSize = medianIdx;
		    if(lSize > 0)
			this->l = new kd_node<Data>(data_, lSize, depth);
		    else
			this->l = nullptr;

		    const size_t rSize = size - lSize - 1;
		    if( rSize > 0)
			this->r = new kd_node<Data>(data_ + lSize + 1,
								rSize, depth);
		    else
			this->r = nullptr;
		}

	    void touch_parents(Data& child, std::deque<kd_node*>& parents)
		{
		    parents.push_back(this);
		    if((typename Data::compare(d))(child, *this))
		    {
			if(this->l != nullptr)
			    this->l->touch_parent(child, parents);
		    }
		    else
		    {
			if(this->r != nullptr)
			    this->r->touch_parent(child, parents);
		    }
		}

	    kd_node& nearest_neighbour_search(const Data& srchpnt)
		{
		    kd_node* best = nullptr;

		    std::deque<kd_node*> path;
		    this->touch_parents(srchpnt, path);

		    // best = path.back();
		    // path.pop_back();

		    Data::key_t bestSqDist = std::numeric_limits<Data::key_t>::max();

		    auto _unwind = [&](const kd_node& node)
			->Data::key_t 
		    {
			const Data::key_t curSqDist = srchpnt.square_distance(node.data);
			if(curSqDist < bestSqDist)
			    bestSqDist = curSqDist;

			//check l, r
			const std::uint8_t d = node.d;
			const Data::key_t spKey = srchpnt.key[d];
			const Data::key_t curNodeKey = node.key[d];

			static auto _otherSideCheck = [&]()->bool
			{
			    const Data::key_t sqDistCheck = std::pow(std::abs(spKey - curNodeKey), 2);
			    return sqDistCheck <= bestSqDist;
			}
			
			if(spKey <= curNodeKey)//left side
			{
			    if(node.l != nullptr)
				_unwind(node.l);

			    //check the other side
			    if(node.r != nullptr)
			    {
				if(_otherSideCheck())
				    _unwind(node.r);
			    }
			}
			else
			{
			    if(node.r != nullptr)
				_unwind(node.r);
			}

		    }
		    while(!path.empty())
		    {
			_unwind(path.back());
			path.pop_back();
		    }
		}
	    std::uint8_t d;
	};
    }
}
