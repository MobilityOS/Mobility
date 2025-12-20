/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Runtime.Core.c
 * PURPOSE:    Implementation for Mobility Runtime Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Runtime.Core.h"

EXTERN_C MO_UINTN MOAPI MoRuntimeGetAlignedSize(
    _In_ MO_UINTN Size,
    _In_ MO_UINTN Alignment)
{
    return (Size + Alignment - 1) & ~(Alignment - 1);
}

MO_FORCEINLINE VOID MoRuntimeMemoryFillByteInternalUnaligned(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINT8 Bytes = (volatile PMO_UINT8)Buffer;
    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        Bytes[Index] = Value;
    }
}

MO_FORCEINLINE VOID MoRuntimeMemoryFillByteInternalNativeAligned(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length)
{
    MO_UINTN NativeValue = 0;
    for (MO_UINTN Index = 0u; Index < sizeof(MO_UINTN); ++Index)
    {
        NativeValue |= (((MO_UINTN)Value) << (Index * 8u));
    }
    volatile PMO_UINTN NativeTypeBuffer = (volatile PMO_UINTN)Buffer;
    MO_UINTN NativeTypeBufferCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeTypeBufferCount; ++Index)
    {
        NativeTypeBuffer[Index] = NativeValue;
    }
}

EXTERN_C MO_RESULT MOAPI MoRuntimeMemoryFillByte(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length)
{
    if (!Buffer)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!Length)
    {
        // For zero length, do nothing and return success.
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN CurrentStart = (MO_UINTN)(Buffer);
    MO_UINTN RemainingLength = Length;

    MO_UINTN AlignedStart = MoRuntimeGetAlignedSize(
        CurrentStart,
        sizeof(MO_UINTN));

    // If the buffer address is not aligned, process the unaligned part with
    // generic implementation first.
    if (AlignedStart != CurrentStart)
    {
        MO_UINTN UnalignedLength = AlignedStart - CurrentStart;
        if (UnalignedLength > RemainingLength)
        {
            UnalignedLength = RemainingLength;
        }
        MoRuntimeMemoryFillByteInternalUnaligned(
            (MO_POINTER)(CurrentStart),
            Value,
            UnalignedLength);
        if (RemainingLength == UnalignedLength)
        {
            // If all bytes have been processed, return directly.
            return MO_RESULT_SUCCESS_OK;
        }
        CurrentStart = AlignedStart;
        RemainingLength -= UnalignedLength;
    }

    // If the buffer is not large enough, use the generic implementation.
    if (RemainingLength < sizeof(MO_UINTN))
    {
        MoRuntimeMemoryFillByteInternalUnaligned(
            (MO_POINTER)(CurrentStart),
            Value,
            RemainingLength);
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeMemoryFillByteInternalNativeAligned(
        (MO_POINTER)(CurrentStart),
        Value,
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeMemoryFillByteInternalUnaligned(
        (MO_POINTER)(CurrentStart + AlignedLength),
        Value,
        UnalignedLength);

    return MO_RESULT_SUCCESS_OK;
}
