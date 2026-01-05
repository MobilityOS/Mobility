/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Runtime.x64.c
 * PURPOSE:    Implementation for Mobility Runtime x64 Specific Parts
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Runtime.x64.h"

PMO_RUNTIME_X64_INTERRUPT_HANDLER MoPlatformInterruptHandlers[256];

EXTERN_C VOID MOAPI MoPlatformSetSegmentDescriptorBase(
    _Out_ PMO_RUNTIME_X64_SEGMENT_DESCRIPTOR Descriptor,
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
    _Out_ PMO_RUNTIME_X64_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetSystemSegmentDescriptorBase(
    _Out_ PMO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
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
    _Out_ PMO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

EXTERN_C VOID MOAPI MoPlatformSetIdtGateDescriptorOffset(
    _Out_ PMO_RUNTIME_X64_IDT_GATE_DESCRIPTOR Descriptor,
    _In_ MO_UINT64 Offset)
{
    if (Descriptor)
    {
        Descriptor->OffsetLow = ((MO_UINT16)(Offset));
        Descriptor->OffsetMiddle = ((MO_UINT16)(Offset >> 16)) & 0xFFFF;
        Descriptor->OffsetHigh = ((MO_UINT32)(Offset >> 32)) & 0xFFFFFFFF;
    }
}
