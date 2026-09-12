/*
 * PROJECT:    Mobility
 * FILE:       stddef.h
 * PURPOSE:    Definition for the C Standard Library <stddef.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDDEF
#define MOBILITY_RUNTIME_STDDEF

#include <Mile.Mobility.Portable.Types.h>

#define __STDC_VERSION_STDDEF_H__ 202311L

typedef MO_INTN ptrdiff_t;
#if (defined(__cplusplus) && (__cplusplus >= 201103L)) || \
    (defined(_MSVC_LANG) && (_MSVC_LANG >= 201103L))
typedef decltype(MO_NULL) nullptr_t;
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L)
typedef typeof(MO_NULL) nullptr_t;
#else
typedef MO_POINTER nullptr_t;
#endif
typedef MO_FP64 max_align_t;
#ifndef __cplusplus
#ifndef _WCHAR_T_DEFINED
#define _WCHAR_T_DEFINED
typedef MO_WIDE_CHAR wchar_t;
#endif
#endif
typedef MO_UINTN size_t;

#ifndef NULL
#define NULL MO_NULL
#endif
#ifndef __cplusplus
#ifdef _MSC_VER
#define unreachable() __assume(0)
#elif defined(__GNUC__) || defined(__clang__)
#define unreachable() __builtin_unreachable()
#else
#define unreachable() ((void)0)
#endif
#endif
#define offsetof(P, D) MO_FIELD_OFFSET(P, D)

#endif // !MOBILITY_RUNTIME_STDDEF
