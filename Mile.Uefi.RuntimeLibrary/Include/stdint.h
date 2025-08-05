/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       stdint.h
 * PURPOSE:    The C Standard Library <stdint.h> header.
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_UEFI_STDINT
#define MILE_UEFI_STDINT

#include <Uefi.h>

typedef INT8             int8_t;
typedef INT16            int16_t;
typedef INT32            int32_t;
typedef INT64            int64_t;
typedef UINT8            uint8_t;
typedef UINT16           uint16_t;
typedef UINT32           uint32_t;
typedef UINT64           uint64_t;

typedef INT8             int_least8_t;
typedef INT16            int_least16_t;
typedef INT32            int_least32_t;
typedef INT64            int_least64_t;
typedef UINT8            uint_least8_t;
typedef UINT16           uint_least16_t;
typedef UINT32           uint_least32_t;
typedef UINT64           uint_least64_t;

typedef INT8             int_fast8_t;
typedef INT32            int_fast16_t;
typedef INT32            int_fast32_t;
typedef INT64            int_fast64_t;
typedef UINT8            uint_fast8_t;
typedef UINT32           uint_fast16_t;
typedef UINT32           uint_fast32_t;
typedef UINT64           uint_fast64_t;

typedef INT64            intmax_t;
typedef INTN             intptr_t;
typedef UINT64           uintmax_t;
typedef UINTN            uintptr_t;

#define INT8_MIN         MIN_INT8
#define INT16_MIN        MIN_INT16
#define INT32_MIN        MIN_INT32
#define INT64_MIN        MIN_INT64
#define INT8_MAX         MAX_INT8
#define INT16_MAX        MAX_INT16
#define INT32_MAX        MAX_INT32
#define INT64_MAX        MAX_INT64
#define UINT8_MAX        MAX_UINT8
#define UINT16_MAX       MAX_UINT16
#define UINT32_MAX       MAX_UINT32
#define UINT64_MAX       MAX_UINT64

#define INT_LEAST8_MIN   INT8_MIN
#define INT_LEAST16_MIN  INT16_MIN
#define INT_LEAST32_MIN  INT32_MIN
#define INT_LEAST64_MIN  INT64_MIN
#define INT_LEAST8_MAX   INT8_MAX
#define INT_LEAST16_MAX  INT16_MAX
#define INT_LEAST32_MAX  INT32_MAX
#define INT_LEAST64_MAX  INT64_MAX
#define UINT_LEAST8_MAX  UINT8_MAX
#define UINT_LEAST16_MAX UINT16_MAX
#define UINT_LEAST32_MAX UINT32_MAX
#define UINT_LEAST64_MAX UINT64_MAX

#define INT_FAST8_MIN    INT8_MIN
#define INT_FAST16_MIN   INT32_MIN
#define INT_FAST32_MIN   INT32_MIN
#define INT_FAST64_MIN   INT64_MIN
#define INT_FAST8_MAX    INT8_MAX
#define INT_FAST16_MAX   INT32_MAX
#define INT_FAST32_MAX   INT32_MAX
#define INT_FAST64_MAX   INT64_MAX
#define UINT_FAST8_MAX   UINT8_MAX
#define UINT_FAST16_MAX  UINT32_MAX
#define UINT_FAST32_MAX  UINT32_MAX
#define UINT_FAST64_MAX  UINT64_MAX

#define INTPTR_MIN       MIN_INTN
#define INTPTR_MAX       MAX_INTN
#define UINTPTR_MAX      MAX_UINTN

#define INTMAX_MIN       INT64_MIN
#define INTMAX_MAX       INT64_MAX
#define UINTMAX_MAX      UINT64_MAX

#define PTRDIFF_MIN      INTPTR_MIN
#define PTRDIFF_MAX      INTPTR_MAX

#ifndef SIZE_MAX
#define SIZE_MAX         UINTPTR_MAX
#endif

#define SIG_ATOMIC_MIN   INT32_MIN
#define SIG_ATOMIC_MAX   INT32_MAX

#define WCHAR_MIN        0x0000
#define WCHAR_MAX        0xffff

#define WINT_MIN         0x0000
#define WINT_MAX         0xffff

#define INT8_C(x)        (x)
#define INT16_C(x)       (x)
#define INT32_C(x)       (x)
#define INT64_C(x)       (x ## LL)

#define UINT8_C(x)       (x)
#define UINT16_C(x)      (x)
#define UINT32_C(x)      (x ## U)
#define UINT64_C(x)      (x ## ULL)

#define INTMAX_C(x)      INT64_C(x)
#define UINTMAX_C(x)     UINT64_C(x)

#endif // !MILE_UEFI_STDINT
