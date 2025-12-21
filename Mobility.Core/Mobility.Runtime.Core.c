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
    MO_UINTN SourceEnd = SourceStart + Length;

    if (SourceEnd < SourceStart)
    {
        // Overflow detected.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

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

MO_FORCEINLINE MO_INTN MoRuntimeMemoryCompareInternalUnaligned(
    _In_ MO_POINTER Left,
    _In_ MO_POINTER Right,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINT8 LeftBytes = (volatile PMO_UINT8)Left;
    volatile PMO_UINT8 RightBytes = (volatile PMO_UINT8)Right;
    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        MO_UINT8 CurrentLeft = LeftBytes[Index];
        MO_UINT8 CurrentRight = RightBytes[Index];
        if (CurrentLeft != CurrentRight)
        {
            return (MO_INTN)CurrentLeft - (MO_INTN)CurrentRight;
        }
    }
    return 0;
}

MO_FORCEINLINE MO_INTN MoRuntimeMemoryCompareInternalNativeAligned(
    _In_ MO_POINTER Left,
    _In_ MO_POINTER Right,
    _In_ MO_UINTN Length)
{
    volatile PMO_UINTN LeftNative = (volatile PMO_UINTN)Left;
    volatile PMO_UINTN RightNative = (volatile PMO_UINTN)Right;
    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeCount; ++Index)
    {
        MO_UINTN CurrentLeft = LeftNative[Index];
        MO_UINTN CurrentRight = RightNative[Index];
        if (CurrentLeft != CurrentRight)
        {
            return MoRuntimeMemoryCompareInternalUnaligned(
                (MO_POINTER)(&LeftNative[Index]),
                (MO_POINTER)(&RightNative[Index]),
                sizeof(MO_UINTN));
        }
    }
    return 0;
}

EXTERN_C MO_INTN MOAPI MoRuntimeMemoryCompare(
    _In_opt_ MO_POINTER Left,
    _In_opt_ MO_POINTER Right,
    _In_opt_ MO_UINTN Length)
{
    if (!Length)
    {
        // For zero length, consider equal.
        return 0;
    }

    if (!Left && !Right)
    {
        // Both are nullptr, consider equal.
        return 0;
    }
    else if (!Left)
    {
        // Left is nullptr, consider less than Right.
        return -1;
    }
    else if (!Right)
    {
        // Right is nullptr, consider greater than Left.
        return 1;
    }

    MO_INTN CurrentResult = 0;
    MO_UINTN CurrentLeft = (MO_UINTN)(Left);
    MO_UINTN CurrentRight = (MO_UINTN)(Right);

    MO_UINTN AlignedLeft = MoRuntimeGetAlignedSize(
        CurrentLeft,
        sizeof(MO_UINTN));
    MO_UINTN AlignedRight = MoRuntimeGetAlignedSize(
        CurrentRight,
        sizeof(MO_UINTN));

    // If the left or right address is not aligned, process the unaligned part
    // with generic implementation first.
    if (AlignedLeft != CurrentLeft ||
        AlignedRight != CurrentRight)
    {
        MO_UINTN UnalignedLength = 0u;
        if (AlignedLeft < AlignedRight)
        {
            UnalignedLength = AlignedLeft - CurrentLeft;
        }
        else
        {
            UnalignedLength = AlignedRight - CurrentRight;
        }
        if (UnalignedLength > Length)
        {
            UnalignedLength = Length;
        }
        CurrentResult = MoRuntimeMemoryCompareInternalUnaligned(
            (MO_POINTER)(CurrentLeft),
            (MO_POINTER)(CurrentRight),
            UnalignedLength);
        if (CurrentResult != 0)
        {
            return CurrentResult;
        }
        if (Length == UnalignedLength)
        {
            // If all bytes have been processed, return directly.
            return 0;
        }
        CurrentLeft = AlignedLeft;
        CurrentRight = AlignedRight;
        Length -= UnalignedLength;
    }

    // If the buffer is not large enough, use the generic implementation.
    if (Length < sizeof(MO_UINTN))
    {
        CurrentResult = MoRuntimeMemoryCompareInternalUnaligned(
            (MO_POINTER)(CurrentLeft),
            (MO_POINTER)(CurrentRight),
            Length);
        return CurrentResult;
    }

    MO_UINTN UnalignedLength = Length % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = Length - UnalignedLength;

    // Process the aligned part with native type implementation.
    CurrentResult = MoRuntimeMemoryCompareInternalNativeAligned(
        (MO_POINTER)(CurrentLeft),
        (MO_POINTER)(CurrentRight),
        AlignedLength);
    if (CurrentResult != 0)
    {
        return CurrentResult;
    }

    // Process the remaining unaligned part with generic implementation.
    CurrentResult = MoRuntimeMemoryCompareInternalUnaligned(
        (MO_POINTER)(CurrentLeft + AlignedLength),
        (MO_POINTER)(CurrentRight + AlignedLength),
        UnalignedLength);
    return CurrentResult;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeElementSort(
    _Inout_ MO_POINTER ElementArray,
    _In_ MO_UINTN ElementCount,
    _In_ MO_UINTN ElementSize,
    _In_ PMO_RUNTIME_SORT_COMPARE_HANDLER CompareHandler,
    _In_opt_ MO_POINTER Context)
{
    if (!ElementArray || !ElementCount || !ElementSize || !CompareHandler)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_UINTN ElementBase = (MO_UINTN)ElementArray;

    for (MO_UINTN Start = 0; Start < ElementCount - 1; ++Start)
    {
        MO_BOOL Swapped = MO_FALSE;

        for (MO_UINTN Index = ElementCount - 1; Index > Start; --Index)
        {
            MO_UINTN LeftOffset = (Index - 1u) * ElementSize;
            MO_UINTN RightOffset = Index * ElementSize;

            MO_POINTER Left = (MO_POINTER)(ElementBase + LeftOffset);
            MO_POINTER Right = (MO_POINTER)(ElementBase + RightOffset);

            if (CompareHandler(Left, Right, Context) <= 0)
            {
                // Skip if already in order.
                continue;
            }

            volatile PMO_UINT8 LeftBytes = (volatile PMO_UINT8)Left;
            volatile PMO_UINT8 RightBytes = (volatile PMO_UINT8)Right;
            for (MO_UINTN ByteIndex = 0u; ByteIndex < ElementSize; ++ByteIndex)
            {
                MO_UINT8 TemporaryByte = LeftBytes[ByteIndex];
                LeftBytes[ByteIndex] = RightBytes[ByteIndex];
                RightBytes[ByteIndex] = TemporaryByte;
            }

            Swapped = MO_TRUE;
        }

        if (!Swapped)
        {
            // Already sorted if no swaps in this pass.
            break;
        }
    }

    return MO_RESULT_SUCCESS_OK;
}
