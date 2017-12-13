#pragma once
#include <string>
#include <algorithm>
#include <cassert>
#include <deque>
#include <array>
#include "math.h"
namespace gb
{
	/*
	 *tree_node
	 *kd_node
	 *bit_vector
	 */
	namespace algorithm
	{
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
				if (l != nullptr)
				{
					delete l;
					l = nullptr;
				}

				if (r != nullptr)
				{
					delete r;
					r = nullptr;
				}
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
			kd_node(Data* data_, size_t size, size_t depth = 0)
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


		class bit_vector
		{
		public:
			inline bit_vector() :
				_data(nullptr),
				_curSize(0),
				_capacity(0)
			{}
			inline ~bit_vector()
			{
				if (_data != nullptr)
				{
					delete[] _data;
					_data = nullptr;
				}
			}
			void reserve(const size_t capacity);
			void realloc(const size_t capacity);
			void insert(const size_t beginIdx, const size_t size, const std::uint8_t bitVal);
			std::uint8_t operator[](const size_t index) const;
		private:
			std::uint8_t* _data;
			size_t _curSize;//bit size
			size_t _capacity;//bit capacity
		private:
			//save from right to left( the higher index the more left-shift)
			static inline void _set_byte_uint(std::uint8_t& byte, const std::uint8_t index, const std::uint8_t bitVal)
			{
				assert(index <= 7);
				assert(bitVal <= 1);

				byte |= bitVal << index;
			}

			static inline std::uint8_t _get_byte_uint(const std::uint8_t& byte, const std::uint8_t index)
			{
				assert(index <= 7);

				const std::uint8_t mask = 1 << index;
				return (byte & mask) >> index;
			}

		};

		/*
		 *@brief, a static 2d array type
		 */
		template<typename T>
		class array_2d
		{
		private:
			struct _proxy
			{
				_proxy(T* data, const std::uint32_t col) :
					_data(data),
					_col(col)
				{}
				T& operator[](const std::uint32_t colIdx)
				{
					assert(colIdx < _col);
					return _data[colIdx];
				}

				T* const _data;
				std::uint32_t _col;
			};

		public:
			~array_2d()
			{
				if (_data != nullptr)
				{
					delete[] _data;
					_data = nullptr;
				}
			}
			array_2d() :
				row(0),
				col(0),
				_data(nullptr)
			{}
			array_2d(const std::uint32_t row_, const std::uint32_t col_) :
				row(row_),
				col(col_),
				_data(new T[row_ * col_]{ 0 })
			{}

			template<typename ... Args>
			array_2d(const std::uint32_t row_, const std::uint32_t col_, Args ... args) :
				row(row_),
				col(col_),
				_data(new T[row_ * col_]{ args ... })
			{}

			array_2d(array_2d&& other) :
				row(other.row),
				col(other.col),
				_data(other._data)
			{
				other._data = nullptr;
			}

			void operator=(array_2d&& other)
			{
				row = other.row;
				col = other.col;
				_data = other._data;
				other._data = nullptr;
			}
			_proxy operator[](const std::uint32_t rowIdx)
			{
				assert(rowIdx < row);
				return _proxy(_data + rowIdx * col, col);
			}

			void realloc(const std::uint32_t row_, const std::uint32_t col_)
			{
				std::uint32_t old_row = row;
				std::uint32_t old_col = col;
				row = row_;
				col = col_;
				T* tmp = new T[row * col]{ 0 };
				if (_data != nullptr)
				{
					if (old_row > row)
						old_row = row;
					if (old_col > col)
						old_col = col;

					for (std::uint32_t i = 0; i < old_row; i++)
					{
						for (std::uint32_t j = 0; j < old_col; j++)
						{
							tmp[i][j] = _data[i][j];
						}
					}

					delete[] _data;
					_data = nullptr;
				}

				_data = tmp;
			}

			void assign(const std::uint32_t row_, const std::uint32_t col_, T* data)
			{
				row = row_;
				col = col_;
				if (_data != nullptr)
				{
					delete[] _data;
					_data = nullptr;
				}
				_data = data;
			}

			/*
			 *@param, location, location[0]: col idx, location[1]: row idx.
			 */
			void insert(const std::array<std::uint32_t, 2>& location, array_2d<T>& other)
			{
				const std::uint32_t o_x = location[0];
				const std::uint32_t o_y = location[1];
				const std::uint32_t height = other.height;
				const std::uint32_t width = other.width;

				for (std::uint32_t i = 0; i < height; i++)
				{
					for (std::uint32_t j = 0; j < width; j++)
					{
						this->operator[](o_y + i)[o_x + j] = other[i][j];
					}
				}
			}

			const T* data()const { return _data; }
			union
			{
				std::uint32_t row;
				std::uint32_t height;
				std::uint32_t y;
			};

			union
			{
				std::uint32_t col;
				std::uint32_t width;
				std::uint32_t x;
			};
		private:
			T* _data;
		};

		template<typename T>
		struct vec2
		{
			union
			{
				struct { T x, y; };
				struct { T r, g; };
				struct { T s, t; };
			};
		    vec2():x(0),y(0){}
		    vec2(T x_, T y_):
			x(x_),
			y(y_)
			{}
			std::uint8_t length()const
			{
				return 2;
			}
			T& operator[](std::uint8_t idx)
			{
				assert(idx < this->length());
				return ((&(this->x))[idx]);
			}

			const T& operator[](std::uint8_t idx)const
			{
				assert(idx < this->length());
				return ((&(this->x))[idx]);
			}
			void operator=(const vec2& other)
			{
				x = other.x;
				y = other.y;
			}
		};
	}
}
