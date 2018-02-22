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
template <typename _Ele,
	  typename _Contain,
	  typename _ArbitraryPointGetter,
	  typename _BB_Unit = Float>
class octree
{
public:
    octree(const aabb<_BB_Unit>& bb, octree* parent = nullptr, const std::uint8_t siblingIdx = 0):
	_bb(bb)
	,_childLenSide(_octanBB[0].lenSide)
	,_centre(_octanBB[7].diagonal[GB_PHYSICS_DIAGONAL_LOWER_IDX])
	,_children{nullptr}
	,_parent(parent)
	,_siblingIdx(siblingIdx)
	{
	    const vec3<_BB_Unit> (&diagonal)[2] = bb.diagonal;
		    
	    const vec3<_BB_Unit> centre = (diagonal[0] + diagonal[1]) / 2;
	    const vec3<_BB_Unit> octanLen = (diagonal[1] - diagonal[0]) / 2;
	    const vec3<_BB_Unit> lowers[2] = {diagonal[0], centre};

	    // store octanBB.lower in (x_l, ((y_l, (z_l, z_u)), (y_u, ...)))(x_u, ...) order
	    // 1st, split by x into (0, 3) (4, 7)
	    // 2nd, split by y into (0, 1) (2, 3) (4, 5) (6, 7)
	    // 3rd, split by z into ...
	    // so the centre of bb is octanBB[7].dia.lower
	    std::uint8_t idx = 0;
	    for(std::uint8_t i = 0; i < 2; i++)
	    {
		for(std::uint8_t j = 0; j < 2; j++)
		{
		    for(std::uint8_t k = 0; k < 2; k++)
		    {
			aabb<_BB_Unit> & octanBB = _octanBB[idx++];
			octanBB.lenSide = octanLen;
			
			vec3<_BB_Unit> (&octanDia)[2] = octanBB.diagonal;
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
    const aabb<_BB_Unit>& getBB() const
	{
	    return _bb;
	}
    const std::set<_Ele>& getCurElements() const
	{
	    return _eles;
	}
    std::size_t size() const
    {
	std::size_t s = _eles.size();
	for(std::uint8_t i = 0; i < 8; i ++)
	    {
		const octree* child = _children[i];
		if(child != nullptr)
		    s += child->size();
	    }
	
	return s;
    }
private:
    // children insert
    void _insert(_Ele & ele, const vec3<_BB_Unit>& ap)
    {
	if(_childLenSide > _minOctanLenSide)
	    {
		const std::uint8_t idx = _getPossiableOctanIdx(ap);
		
		const aabb<_BB_Unit>& octanBB = _octanBB[idx];
		
		// check if octan ctn ele
		if(_ctn(ele, octanBB))
		    {
			octree* & child = _children[idx];
			
			if(child != nullptr)
			    child->_insert(ele, ap);
			else
			    {
				child = new octree(octanBB, this, idx);
				child->_insert(ele, ap);
			    }
			return;
		    }
	    }
	    
	    _eles.insert(ele);
	}
    
public:
    void insert(_Ele & ele)
	{
	    //	    if(_ctn(ele, _bb))
	    _insert(ele, _apg(ele));
	    //	    else
	    //		_eles.insert(ele);
	}
    
private:
    void _insert(_Ele & ele, octree * & oct, const vec3<_BB_Unit> & ap)
	{
	    if(_childLenSide > _minOctanLenSide)
	    {
		const std::uint8_t idx = _getPossiableOctanIdx(ap);
		const aabb<_BB_Unit>& octanBB = _octanBB[idx];
		if(_ctn(ele, octanBB))
		    {
			octree* & child = _children[idx];
			if(child != nullptr)
			    child->_insert(ele, oct, ap);
			else
			    {
				child = new octree(octanBB, this, idx);
				child->_insert(ele, oct, ap);
			    }
			return;
		    }
	    }

	    oct = this;
	    _eles.insert(ele);
	}
    
public:
    void insert(_Ele & ele, octree* & oct)
	{
	    // if(_ctn(ele, _bb))
	    _insert(ele, oct, _apg(ele));
	    // else
	    // {
	    // 	oct = this;
	    // 	_eles.insert(ele);
	    // }
	}
    
private:
    void _shrink()
    {
	if(_eles.size() != 0)
	    return;
	for(std::uint8_t i = 0; i < 8; i++)
	    {
		if(_children[i] != nullptr)
		    return;
	    }

	if(_parent != nullptr)
	    {
		octree* & cur = _parent->_children[_siblingIdx];
	
		delete cur;
		cur = nullptr;

		_parent->_shrink();
		
	    }
    }

public:    
    void remove_here(_Ele & ele)
    {
	typename std::set<_Ele>::iterator i = _eles.find(ele);
	if( i != _eles.end())
	    {
		_eles.erase(i);
		_shrink();
	    }
    }

private:    
    void _remove(_Ele& ele, const vec3<_BB_Unit> & ap)
    {
	const std::uint8_t idx = _getPossiableOctanIdx(ap);
	const aabb<_BB_Unit>& octanBB = _octanBB[idx];
	if(_ctn(ele, octanBB))
	    {
		octree* & child = _children[idx];
		assert(child != nullptr);
		if(child != nullptr)
		    child->_remove(ele, ap);
	    }
	else
	    remove_here(ele);
    }

public:
    void remove(_Ele & ele)
    {
	_remove(ele, _apg(ele));
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
    std::uint8_t _getPossiableOctanIdx(const vec3<_BB_Unit>& ap) const
    {
	// check which octanBB is the ele.centre in
	// according to the way octanBB stored
	std::uint8_t idx = ap.x > _centre.x? 4 : 0;
	idx += ap.y > _centre.y? 2 : 0;
	idx += ap.z > _centre.z? 1 : 0;

	return idx;
    }
private:
    std::set<_Ele> _eles;
    aabb<_BB_Unit> _bb;
    // (xl, ((yl, z[l,u]), (yu, z[l, u])))(xu, ...)
    // for(int i = 0; i <2; i ++)
    // 	{
    // 	    for j
    // 		for k
    // 		    octanbb.l[idx++] = ls.vec3[i][j][k]
    // 	}
    aabb<_BB_Unit> _octanBB[8];
    const vec3<_BB_Unit> & _childLenSide;
    const vec3<_BB_Unit> & _centre;
    octree* _children[8];
    
    octree* _parent;
    const std::uint8_t _siblingIdx;
    
    static constexpr _Contain _ctn{};
    static constexpr _ArbitraryPointGetter _apg{};
    static constexpr vec3<_BB_Unit> _minOctanLenSide{std::numeric_limits<_BB_Unit>::min() * 2};
};
template <typename _Ele, typename _Contain, typename _ArbitraryPointGetter, typename _BB_Unit>
constexpr _Contain octree<_Ele, _Contain, _ArbitraryPointGetter, _BB_Unit>::_ctn;

template <typename _Ele, typename _Contain, typename _ArbitraryPointGetter, typename _BB_Unit>
constexpr _ArbitraryPointGetter octree<_Ele, _Contain, _ArbitraryPointGetter, _BB_Unit>::_apg;

template <typename _Ele, typename _Contain, typename _ArbitraryPointGetter, typename _BB_Unit>
constexpr vec3<_BB_Unit> octree<_Ele, _Contain, _ArbitraryPointGetter, _BB_Unit>::_minOctanLenSide;

GB_PHYSICS_NS_END
