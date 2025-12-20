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
    volatile PMO_UINTN NativeBuffer = (volatile PMO_UINTN)Buffer;
    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeCount; ++Index)
    {
        NativeBuffer[Index] = NativeValue;
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

MO_FORCEINLINE VOID MoRuntimeMemoryCopyInternalUnaligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINT8 DestinationBytes = (volatile PMO_UINT8)Destination;
    volatile PMO_UINT8 SourceBytes = (volatile PMO_UINT8)Source;
    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        DestinationBytes[Index] = SourceBytes[Index];
    }
}

MO_FORCEINLINE VOID MoRuntimeMemoryCopyInternalNativeAligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINTN DestinationNative = (volatile PMO_UINTN)Destination;
    volatile PMO_UINTN SourceNative = (volatile PMO_UINTN)Source;
    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeCount; ++Index)
    {
        DestinationNative[Index] = SourceNative[Index];
    }
}

MO_FORCEINLINE VOID MoRuntimeMemoryCopyInternal(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    MO_UINTN CurrentDestination = (MO_UINTN)(Destination);
    MO_UINTN CurrentSource = (MO_UINTN)(Source);
    MO_UINTN RemainingLength = Length;

    MO_UINTN AlignedDestination = MoRuntimeGetAlignedSize(
        CurrentDestination,
        sizeof(MO_UINTN));
    MO_UINTN AlignedSource = MoRuntimeGetAlignedSize(
        CurrentSource,
        sizeof(MO_UINTN));

    // If the destination or source address is not aligned, process the
    // unaligned part with generic implementation first.
    if (AlignedDestination != CurrentDestination ||
        AlignedSource != CurrentSource)
    {
        MO_UINTN UnalignedLength = 0u;
        if (AlignedDestination < AlignedSource)
        {
            UnalignedLength = AlignedDestination - CurrentDestination;
        }
        else
        {
            UnalignedLength = AlignedSource - CurrentSource;
        }
        if (UnalignedLength > RemainingLength)
        {
            UnalignedLength = RemainingLength;
        }
        MoRuntimeMemoryCopyInternalUnaligned(
            (MO_POINTER)(CurrentDestination),
            (MO_POINTER)(CurrentSource),
            UnalignedLength);
        if (RemainingLength == UnalignedLength)
        {
            // If all bytes have been processed, return directly.
            return;
        }
        CurrentDestination += UnalignedLength;
        CurrentSource += UnalignedLength;
        RemainingLength -= UnalignedLength;
    }

    // If the buffer is not large enough, use the generic implementation.
    if (RemainingLength < sizeof(MO_UINTN))
    {
        MoRuntimeMemoryCopyInternalUnaligned(
            (MO_POINTER)(CurrentDestination),
            (MO_POINTER)(CurrentSource),
            RemainingLength);
        return;
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeMemoryCopyInternalNativeAligned(
        (MO_POINTER)(CurrentDestination),
        (MO_POINTER)(CurrentSource),
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeMemoryCopyInternalUnaligned(
        (MO_POINTER)(CurrentDestination + AlignedLength),
        (MO_POINTER)(CurrentSource + AlignedLength),
        UnalignedLength);
}

MO_FORCEINLINE VOID MoRuntimeMemoryBackwardCopyInternalUnaligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINT8 DestinationBytes = (volatile PMO_UINT8)Destination;
    volatile PMO_UINT8 SourceBytes = (volatile PMO_UINT8)Source;
    for (MO_UINTN Index = Length; Index > 0u; --Index)
    {
        DestinationBytes[Index - 1u] = SourceBytes[Index - 1u];
    }
}

MO_FORCEINLINE VOID MoRuntimeMemoryBackwardCopyInternalNativeAligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINTN DestinationNative = (volatile PMO_UINTN)Destination;
    volatile PMO_UINTN SourceNative = (volatile PMO_UINTN)Source;
    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = NativeCount; Index > 0u; --Index)
    {
        DestinationNative[Index - 1u] = SourceNative[Index - 1u];
    }
}

MO_FORCEINLINE VOID MoRuntimeMemoryBackwardCopyInternal(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    MO_UINTN RemainingLength = Length;
    MO_UINTN CurrentDestination = ((MO_UINTN)(Destination)) + RemainingLength;
    MO_UINTN CurrentSource = ((MO_UINTN)(Source)) + RemainingLength;

    // Should use floor alignment for backward copy.

    MO_UINTN AlignedDestination = CurrentDestination & ~(sizeof(MO_UINTN) - 1);
    MO_UINTN AlignedSource = CurrentSource & ~(sizeof(MO_UINTN) - 1);

    // If the destination or source address is not aligned, process the
    // unaligned part with generic implementation first.
    if (AlignedDestination != CurrentDestination ||
        AlignedSource != CurrentSource)
    {
        MO_UINTN UnalignedLength = 0u;
        if (AlignedDestination > AlignedSource)
        {
            UnalignedLength = CurrentDestination - AlignedDestination;
        }
        else
        {
            UnalignedLength = CurrentSource - AlignedSource;
        }
        if (UnalignedLength > RemainingLength)
        {
            UnalignedLength = RemainingLength;
        }
        MoRuntimeMemoryBackwardCopyInternalUnaligned(
            (MO_POINTER)(CurrentDestination - UnalignedLength),
            (MO_POINTER)(CurrentSource - UnalignedLength),
            UnalignedLength);
        if (RemainingLength == UnalignedLength)
        {
            // If all bytes have been processed, return directly.
            return;
        }
        CurrentDestination -= UnalignedLength;
        CurrentSource -= UnalignedLength;
        RemainingLength -= UnalignedLength;
    }

    // If the buffer is not large enough, use the generic implementation.
    if (RemainingLength < sizeof(MO_UINTN))
    {
        MoRuntimeMemoryBackwardCopyInternalUnaligned(
            (MO_POINTER)(CurrentDestination - RemainingLength),
            (MO_POINTER)(CurrentSource - RemainingLength),
            RemainingLength);
        return;
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeMemoryBackwardCopyInternalNativeAligned(
        (MO_POINTER)(CurrentDestination - AlignedLength),
        (MO_POINTER)(CurrentSource - AlignedLength),
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeMemoryBackwardCopyInternalUnaligned(
        Destination,
        Source,
        UnalignedLength);
}

EXTERN_C MO_RESULT MOAPI MoRuntimeMemoryMove(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    if (!Destination || !Source)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!Length)
    {
        // For zero length, do nothing and return success.
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN DestinationStart = (MO_UINTN)(Destination);
    MO_UINTN SourceStart = (MO_UINTN)(Source);

    if (DestinationStart < SourceStart ||
        DestinationStart >= (SourceStart + Length))
    {
        // No overlap or safe to copy forward.
        MoRuntimeMemoryCopyInternal(
            Destination,
            Source,
            Length);
    }
    else
    {
        // Overlap and need to copy backward.
        MoRuntimeMemoryBackwardCopyInternal(
            Destination,
            Source,
            Length);
    }

    return MO_RESULT_SUCCESS_OK;
}
