/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.MemoryAccess.Unstaged.h
 * PURPOSE:    Definition for Mobility Unstaged Memory Access Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED
#define MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED

#include "Mile.Mobility.Portable.Types.h"

/*
 * - All functions in this header file are C functions.
 * - All functions are prefixed with "MoMile".
 * - Only depends on Mile.Mobility.Portable.Types.h.
 */

#ifdef _MSC_VER
MO_EXTERN_C MO_VOID _ReadWriteBarrier(MO_VOID);
#pragma intrinsic(_ReadWriteBarrier)
#endif /* _MSC_VER */

/**
 * @brief Limits the compiler optimizations that can reorder memory accesses
 *        across the point of the call.
 */
static MO_FORCEINLINE MO_VOID MoMileCompilerBarrier(MO_VOID)
{
#ifdef _MSC_VER /* MSVC */
#ifdef __cplusplus
    ::_ReadWriteBarrier();
#else
    _ReadWriteBarrier();
#endif
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    __asm__ __volatile__("" : : : "memory");
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

#endif // !MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED
