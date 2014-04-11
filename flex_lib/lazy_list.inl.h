// This is a part of lazy_list.h file.

#if 0
template<typename NT, template <typename> class A>
lazy_list<NT, A> lazy_list<NT, A>::tail() const
{
	typedef NodeEvaluator node_evtor_t;
	NodeIniter initer;
	bool is_first_time = true;

	return this_type([is_first_time, this, initer](bool& isEOS) mutable -> node_evtor_t
	{
		if (is_first_time)
		{
			if (this->begin() == this->end())
			{
				isEOS = true;
				return node_evtor_t();
			}

			auto tb = ++ this->begin();
			auto te = this->end();
			initer = this->MakeRangeIniter(tb, te);
			is_first_time = false;
		}

		return initer(isEOS);
	});
}
#endif


