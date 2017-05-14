#ifndef FLEX_LIB_PIMPL_H
#define FLEX_LIB_PIMPL_H

#include "detector.h"

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
    
#if 0
    template<typename Impl, typename ... Args, typename Tag = std::enable_if_t<std::is_same<decltype(ImplType(impl, std::forward<Args>(args)...)), ImplType>::value, void*>>
    // static auto Create(Impl* impl, Args&& ... args) -> std::enable_if_t<std::is_same<decltype(ImplType(impl, std::forward<Args>(args)...)), ImplType>::value, ImplPtr>
    static auto Create(Impl* impl, Args&& ... args) -> decltype(std::make_unique<ImplType>(impl, std::forward<Args>(args)...))
    {
        return std::make_unique<ImplType>(impl, std::forward<Args>(args)...);
    }
    
    template<typename Impl, typename ... Args, typename Tag = std::enable_if_t<std::is_same<decltype(ImplType(std::forward<Args>(args)...)), ImplType>::value, void*>>
    // static auto Create(Impl*, Args&& ... args) -> std::enable_if_t<std::is_same<decltype(ImplType(std::forward<Args>(args)...)), ImplType>::value, ImplPtr>
    static auto Create(Impl*, Args&& ... args) -> decltype(std::make_unique<ImplType>(std::forward<Args>(args)...))
    {
        return std::make_unique<ImplType>(std::forward<Args>(args)...);
    }
#endif
};
}

#endif // FLEX_LIB_PIMPL_H
