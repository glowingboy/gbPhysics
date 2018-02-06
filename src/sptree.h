// spatial partitioning tree

#pragma once
#include "boundingbox.h"

#include "math.h"
#include <algorithm>
#include <queue>
#include <set>
#include <vector>
#include <functional>
GB_PHYSICS_NS_BEGIN

struct binary_tree_node
{
    binary_tree_node() :
	l(nullptr),
	r(nullptr)
	{}

    binary_tree_node* l;
    binary_tree_node* r;
    size_t size()
	{
	    size_t size = 1;
	    if (l != nullptr)
		size += l->size();
	    if (r != nullptr)
		size += r->size();

	    return size;
	}
    virtual ~binary_tree_node()
	{
	    delete l;
	    l = nullptr;

	    delete r;
	    r = nullptr;
	}
    template<typename Func>
    void traverse_in_order(Func& func)
	{
	    l->traverse_in_order(func);
	    func(this);
	    r->traverse_in_order(func);
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
		return l.key[d] < r.key[d];
	    }

	compare(const std::uint8_t d_) :
	    d(d_)
	    {}
	compare(const compare& other) :
	    d(other.d)
	    {}
	void operator=(const compare& other)
	    {
		d = other.d;
	    }

	std::uint8_t d;
    };

    template<typename ... Args>
    kd_key(Args ... arg) :
	key{ arg ... }
	{}

    key_t square_distance(const kd_key& other)const
	{
	    return gb::math::square_distance<key_t, k_>(this->key, other.key);
	}
    key_t key[k_];
    static std::uint8_t k;
};
template<typename Key, std::uint8_t k_>
std::uint8_t kd_key<Key, k_>::k = k_;

/*
 *@brief, Data must derived from kd_key
 */
template<typename Data>
struct kd_node : public binary_tree_node
{
    typedef typename Data::key_t key_t;
    kd_node(Data* data_, size_t size, size_t depth = 0) // offline algorithm
	{
	    assert((data_ != nullptr && size != 0));

	    d = depth % Data::k;

	    //selecte median
	    const size_t medianIdx = size / 2;
	    std::nth_element<Data*, typename Data::compare>(data_,
							    data_ + medianIdx,
							    data_ + size,
							    typename Data::compare(d));

	    this->data = data_[medianIdx];

	    depth++;

	    const size_t lSize = medianIdx;
	    if (lSize > 0)
		this->l = new kd_node<Data>(data_, lSize, depth);
	    else
		this->l = nullptr;

	    const size_t rSize = size - lSize - 1;
	    if (rSize > 0)
		this->r = new kd_node<Data>(data_ + lSize + 1,
					    rSize, depth);
	    else
		this->r = nullptr;
	}

    void touch_parents(const Data& child, std::deque<kd_node*>& parents)
	{
	    parents.push_back(this);
	    if ((typename Data::compare(d))(child, this->data))
	    {
		if (this->l != nullptr)
		    ((kd_node*)(this->l))->touch_parents(child, parents);
	    }
	    else
	    {
		if (this->r != nullptr)
		    ((kd_node*)(this->r))->touch_parents(child, parents);
	    }
	}

    void _unwind(const Data& srchpnt, key_t& bestSqDist, kd_node*& best, kd_node* node, bool bFromPath)
	{
	    const key_t curSqDist = srchpnt.square_distance(node->data);
	    if (curSqDist < bestSqDist)
	    {
		bestSqDist = curSqDist;
		best = node;
	    }


	    //check l, r
	    const std::uint8_t d = node->d;
	    const key_t spKey = srchpnt.key[d];
	    const key_t curNodeKey = node->data.key[d];

	    static auto _otherSideCheck = [&]()->bool
		{
		    const key_t sqDistCheck = (key_t)std::pow(spKey - curNodeKey, 2);
		    return sqDistCheck <= bestSqDist;
		};

	    if (spKey <= curNodeKey)//left side
	    {
		//same side
		if (!bFromPath && node->l != nullptr)
		    _unwind(srchpnt, bestSqDist, best, (kd_node*)node->l, false);

		//check the other side
		if (node->r != nullptr)
		{
		    if (_otherSideCheck())
			_unwind(srchpnt, bestSqDist, best, (kd_node*)node->r, false);
		}
	    }
	    else
	    {
		//same side
		if (!bFromPath && node->r != nullptr)
		    _unwind(srchpnt, bestSqDist, best, (kd_node*)node->r, false);

		if (node->l != nullptr)
		{
		    if (_otherSideCheck())
			_unwind(srchpnt, bestSqDist, best, (kd_node*)node->l, false);
		}
	    }

	};


