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

/* Check the assumptions on the fixed width integer types. */

MO_C_STATIC_ASSERT(1 == sizeof(MO_UINT8));
MO_C_STATIC_ASSERT(2 == sizeof(MO_UINT16));
MO_C_STATIC_ASSERT(4 == sizeof(MO_UINT32));
MO_C_STATIC_ASSERT(8 == sizeof(MO_UINT64));
MO_C_STATIC_ASSERT(MO_POINTER_SIZE == sizeof(MO_UINTN));

#ifdef _MSC_VER /* MSVC */
MO_EXTERN_C char _InterlockedCompareExchange8(
    char volatile* _Destination,
    char _Exchange,
    char _Comparand);
MO_EXTERN_C short _InterlockedCompareExchange16(
    short volatile* _Destination,
    short _Exchange,
    short _Comparand);
MO_EXTERN_C long __cdecl _InterlockedCompareExchange(
    long volatile* _Destination,
    long _Exchange,
    long _Comparand);
MO_EXTERN_C __int64 _InterlockedCompareExchange64(
    __int64 volatile* _Destination,
    __int64 _Exchange,
    __int64 _Comparand);
MO_EXTERN_C void* _InterlockedCompareExchangePointer(
    void* volatile* _Destination,
    void* _Exchange,
    void* _Comparand);
#pragma intrinsic(_InterlockedCompareExchange8)
#pragma intrinsic(_InterlockedCompareExchange16)
#pragma intrinsic(_InterlockedCompareExchange)
#pragma intrinsic(_InterlockedCompareExchange64)
#pragma intrinsic(_InterlockedCompareExchangePointer)
MO_EXTERN_C long __cdecl _InterlockedExchange(
    long volatile* _Target, long _Value);
MO_EXTERN_C void* _InterlockedExchangePointer(
    void* volatile* _Target,
    void* _Value);
#pragma intrinsic(_InterlockedExchange)
#pragma intrinsic(_InterlockedExchangePointer)
MO_EXTERN_C long __cdecl _InterlockedDecrement(
    long volatile* Addend);
MO_EXTERN_C long __cdecl _InterlockedIncrement(
    long volatile* Addend);
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedIncrement)
MO_EXTERN_C long _InterlockedAnd(
    long volatile* _Value,
    long _Mask);
MO_EXTERN_C long _InterlockedOr(
    long volatile* _Value,
    long _Mask);
MO_EXTERN_C long _InterlockedXor(
    long volatile* _Value,
    long _Mask);
#pragma intrinsic(_InterlockedAnd)
#pragma intrinsic(_InterlockedOr)
#pragma intrinsic(_InterlockedXor)
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
#if !(defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_1) && \
    defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_2) && \
    defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_4) && \
    defined(__GCC_HAVE_SYNC_COMPARE_AND_SWAP_8))
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
#endif

MO_EXTERN_C MO_UINT8 MOAPI MoMileInterlockedCompareExchange8(
    _Mo_InOut_ MO_UINT8 volatile* Destination,
    _Mo_In_ MO_UINT8 Exchange,
    _Mo_In_ MO_UINT8 Comparand)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT8)_InterlockedCompareExchange8(
        (char volatile*)Destination,
        (char)Exchange,
        (char)Comparand);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_val_compare_and_swap(
        Destination,
        Comparand,
        Exchange);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT16 MOAPI MoMileInterlockedCompareExchange16(
    _Mo_InOut_ MO_UINT16 volatile* Destination,
    _Mo_In_ MO_UINT16 Exchange,
    _Mo_In_ MO_UINT16 Comparand)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT16)_InterlockedCompareExchange16(
        (short volatile*)Destination,
        (short)Exchange,
        (short)Comparand);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_val_compare_and_swap(
        Destination,
        Comparand,
        Exchange);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedCompareExchange32(
    _Mo_InOut_ MO_UINT32 volatile* Destination,
    _Mo_In_ MO_UINT32 Exchange,
    _Mo_In_ MO_UINT32 Comparand)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedCompareExchange(
        (long volatile*)Destination,
        (long)Exchange,
        (long)Comparand);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_val_compare_and_swap(
        Destination,
        Comparand,
        Exchange);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT64 MOAPI MoMileInterlockedCompareExchange64(
    _Mo_InOut_ MO_UINT64 volatile* Destination,
    _Mo_In_ MO_UINT64 Exchange,
    _Mo_In_ MO_UINT64 Comparand)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT64)_InterlockedCompareExchange64(
        (__int64 volatile*)Destination,
        (__int64)Exchange,
        (__int64)Comparand);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_val_compare_and_swap(
        Destination,
        Comparand,
        Exchange);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_POINTER MOAPI MoMileInterlockedCompareExchangePointer(
    _Mo_InOut_ MO_POINTER volatile* Destination,
    _Mo_In_Opt_ MO_POINTER Exchange,
    _Mo_In_Opt_ MO_POINTER Comparand)
{
#ifdef _MSC_VER /* MSVC */
    return _InterlockedCompareExchangePointer(
        Destination,
        Exchange,
        Comparand);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_val_compare_and_swap(
        Destination,
        Comparand,
        Exchange);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedExchange32(
    _Mo_InOut_ MO_UINT32 volatile* Target,
    _Mo_In_ MO_UINT32 Value)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedExchange(
        (long volatile*)Target,
        (long)Value);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    MO_UINT32 OldValue = __sync_val_compare_and_swap(
        Target,
        (MO_UINT32)0,
        (MO_UINT32)0);
    for (;;)
    {
        MO_UINT32 ActualValue = __sync_val_compare_and_swap(
            Target,
            OldValue,
            Value);
        if (ActualValue == OldValue)
        {
            return OldValue;
        }
        OldValue = ActualValue;
    }
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_POINTER MOAPI MoMileInterlockedExchangePointer(
    _Mo_InOut_ MO_POINTER volatile* Target,
    _Mo_In_Opt_ MO_POINTER Value)
{
#ifdef _MSC_VER /* MSVC */
    return _InterlockedExchangePointer(
        Target,
        Value);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    MO_POINTER OldValue = __sync_val_compare_and_swap(
        Target,
        MO_NULL,
        MO_NULL);
    for (;;)
    {
        MO_POINTER ActualValue = __sync_val_compare_and_swap(
            Target,
            OldValue,
            Value);
        if (ActualValue == OldValue)
        {
            return OldValue;
        }
        OldValue = ActualValue;
    }
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedIncrement32(
    _Mo_InOut_ MO_UINT32 volatile* Addend)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedIncrement(
        (long volatile*)Addend);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_add_and_fetch(
        Addend,
        (MO_UINT32)1);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedDecrement32(
    _Mo_InOut_ MO_UINT32 volatile* Addend)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedDecrement(
        (long volatile*)Addend);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_sub_and_fetch(
        Addend,
        (MO_UINT32)1);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedAnd32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedAnd(
        (long volatile*)Value,
        (long)Mask);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_fetch_and_and(
        Value,
        Mask);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedOr32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedOr(
        (long volatile*)Value,
        (long)Mask);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_fetch_and_or(
        Value,
        Mask);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedXor32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask)
{
#ifdef _MSC_VER /* MSVC */
    return (MO_UINT32)_InterlockedXor(
        (long volatile*)Value,
        (long)Mask);
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    return __sync_fetch_and_xor(
        Value,
        Mask);
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

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
