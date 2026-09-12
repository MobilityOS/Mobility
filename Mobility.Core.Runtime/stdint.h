/*
 * PROJECT:    Mobility
 * FILE:       stdint.h
 * PURPOSE:    Definition for the C Standard Library <stdint.h> header
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_STDINT
#define MOBILITY_RUNTIME_STDINT

#include <Mile.Mobility.Portable.Types.h>

#define __STDC_VERSION_STDINT_H__ 202311L

typedef MO_INT8 int8_t;
typedef MO_INT16 int16_t;
typedef MO_INT32 int32_t;
typedef MO_INT64 int64_t;

typedef int8_t int_fast8_t;
typedef int32_t int_fast16_t;
typedef int32_t int_fast32_t;
typedef int64_t int_fast64_t;

typedef int8_t int_least8_t;
typedef int16_t int_least16_t;
typedef int32_t int_least32_t;
typedef int64_t int_least64_t;

typedef int64_t intmax_t;
typedef MO_INTN intptr_t;

typedef MO_UINT8 uint8_t;
typedef MO_UINT16 uint16_t;
typedef MO_UINT32 uint32_t;
typedef MO_UINT64 uint64_t;

typedef uint8_t uint_fast8_t;
typedef uint32_t uint_fast16_t;
typedef uint32_t uint_fast32_t;
typedef uint64_t uint_fast64_t;

typedef uint8_t uint_least8_t;
typedef uint16_t uint_least16_t;
typedef uint32_t uint_least32_t;
typedef uint64_t uint_least64_t;

typedef uint64_t uintmax_t;
typedef MO_UINTN uintptr_t;

#define INT8_MIN MO_INT8_MIN
#define INT16_MIN MO_INT16_MIN
#define INT32_MIN MO_INT32_MIN
#define INT64_MIN MO_INT64_MIN
#define INT8_MAX MO_INT8_MAX
#define INT16_MAX MO_INT16_MAX
#define INT32_MAX MO_INT32_MAX
#define INT64_MAX MO_INT64_MAX
#define UINT8_MAX MO_UINT8_MAX
#define UINT16_MAX MO_UINT16_MAX
#define UINT32_MAX MO_UINT32_MAX
#define UINT64_MAX MO_UINT64_MAX
#define INT8_WIDTH MO_INT8_WIDTH
#define INT16_WIDTH MO_INT16_WIDTH
#define INT32_WIDTH MO_INT32_WIDTH
#define INT64_WIDTH MO_INT64_WIDTH
#define UINT8_WIDTH MO_UINT8_WIDTH
#define UINT16_WIDTH MO_UINT16_WIDTH
#define UINT32_WIDTH MO_UINT32_WIDTH
#define UINT64_WIDTH MO_UINT64_WIDTH

#define INT_FAST8_MIN INT8_MIN
#define INT_FAST16_MIN INT32_MIN
#define INT_FAST32_MIN INT32_MIN
#define INT_FAST64_MIN INT64_MIN
#define INT_FAST8_MAX INT8_MAX
#define INT_FAST16_MAX INT32_MAX
#define INT_FAST32_MAX INT32_MAX
#define INT_FAST64_MAX INT64_MAX
#define UINT_FAST8_MAX UINT8_MAX
#define UINT_FAST16_MAX UINT32_MAX
#define UINT_FAST32_MAX UINT32_MAX
#define UINT_FAST64_MAX UINT64_MAX
#define INT_FAST8_WIDTH MO_INT8_WIDTH
#define INT_FAST16_WIDTH MO_INT32_WIDTH
#define INT_FAST32_WIDTH MO_INT32_WIDTH
#define INT_FAST64_WIDTH MO_INT64_WIDTH
#define UINT_FAST8_WIDTH MO_UINT8_WIDTH
#define UINT_FAST16_WIDTH MO_UINT32_WIDTH
#define UINT_FAST32_WIDTH MO_UINT32_WIDTH
#define UINT_FAST64_WIDTH MO_UINT64_WIDTH

#define INT_LEAST8_MIN INT8_MIN
#define INT_LEAST16_MIN INT16_MIN
#define INT_LEAST32_MIN INT32_MIN
#define INT_LEAST64_MIN INT64_MIN
#define INT_LEAST8_MAX INT8_MAX
#define INT_LEAST16_MAX INT16_MAX
#define INT_LEAST32_MAX INT32_MAX
#define INT_LEAST64_MAX INT64_MAX
#define UINT_LEAST8_MAX UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX
#define INT_LEAST8_WIDTH MO_INT8_WIDTH
#define INT_LEAST16_WIDTH MO_INT16_WIDTH
#define INT_LEAST32_WIDTH MO_INT32_WIDTH
#define INT_LEAST64_WIDTH MO_INT64_WIDTH
#define UINT_LEAST8_WIDTH MO_UINT8_WIDTH
#define UINT_LEAST16_WIDTH MO_UINT16_WIDTH
#define UINT_LEAST32_WIDTH MO_UINT32_WIDTH
#define UINT_LEAST64_WIDTH MO_UINT64_WIDTH

#define INTMAX_MIN INT64_MIN
#define INTMAX_MAX INT64_MAX
#define UINTMAX_MAX UINT64_MAX
#define INTMAX_WIDTH MO_INT64_WIDTH
#define UINTMAX_WIDTH MO_UINT64_WIDTH

#define INTPTR_MIN MO_INTN_MIN
#define INTPTR_MAX MO_INTN_MAX
#define UINTPTR_MAX MO_UINTN_MAX
#define INTPTR_WIDTH MO_INTN_WIDTH
#define UINTPTR_WIDTH MO_UINTN_WIDTH

#define PTRDIFF_MIN INTPTR_MIN
#define PTRDIFF_MAX INTPTR_MAX
#define PTRDIFF_WIDTH MO_INTN_WIDTH
#define SIZE_MAX UINTPTR_MAX
#define SIZE_WIDTH MO_UINTN_WIDTH

#define SIG_ATOMIC_MIN INT32_MIN
#define SIG_ATOMIC_MAX INT32_MAX
#define SIG_ATOMIC_WIDTH MO_INT32_WIDTH

#define WCHAR_MIN MO_WIDE_CHAR_MIN
#define WCHAR_MAX MO_WIDE_CHAR_MAX
#define WCHAR_WIDTH MO_WIDE_CHAR_WIDTH

#define WINT_MIN MO_WIDE_CHAR_MIN
#define WINT_MAX MO_WIDE_CHAR_MAX
#define WINT_WIDTH MO_WIDE_CHAR_WIDTH

#define INT8_C(Value) MO_INT8_C(Value)
#define INT16_C(Value) MO_INT16_C(Value)
#define INT32_C(Value) MO_INT32_C(Value)
#define INT64_C(Value) MO_INT64_C(Value)
#define UINT8_C(Value) MO_UINT8_C(Value)
#define UINT16_C(Value) MO_UINT16_C(Value)
#define UINT32_C(Value) MO_UINT32_C(Value)
#define UINT64_C(Value) MO_UINT64_C(Value)
#define INTMAX_C(Value) MO_INT64_C(Value)
#define UINTMAX_C(Value) MO_UINT64_C(Value)

#endif // !MOBILITY_RUNTIME_STDINT
