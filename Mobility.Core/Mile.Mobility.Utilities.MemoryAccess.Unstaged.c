/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.MemoryAccess.Unstaged.c
 * PURPOSE:    Implementation for Mobility Unstaged Memory Access Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Mobility.Utilities.MemoryAccess.Unstaged.h"

#ifdef _MSC_VER
#if defined(_M_ARM) || defined(_M_ARM_NT) || defined(_M_ARM64) || \
    defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)
#if (defined(_M_ARM) || defined(_M_ARM_NT)) && (_MSC_FULL_VER < 170040825)
MO_EXTERN_C MO_VOID __emit(MO_UINT32 _Opcode);
#pragma intrinsic(__emit)
#define __dmb(x) { __emit(0xF3BF); __emit(0x8F5F); }
#else
MO_EXTERN_C MO_VOID __dmb(MO_UINT32 _Type);
#pragma intrinsic(__dmb)
#endif
#elif defined(_M_IX86) || defined(_M_AMD64)
MO_EXTERN_C long _InterlockedOr(long volatile*, long);
#pragma intrinsic(_InterlockedOr)
#endif
#endif  /* _MSC_VER */

MO_EXTERN_C MO_VOID MOAPI MoMileMemoryBarrier(MO_VOID)
{
#ifdef _MSC_VER /* MSVC */
#if defined(_M_ARM) || defined(_M_ARM_NT) || defined(_M_ARM64) || \
    defined(_M_HYBRID_X86_ARM64) || defined(_M_ARM64EC)
    __dmb(0xF); /* dmb sy */
#elif defined(_M_IX86) || defined(_M_AMD64)
    /* __faststorefence on x64 is implemented as a dummy locked OR with an */
    /* immediate zero. Use _InterlockedOr directly to share the same */
    /* locked-RMW fence implementation between x86 and x64. */
    /* The barrier variable is accessed only to create a side effect. */
    volatile long Barrier = 0;
    (MO_VOID)_InterlockedOr(&Barrier, 0);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
#if defined(__arm__) || defined(__aarch64__) || defined(__arm64ec__)
    __asm__ __volatile__("dmb sy" : : : "memory");
#elif defined(__i386__) || defined(__x86_64__)
    __sync_synchronize();
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}
