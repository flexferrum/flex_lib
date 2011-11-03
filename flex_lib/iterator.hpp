/*****************************************************************************************
	Created:	26.06.2010	15:24

	Purpose:	
******************************************************************************************
	History:

	20100626 Flex Ferrum:
******************************************************************************************/

#ifndef GameWorld_xboost_iterator_hpp__
#define GameWorld_xboost_iterator_hpp__

#include <iterator>

namespace xboost
{
	template<typename C>
	auto begin(C& cont) -> decltype(cont.begin()) {return cont.begin();}

	template<typename C>
	auto end(C& cont) -> decltype(cont.end()) {return cont.end();}

	template<typename C>
	auto begin(C const& cont) -> decltype(cont.begin()) {return cont.begin();}

	template<typename C>
	auto end(C const& cont) -> decltype(cont.end()) {return cont.end();}

	template<typename C, int N>
	C* begin(C (&cont)[N]) {return cont;}

	template<typename C, int N>
	C* end(C (&cont)[N]) {return cont + N;}


}

#endif // GameWorld_xboost_iterator_hpp__
