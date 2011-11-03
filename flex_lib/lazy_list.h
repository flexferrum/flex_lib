#ifndef FLEX_LIB_LAZY_LIST_H__
#define FLEX_LIB_LAZY_LIST_H__

#include <algorithm>
#include <iostream>
#include <list>
#include <vector>
#include <functional>
#include <iterator>
#include <stdint.h>
#include <type_traits>


namespace flex_lib
{
	template<typename LL>
	class lazy_list_iterator;

	template<typename NT, template <typename> class A = std::allocator> class lazy_list
	{
	public:
		typedef std::function<NT ()> NodeEvaluator;

	private:
		typedef std::function<NodeEvaluator (bool& isEOS)> NodeIniter;
		typedef lazy_list<NT, A> this_type;

		template<typename U>
		struct lazy_list_node
		{
			typedef std::function<bool (this_type*, lazy_list_node&)> NodeConstructor;
			mutable bool is_evaluated;
			mutable bool is_constructed;
			mutable bool is_final_node;
			mutable U node_value;

			NodeConstructor node_ctor;
			NodeEvaluator node_evtor;

			U const& dereference() const
			{
				if (!is_evaluated)
				{
					node_value = node_evtor();
					is_evaluated = true;
				}

				return node_value;
			}

			void construct(this_type const& l) const
			{
				if (is_constructed)
					return;

				is_final_node = !node_ctor(const_cast<this_type*>(&l), const_cast<lazy_list_node<U>&>(*this));
				is_constructed = true;
			}

			lazy_list_node() : is_evaluated(false), is_constructed(false), is_final_node(false) {;}
			lazy_list_node(NodeConstructor const& ctor) : is_evaluated(false), is_constructed(false), is_final_node(false), node_ctor(ctor) {;}
		};


	public:
		typedef NT value_type;
		typedef const value_type const_value_type;
		typedef value_type& reference_type;
		typedef value_type const& const_reference_type;
		typedef NT* pointer_type;
		typedef NT const* const_pointer_type;
		typedef lazy_list_node<NT> node_type_t;
		typedef std::list<node_type_t, A<node_type_t>> inner_list_t;
		typedef lazy_list_iterator<this_type> const_iterator;
		typedef const_iterator iterator;

		lazy_list() {;}

	#if 0
		template<typename VT>
		lazy_list(std::initializer_list<VT>&& values)
		{
			auto vb = std::begin(values);
			auto ve = std::end(values);
			NodeIniter init_fn = [vb, ve](bool& isEOS) mutable -> NodeEvaluator
			{
				return (isEOS = vb == ve) ? NodeEvaluator() : ([&vb]()->NT{return *vb ++;});
			};

			InitList(init_fn);
		}
	#endif

		template<typename It>
		lazy_list(It from, It to)
		{
			InitRange(from, to);
		}

		lazy_list(NodeIniter&& initer)
		{
			InitList(initer);
		}

		const_iterator begin() const {return const_iterator(*this, inner_list.begin());}
		const_iterator end() const {return const_iterator(*this);}
		const_iterator constructed_end() const {return const_iterator(*this, -- inner_list.end());}

		const_iterator cbegin() const {return begin();}
		const_iterator cend() const {return end();}

		const_reference_type front() const {return *begin();}

		const_reference_type head() const {return front();}

		this_type tail() const;

		this_type& cons(NT const& val)
		{
			auto first_val = val;
			inner_list.push_front(node_type_t([first_val](this_type* list, node_type_t& node) -> bool
			{
				auto val = first_val;
				node.node_evtor = [val](){return val;};
				return true;
			}));

			return *this;
		}

		this_type cons(NT val) const
		{
			NodeIniter tail_initer = MakeRangeIniter(begin(), end());
			bool is_first_time = true;
			return this_type([val, tail_initer, is_first_time](bool& isEOS) mutable -> NodeEvaluator
			{
				if (is_first_time)
				{
					is_first_time = false;
					isEOS = false;
					NT first_val = val;
					return [first_val]() {return first_val;};
				}

				return tail_initer(isEOS);
			});
		}

		template<typename FN>
		this_type filter(FN predicate)
		{
			typedef NodeEvaluator node_evtor_t;

			const_iterator cur(*this);
			const_iterator end(*this);
			bool is_first_time = true;
			this_type source_list(*this);

			return this_type([cur, end, predicate, is_first_time, source_list](bool& isEOS) mutable -> node_evtor_t
			{
				if (is_first_time)
				{
					cur = source_list.begin();
					end = source_list.end();
					is_first_time = false;
				}
				else
					++ cur;

				while (!(isEOS = cur == end))
				{
					if (predicate(*cur))
						break;

					++ cur;
				}

				if (isEOS)
					return node_evtor_t();

				auto& item_it = cur;
				return [item_it]() 
				{
					return *item_it;
				};
			});
		}

		template<typename FN>
		auto map(FN fn) -> lazy_list<typename std::remove_reference<decltype(fn(value_type()))>::type, A>
		{
			typedef typename std::remove_reference<decltype(fn(value_type()))>::type mapped_val_t;
			typedef lazy_list<mapped_val_t, A> ret_type;
			typedef typename ret_type::value_type ret_value_type;
			typedef typename ret_type::NodeEvaluator node_evtor_t;
			bool is_first_time = true;

			NodeIniter range_initer; //  = MakeRangeIniter(begin(), end());
			this_type source_list(*this);
			return ret_type([range_initer, fn, source_list, is_first_time, this](bool& isEOS) mutable -> node_evtor_t
			{
				if (is_first_time)
				{
					range_initer = MakeRangeIniter(source_list.begin(), source_list.end());
					is_first_time = false;
				}
				auto src_fn = range_initer(isEOS);
				if (isEOS)
					return node_evtor_t();

				auto& map_fn = fn;
				return [&map_fn, src_fn]() 
				{
					return map_fn(src_fn());
				};
			});
		}


		template<typename F, typename A1, typename A2>
		struct result_of
		{
			static F f;
			typedef decltype(f(A1(), A2())) type;
		};

		template<typename LL, typename FN>
		auto zip_with(LL const& secondList, FN fn) -> lazy_list<typename result_of<FN, value_type, typename LL::value_type>::type, A>
		{
			typedef typename LL::value_type second_val_t;
			typedef lazy_list<decltype(fn(value_type(), second_val_t())), A> ret_type;
			typedef typename ret_type::value_type ret_value_type;
			typedef typename ret_type::NodeEvaluator node_evtor_t;

			const_iterator cur1(*this);
			const_iterator end1(*this);
			typename LL::const_iterator cur2(secondList);
			typename LL::const_iterator end2(secondList);
			bool is_first_time = true;

			return ret_type([cur1, cur2, end1, end2, fn, is_first_time, secondList, this](bool& isEOS) mutable -> node_evtor_t
			{
				if (is_first_time)
				{
					cur1 = this->begin();
					end1 = this->end();
					cur2 = secondList.begin();
					end2 = secondList.end();
					is_first_time = false;
				}
				else
				{
					++ cur1;
					++ cur2;
				}

				if (cur1 == end1 || cur2 == end2)
				{
					isEOS = true;
					return node_evtor_t();
				}

				auto& calc_fn = fn;
				auto& left = cur1;
				auto& right = cur2;

				return [&calc_fn, &left, &right]() {return calc_fn(*left, *right);};
			});
		}  


		template<typename LL, typename FN>
		auto test_zip_with(LL const& secondList, FN const& fn) -> std::vector<typename result_of<FN, value_type, typename LL::value_type>::type>// lazy_list<decltype(fn(value_type(), typename LL::value_type())), A>
		{
			return lazy_list<decltype(fn(value_type(), typename LL::value_type())), A>();
		}

		bool empty() const
		{
			return begin() == end();
		}

		size_t size() const
		{
			return std::distance(begin(), end());
		}

	private:
		mutable inner_list_t inner_list;
		friend class lazy_list_iterator<this_type>;
		NodeIniter m_NodeIniter;

		template<typename It>
		static NodeIniter MakeRangeIniter(It from, It to)
		{
			typedef NodeEvaluator node_evtor_t;
			typedef NT list_value_t;
			bool is_first_time = true;
			NodeIniter init_fn = [from, to, is_first_time](bool& isEOS) mutable -> node_evtor_t
			{
				if (is_first_time)
					is_first_time = false;
				else
					++ from;

				if ((isEOS = from == to))
					return node_evtor_t();

				auto cur = from;

				return [cur]() {return *cur;};
			};

			return init_fn;
		}

		template<typename It>
		void InitRange(It from, It to)
		{
			InitList(MakeRangeIniter(from, to));
		}

		bool NodeConstructor(this_type* list, node_type_t& node)
		{
			// std::cout << "Construct next node" << std::endl;

			bool is_final_node = false;
			node.node_evtor = m_NodeIniter(is_final_node); // [&initFn](bool& isEOS) mutable -> NT {return initFn(isEOS);};
			inner_list.push_back(node_type_t([](this_type* list, node_type_t& node) -> bool
			{
				return list->NodeConstructor(list, node);
			}));

			return !is_final_node;
		}

		void InitList(NodeIniter const& initFn)
		{
			m_NodeIniter = initFn;
			inner_list.push_back(node_type_t([](this_type* list, node_type_t& node) mutable -> bool
			{
				return list->NodeConstructor(list, node);
			}));
		}
	};

