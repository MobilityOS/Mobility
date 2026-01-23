/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Platform.x64.c
 * PURPOSE:    Implementation for Mobility Runtime x64 Specific Parts
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Platform.x64.h"

#ifdef _MSC_VER

#ifndef MOBILITY_PLATFORM_X64_PRIVATE
#define MOBILITY_PLATFORM_X64_PRIVATE

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

char _InterlockedCompareExchange8(
    char volatile* Destination,
    char Exchange,
    char Comparand);
short _InterlockedCompareExchange16(
    short volatile* Destination,
    short Exchange,
    short Comparand);
long __cdecl _InterlockedCompareExchange(
    long volatile* Destination,
    long Exchange,
    long Comparand);
__int64 _InterlockedCompareExchange64(
    __int64 volatile* Destination,
    __int64 Exchange,
    __int64 Comparand);
void* _InterlockedCompareExchangePointer(
    void* volatile* Destination,
    void* Exchange,
    void* Comparand);

short _InterlockedDecrement16(
    short volatile* Addend);
long __cdecl _InterlockedDecrement(
    long volatile* Addend);
__int64 _InterlockedDecrement64(
    __int64 volatile* _Addend);

short _InterlockedIncrement16(
    short volatile* Addend);
long __cdecl _InterlockedIncrement(
    long volatile* Addend);
__int64 _InterlockedIncrement64(
    __int64 volatile* Addend);

void _mm_pause();

void __halt();

void __cdecl __debugbreak();

void __cpuid(int[4], int);

unsigned __int64 __readmsr(unsigned long);
void __writemsr(unsigned long, unsigned __int64);

unsigned __int64 __readcr3();
void __writecr3(unsigned __int64);

void __lidt(void*);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !MOBILITY_PLATFORM_X64_PRIVATE

EXTERN_C MO_UINT8 MoPlatformInterlockedCompareExchange8(
    _Inout_ MO_UINT8 volatile* Destination,
    _In_ MO_UINT8 Exchange,
    _In_ MO_UINT8 Comparand)
{
    return _InterlockedCompareExchange8(
        (char volatile*)Destination,
        (char)Exchange,
        (char)Comparand);
}

EXTERN_C MO_UINT16 MoPlatformInterlockedCompareExchange16(
    _Inout_ MO_UINT16 volatile* Destination,
    _In_ MO_UINT16 Exchange,
    _In_ MO_UINT16 Comparand)
{
    return _InterlockedCompareExchange16(
        (short volatile*)Destination,
        (short)Exchange,
        (short)Comparand);
}

EXTERN_C MO_UINT32 MoPlatformInterlockedCompareExchange32(
    _Inout_ MO_UINT32 volatile* Destination,
    _In_ MO_UINT32 Exchange,
    _In_ MO_UINT32 Comparand)
{
    return _InterlockedCompareExchange(
        (long volatile*)Destination,
        (long)Exchange,
        (long)Comparand);
}

EXTERN_C MO_UINT64 MoPlatformInterlockedCompareExchange64(
    _Inout_ MO_UINT64 volatile* Destination,
    _In_ MO_UINT64 Exchange,
    _In_ MO_UINT64 Comparand)
{
    return _InterlockedCompareExchange64(
        (__int64 volatile*)Destination,
        (__int64)Exchange,
        (__int64)Comparand);
}

EXTERN_C MO_POINTER MoPlatformInterlockedCompareExchangePointer(
    _Inout_ MO_POINTER volatile* Destination,
    _In_ MO_POINTER Exchange,
    _In_ MO_POINTER Comparand)
{
    return _InterlockedCompareExchangePointer(
        (void* volatile*)Destination,
        (void*)Exchange,
        (void*)Comparand);
}

EXTERN_C MO_UINT16 MoPlatformInterlockedDecrement16(
    _In_ MO_UINT16 volatile* Addend)
{
    return _InterlockedDecrement16((short volatile*)Addend);
}

EXTERN_C MO_UINT32 MoPlatformInterlockedDecrement32(
    _In_ MO_UINT32 volatile* Addend)
{
    return _InterlockedDecrement((long volatile*)Addend);
}