    key_t nearest_neighbour_search(const Data& srchpnt, kd_node*& ret)
	{
	    ret = nullptr;

	    std::deque<kd_node*> path;
	    this->touch_parents(srchpnt, path);

	    // best = path.back();
	    // path.pop_back();
#ifdef _MSC_VER
#undef max
#endif
	    key_t bestSqDist = std::numeric_limits<key_t>::max();

	    while (!path.empty())
	    {
		_unwind(srchpnt, bestSqDist, ret, path.back(), true);
		path.pop_back();
	    }

	    return bestSqDist;
	}
    Data data;
    std::uint8_t d;
};

/*
 *@param _Intersect, bool _Intersect(_Ele, _bb).
 */
template <typename _Ele, typename _Intersect, typename _BB_Unit = Float>
class octree
{
public:
    octree(const aabb<_BB_Unit>& aabb):
	_bb(aabb),
	_children{nullptr}
	{
	    const vec3<_BB_Unit> (&diagonal)[2] = aabb.diagonal;
		    
	    const vec3<_BB_Unit> centre = (diagonal[0] + diagonal[1]) * 0.5f;
	    const vec3<_BB_Unit> octanLen = (diagonal[1] - diagonal[0]) * 0.5f;
	    const vec3<_BB_Unit> lowers[2] = {diagonal[0], centre};

	    std::uint8_t idx = 0;
	    for(std::uint8_t i = 0; i < 2; i++)
	    {
		for(std::uint8_t j = 0; j < 2; j++)
		{
		    for(std::uint8_t k = 0; k < 2; k++)
		    {
			vec3<_BB_Unit> (&octanDia)[2] = _octanBB[idx++].diagonal;
			vec3<_BB_Unit>& lower = octanDia[0];
			lower.x = lowers[i].x;
			lower.y = lowers[j].y;
			lower.z = lowers[k].z;
			octanDia[1] = lower + octanLen;
		    }
		}
	    }
	}
    ~octree()
	{
	    for(std::uint8_t i = 0; i < 8; i++)
	    {
		octree* & child = _children[i];
		delete child;
		child = nullptr;
	    }
	}
public:
    const aabb<_BB_Unit>& GetBB() const
	{
	    return _bb;
	}
    const std::set<_Ele>& GetElements() const
	{
	    return _eles;
	}
    void insert(_Ele & ele)
	{
	    if(_ist(ele, _bb))
	    {
		for(std::uint8_t i = 0; i < 8; i++)
		{
		    const aabb<_BB_Unit>& octanBB = _octanBB[i];
		    if(_ist(ele, octanBB))
		    {
			if(_children[i] != nullptr)
			    _children->insert(ele);
			else
			{
			    _children[i] = new octree(octanBB);
			    _children->insert(ele);
			}
		    }
		    else
			_eles.insert(ele);
		}
	    }
	    else
		_eles.insert(ele);
	}
    void insert(_Ele & ele, octree* & oct)
	{
	    if(_ist(ele, _bb))
	    {
		for(std::uint8_t i = 0; i < 8; i++)
		{
		    const aabb<_BB_Unit>& octanBB = _octanBB[i];
		    if(_ist(ele, octanBB))
		    {
			octree* & child = _children[i];
			if(child != nullptr)
			    child->insert(ele);
			else
			{
			    child = new octree(octanBB);
			    child->insert(ele);
			}
			oct = child;
		    }
		    else
			_eles.insert(ele);
		}
	    }
	    else
		_eles.insert(ele);
		    
	    oct = this;
	}
	    
    std::vector<_Ele> query_intersect(const aabb<_BB_Unit>& q) const
	{
	    std::vector<_Ele> ret;
	    if(q.intersect(_bb))
	    {
		ret.insert(ret.end(), _eles.begin(), _eles.end());

		for(std::uint8_t i = 0; i < 8; i++)
		{
		    octree*& child = _children[i];
		    if(child != nullptr)
		    {
			std::vector<_Ele> subRet = child->query_intersect(q);
			ret.insert(ret.end(), subRet.begin(), subRet.end());
		    }
		}
	    }
	    return ret;
	}
private:
    std::set<_Ele> _eles;
    aabb<_BB_Unit> _bb;
    aabb<_BB_Unit> _octanBB[8];
    octree* _children[8];

    static _Intersect _ist;
};
template <typename _Ele, typename _Intersect, typename _BB_Unit>
_Intersect octree<_Ele, _Intersect, _BB_Unit>::_ist;

GB_PHYSICS_NS_END
