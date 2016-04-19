#ifndef FLEX_LIB_COMPILER_CLANG_H__
#define FLEX_LIB_COMPILER_CLANG_H__

#if !defined(__cplusplus) || __cplusplus < 201103L
#error "-std=c++11 option should be defined for compilator command line"
#endif

#define FL_CLANG_FULL_VERSION(v, r, p) (((v) << 24) + ((r) << 16) + (p))
#define FL_CLANG_VERSION(v, r) FL_CLANG_FULL_VERSION(v, r, 0)

#define FL_COMPILER_CLANG FL_CLANG_FULL_VERSION(__clang_major__, __clang_minor__, __clang_patchlevel__)

#define FL_CXX11_SUPPORT_ALIGNAS __has_feature(cxx_alignas)
#define FL_CXX11_SUPPORT_AUTO __has_feature(cxx_auto_type)
#define FL_CXX11_SUPPORT_CONSTEXPR __has_feature(cxx_constexpr)
#define FL_CXX11_SUPPORT_DECLTYPE __has_feature(cxx_decltype)
#define FL_CXX11_SUPPORT_DEFAULTFUNCTIONS __has_feature(cxx_defaulted_functions)
#define FL_CXX11_SUPPORT_DELETEDFUNCTIONS __has_feature(cxx_deleted_functions)
#define FL_CXX11_SUPPORT_DELEGATINGCTORS __has_feature(cxx_delegating_constructors)
#define FL_CXX11_SUPPORT_EXPLICITCONVS __has_feature(cxx_explicit_conversions)
#define FL_CXX11_SUPPORT_INHERITINGCTORS __has_feature(cxx_inheriting_constructors)
#define FL_CXX11_SUPPORT_INITIALIZERLISTS __has_feature(cxx_generalized_initializers)
#define FL_CXX11_SUPPORT_LAMBDAS  __has_feature(cxx_lambdas)
#define FL_CXX14_SUPPORT_POLYLAMBDAS 0
#define FL_CXX14_SUPPORT_LAMBDAINITINCAPTURE 0
#define FL_CXX11_SUPPORT_LOCALTYPESASTPLARGS __has_feature(cxx_local_type_template_args)
#define FL_CXX11_SUPPORT_INLINENAMESPACE __has_feature(cxx_inline_namespaces)
#define FL_CXX11_SUPPORT_AUTOFUNCTIONS __has_feature(cxx_trailing_return)
#define FL_CXX11_SUPPORT_NULLPTR __has_feature(cxx_nullptr)
#define FL_CXX11_SUPPORT_UNICODESTRINGS __has_feature(cxx_unicode_literals)
#define FL_CXX11_SUPPORT_RAWSTRINGS __has_feature(cxx_raw_string_literals)
#define FL_CXX11_SUPPORT_UDLITERALS __has_feature(cxx_user_literals)
#define FL_CXX11_SUPPORT_RVALUEREFS __has_feature(cxx_rvalue_references)
#define FL_CXX11_SUPPORT_STATICASSERT __has_feature(cxx_static_assert)
#define FL_CXX11_SUPPORT_STRONGENUMS __has_feature(cxx_strong_enums)
#define FL_CXX11_SUPPORT_TEMPLATEALIASES __has_feature(cxx_alias_templates)
#define FL_CXX11_SUPPORT_THREADLOCALSTORAGE __has_feature(cxx_thread_local)
#define FL_CXX11_SUPPORT_UNRESTRICTEDUNIONS  __has_feature(cxx_unrestricted_unions)
#define FL_CXX11_SUPPORT_VARIADICTEMPLATES __has_feature(cxx_variadic_templates)
#define FL_CXX11_SUPPORT_RANGEBASEDFOR __has_feature(cxx_range_for)
#define FL_CXX11_SUPPORT_OVERRIDESUPPORT __has_feature(cxx_override_control)
#define FL_CXX11_SUPPORT_ATTRIBUTES __has_feature(cxx_attributes)
#define FL_CXX11_SUPPORT_REFQUALIFIERS __has_feature(cxx_reference_qualified_functions)
#define FL_CXX11_SUPPORT_NONSTATICMEMBERSINIT __has_feature(cxx_nonstatic_member_init)

#if FL_COMPILER_CLANG >= FL_CLANG_VERSION(3,0)
#define FL_CXX11_SUPPORT_ALIGNOF 1
#define FL_CXX11_SUPPORT_EXTENDEDFRIEND 1
#define FL_CXX11_SUPPORT_LONGLONG 1
#define FL_CXX11_SUPPORT_NEWCHARTYPES 1
#define FL_CXX11_SUPPORT_RANGLEBRACKETS 1
#define FL_CXX11_SUPPORT_TYPETRAITS 1
#define FL_CXX11_SUPPORT_MAGICSTATICS 1
#   if FL_COMPILER_CLANG >= FL_CLANG_VERSION(3,1)
#define FL_CXX11_SUPPORT_ATOMIC 1
#define FL_CXX11_SUPPORT_FORWARDENUMS 1
#define FL_CXX11_SUPPORT_EXTENDEDSIZEOF 1
#   endif // FL_COMPILER_CLANG >= FL_CLANG_VERSION(3,1)
#else 
#   error "Only clang 3.0 or better is supported"
#endif // FL_COMPILER_CLANG >= FL_CLANG_VERSION(3,0)



#endif // FLEX_LIB_COMPILER_CLANG_H__
