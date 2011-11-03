/*****************************************************************************************
	Created:	26.06.2010	15:24

	Purpose:	
******************************************************************************************
	History:

	20100626 Flex Ferrum:
******************************************************************************************/

#ifndef GameWorld_xboost_foreach_hpp__
#define GameWorld_xboost_foreach_hpp__

#include "iterator.hpp"

#define BOOST_FOREACH(var, cont) \
	if (bool continue_loop_ = true) \
		for (auto p_iterator_ = xboost::begin(cont); p_iterator_ != xboost::end(cont); ++ p_iterator_, continue_loop_ = true) \
			for (var = *p_iterator_; continue_loop_; continue_loop_ = false)

#define BOOST_AUTO_FOREACH(v, cont) BOOST_FOREACH(auto v, cont)

#define FOREACH(v, cont) BOOST_FOREACH(v, cont)
#define AUTO_FOREACH(v, cont) BOOST_AUTO_FOREACH(v, cont)

#endif // GameWorld_xboost_foreach_hpp__