EXTERN_C MO_UINT64 MoPlatformInterlockedDecrement64(
    _In_ MO_UINT64 volatile* Addend)
{
    return _InterlockedDecrement64((__int64 volatile*)Addend);
}

EXTERN_C MO_UINT16 MoPlatformInterlockedIncrement16(
    _In_ MO_UINT16 volatile* Addend)
{
    return _InterlockedIncrement16((short volatile*)Addend);
}

EXTERN_C MO_UINT32 MoPlatformInterlockedIncrement32(
    _In_ MO_UINT32 volatile* Addend)
{
    return _InterlockedIncrement((long volatile*)Addend);
}

EXTERN_C MO_UINT64 MoPlatformInterlockedIncrement64(
    _In_ MO_UINT64 volatile* Addend)
{
    return _InterlockedIncrement64((__int64 volatile*)Addend);
}

EXTERN_C VOID MOAPI MoPlatformPause()
{
    _mm_pause();
}

EXTERN_C VOID MOAPI MoPlatformHalt()
{
    __halt();
}

EXTERN_C VOID MOAPI MoPlatformDebugBreak()
{
    __debugbreak();
}

EXTERN_C VOID MOAPI MoPlatformReadCpuid(
    _Out_ PMO_PLATFORM_X64_CPUID_RESULT Result,
    _In_ MO_UINT32 Index)
{
    __cpuid((int*)Result, (int)Index);
}

EXTERN_C MO_UINT64 MOAPI MoPlatformReadMsr(
    _In_ MO_UINT32 Index)
{
    return __readmsr(Index);
}

EXTERN_C VOID MOAPI MoPlatformWriteMsr(
    _In_ MO_UINT32 Index,
    _In_ MO_UINT64 Value)
{
    __writemsr(Index, Value);
}

EXTERN_C MO_UINT64 MOAPI MoPlatformReadCr3()
{
    return __readcr3();
}

EXTERN_C VOID MOAPI MoPlatformWriteCr3(
    _In_ MO_UINT64 Value)
{
    __writecr3(Value);
}

EXTERN_C VOID MOAPI MoPlatformLoadInterruptDescriptorTable(
    _In_ PMO_PLATFORM_X64_PSEUDO_DESCRIPTOR Descriptor)
{
    __lidt(Descriptor);
}

#endif // _MSC_VER

EXTERN_C VOID MOAPI MoPlatformSetSegmentDescriptorBase(
    _Out_ PMO_PLATFORM_X64_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Base)
{
    if (Descriptor)
    {
        Descriptor->BaseLow = ((MO_UINT16)(Base));
        Descriptor->BaseMiddle = ((MO_UINT8)(Base >> 16)) & 0xFF;
        Descriptor->BaseHigh = ((MO_UINT8)(Base >> 24)) & 0xFF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetSegmentDescriptorLimit(
    _Out_ PMO_PLATFORM_X64_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetSystemSegmentDescriptorBase(
    _Out_ PMO_PLATFORM_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT64 Base)
{
    if (Descriptor)
    {
        Descriptor->BaseLow = ((MO_UINT16)(Base));
        Descriptor->BaseMiddleLow = ((MO_UINT8)(Base >> 16)) & 0xFF;
        Descriptor->BaseMiddleHigh = ((MO_UINT8)(Base >> 24)) & 0xFF;
        Descriptor->BaseHigh = ((MO_UINT32)(Base >> 32)) & 0xFFFFFFFF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetSystemSegmentDescriptorLimit(
    _Out_ PMO_PLATFORM_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetIdtGateDescriptorOffset(
    _Out_ PMO_PLATFORM_X64_IDT_GATE_DESCRIPTOR Descriptor,
    _In_ MO_UINT64 Offset)
{
    if (Descriptor)
    {
        Descriptor->OffsetLow = ((MO_UINT16)(Offset));
        Descriptor->OffsetMiddle = ((MO_UINT16)(Offset >> 16)) & 0xFFFF;
        Descriptor->OffsetHigh = ((MO_UINT32)(Offset >> 32)) & 0xFFFFFFFF;
    }
}