	template<typename LL>
	class lazy_list_iterator
	{
		typedef typename LL::inner_list_t::const_iterator	inner_iterator;
	public:
		typedef lazy_list_iterator<LL> 				this_type;
		typedef typename LL::node_type_t          	node_type_t;

		typedef typename inner_iterator::difference_type	difference_type;
		typedef std::forward_iterator_tag    				iterator_category;
		typedef typename LL::value_type             		value_type;
		typedef value_type const*                           pointer;
		typedef value_type const&                           reference;

		lazy_list_iterator(LL const& list) : iterating_list(&list), is_end(true) {;}
		lazy_list_iterator(LL const& list, inner_iterator it) : iterating_list(&list), current(it), is_end(false) {;}

		reference operator*() const
		{
			current->construct(*iterating_list);
			return current->dereference();
		}

		pointer operator->() const
		{
			current->construct(*iterating_list);
			return &current->dereference();
		}

		this_type& operator ++()
		{
			if (current->is_final_node)
				return *this;

			inner_iterator it = current ++;
			if (current == iterating_list->inner_list.end() || !current->is_constructed)
			{
				if (current != iterating_list->inner_list.end())
				{
					current->construct(*iterating_list);
				}
				else
				{
					it->construct(*iterating_list);
					current = ++ it;
				}
			}

			return *this;
		}

		this_type operator ++(int)
		{
			this_type tmp(*this);
			++(*this);
			return tmp;
		}

		bool operator == (this_type const& other) const
		{
			if (is_end)
			{
				if (other.is_end)
					return true;

				if (other.iterating_list != iterating_list)
					return false;

				if (!other.current->is_constructed)
					other.current->construct(*iterating_list);

				return other.current->is_final_node;
			}

			if (!current->is_constructed)
				current->construct(*iterating_list);

			if (other.is_end)
				return other == *this;

			return current == other.current;
		}

		bool operator != (this_type const& other) const
		{
			return !(*this == other);
		}

		this_type& operator = (this_type const& other)
		{
			if (this == &other)
				return *this;

			iterating_list = other.iterating_list;
			current = other.current;
			is_end = other.is_end;
			return *this;
		}

		void swap(this_type& other)
		{
			std::swap(iterating_list, other.iterating_list);
			current.swap(other.current);
			std::swap(is_end, other.is_end);
		}

	private:
		LL const* iterating_list;
		inner_iterator current;
		bool is_end;
	};

#	include "lazy_list.inl.h"

}

namespace fl = flex_lib;

#endif // FLEX_LIB_LAZY_LIST_H__
