#ifndef FLEX_LIB_PIMPL_H
#define FLEX_LIB_PIMPL_H

#include <memory>
#include <utility>

namespace flex_lib
{
template<typename ImplType>
class pimpl
{
protected:
	typedef ImplType impl_t;
	std::unique_ptr<impl_t> m_impl;
	
	
protected:
	template<typename ... Args>
	pimpl(Args&& ... args) 
		: m_impl(new impl_t(std::forward<Args>(args)...))
	{
	}

	~pimpl() {;}	
};
}

#endif // FLEX_LIB_PIMPL_H
