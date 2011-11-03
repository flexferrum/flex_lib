#ifndef FLEX_LIB_STRINGIZED_ENUM_H__
#define FLEX_LIB_STRINGIZED_ENUM_H__

#include <boost/preprocessor/seq/seq.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/seq/enum.hpp>
#include <boost/preprocessor/punctuation/comma.hpp>
#include <boost/preprocessor/comparison/equal.hpp>
#include <boost/preprocessor/facilities/empty.hpp>
#include <boost/preprocessor/stringize.hpp>

#include <map>
#include <string>
#include <stdexcept>

namespace flex_lib
{
namespace detail
{
	template<typename S, typename E>
	E FindEnumEntryForString(std::map<S, E> const& map, typename std::map<S, E>::key_type str)
	{
		auto p = map.find(str);

		if (p == map.end())
			throw std::bad_cast();

		return p->second;
	}
}
}

#define FL_ENUM_EMPTY_NAME BOOST_PP_EMPTY()

#define FL_DECLARE_ENUM_ENTRY_IMPL(EntryName, EntryVal, _) EntryName EntryVal

#define FL_DECLARE_ENUM_ENTRY(_1, _2, Entry) \
	FL_DECLARE_ENUM_ENTRY_IMPL Entry, 

#define FL_DECLARE_ENUM(EnumName, S) \
	enum EnumName \
	{ \
		BOOST_PP_SEQ_FOR_EACH(FL_DECLARE_ENUM_ENTRY, _, S)\
	};

#define FL_MAKE_STRING_ENUM_NAME(Entry) Entry
#define FL_MAKE_WSTRING_ENUM_NAME(Entry) L ## Entry

#define FL_DECLARE_ENUM2STRING_ENTRY_IMPL(STRING_EXPANDER, EntryId, EntryName) \
	case EntryId: \
		result = STRING_EXPANDER(EntryName); \
		break; 

#define FL_DECLARE_STRING2ENUM_ENTRY_IMPL(STRING_EXPANDER, EntryId, EntryName) result[STRING_EXPANDER(EntryName)] = EntryId;

#define FL_DECLARE_ENUM2STRING_ENTRY(_, STRING_EXPANDER, Entry) FL_DECLARE_ENUM2STRING_ENTRY_IMPL(STRING_EXPANDER, BOOST_PP_TUPLE_ELEM(3, 0, Entry), BOOST_PP_TUPLE_ELEM(3, 2, Entry))
#define FL_DECLARE_STRING2ENUM_ENTRY(_, STRING_EXPANDER, Entry) FL_DECLARE_STRING2ENUM_ENTRY_IMPL(STRING_EXPANDER, BOOST_PP_TUPLE_ELEM(3, 0, Entry), BOOST_PP_TUPLE_ELEM(3, 2, Entry))
	

#define FL_DECLARE_ENUM_STRINGS(EnumName, MACRO_NAME, S) \
	inline EnumName ## _StringMap_CharType const* EnumName##ToString(EnumName e) \
	{ \
		EnumName ## _StringMap_CharType const* result = NULL; \
		switch (e) \
		{ \
		BOOST_PP_SEQ_FOR_EACH(FL_DECLARE_ENUM2STRING_ENTRY, MACRO_NAME, S) \
		} \
		\
		return result; \
	} \
\
	inline EnumName StringTo##EnumName(EnumName ## _StringMap_CharType const* str) \
	{ \
		static std::map<std::basic_string<EnumName ## _StringMap_CharType>, EnumName> strings_map = []() -> std::map<std::basic_string<EnumName ## _StringMap_CharType>, EnumName> { \
			std::map<std::basic_string<EnumName ## _StringMap_CharType>, EnumName> result; \
			BOOST_PP_SEQ_FOR_EACH(FL_DECLARE_STRING2ENUM_ENTRY, MACRO_NAME, S) \
			return result; \
		}(); \
		\
		return flex_lib::detail::FindEnumEntryForString(strings_map, str); \
	} \

#define FL_STRINGIZED_ENUM(EnumName, EnumItems) \
	typedef char EnumName ## _StringMap_CharType; \
	FL_DECLARE_ENUM(EnumName, EnumItems) \
	FL_DECLARE_ENUM_STRINGS(EnumName, FL_MAKE_STRING_ENUM_NAME, EnumItems) \

#define FL_WSTRINGIZED_ENUM(EnumName, EnumItems) \
	typedef wchar_t EnumName ## _StringMap_CharType; \
	FL_DECLARE_ENUM(EnumName, EnumItems) \
	FL_DECLARE_ENUM_STRINGS(EnumName, FL_MAKE_WSTRING_ENUM_NAME, EnumItems) \

#define FL_ENUM_ENTRY(EnumEntry) ((EnumEntry, BOOST_PP_EMPTY(), #EnumEntry))
#define FL_ENUM_NAMED_ENTRY(EnumEntry, EnumEntryName) ((EnumEntry, BOOST_PP_EMPTY(), EnumEntryName))
#define FL_ENUM_SPEC_ENTRY(EnumEntry, Id) ((EnumEntry, = ## Id, #EnumEntry))

#endif // FLEX_LIB_STRINGIZED_ENUM_H__
