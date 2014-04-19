#ifndef FLEX_LIB_COMPILER_H__
#define FLEX_LIB_COMPILER_H__

#ifdef __clang__
#include "compiler_clang.h"
#elif defined (__GNUC__)
#include "compiler_gcc.h"
#elif defined (_MSC_VER)
#include "compiler_msvc.h"
#else
#pragma (warning, "Unknown compiler")
#endif

#endif // FLEX_LIB_COMPILER_H__
