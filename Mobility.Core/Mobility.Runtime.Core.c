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

MO_FORCEINLINE VOID MoRuntimeInternalMemoryFillByteUnaligned(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINT8*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer. Using
    // 'volatile PMO_UINT8' would only make the pointer variable itself
    // volatile.

    volatile MO_UINT8* Bytes = (volatile MO_UINT8*)Buffer;

    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        Bytes[Index] = Value;
    }
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryFillByteNativeAligned(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length)
{
    MO_UINTN NativeValue = 0;
    for (MO_UINTN Index = 0u; Index < sizeof(MO_UINTN); ++Index)
    {
        NativeValue |= (((MO_UINTN)Value) << (Index * 8u));
    }

    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINTN*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer. Using
    // 'volatile MO_UINTN*' would only make the pointer variable itself
    // volatile.

    volatile MO_UINTN* NativeBuffer = (volatile MO_UINTN*)Buffer;

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
        MoRuntimeInternalMemoryFillByteUnaligned(
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
        MoRuntimeInternalMemoryFillByteUnaligned(
            (MO_POINTER)(CurrentStart),
            Value,
            RemainingLength);
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeInternalMemoryFillByteNativeAligned(
        (MO_POINTER)(CurrentStart),
        Value,
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeInternalMemoryFillByteUnaligned(
        (MO_POINTER)(CurrentStart + AlignedLength),
        Value,
        UnalignedLength);

    return MO_RESULT_SUCCESS_OK;
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryCopyUnaligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINT8*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer. Using
    // 'volatile PMO_UINT8' would only make the pointer variable itself
    // volatile.

    volatile MO_UINT8* DestinationBytes = (volatile MO_UINT8*)Destination;
    volatile MO_UINT8* SourceBytes = (volatile MO_UINT8*)Source;

    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        DestinationBytes[Index] = SourceBytes[Index];
    }
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryCopyNativeAligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINTN*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer. Using
    // 'volatile MO_UINTN*' would only make the pointer variable itself
    // volatile.

    volatile MO_UINTN* DestinationNative = (volatile MO_UINTN*)Destination;
    volatile MO_UINTN* SourceNative = (volatile MO_UINTN*)Source;

    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeCount; ++Index)
    {
        DestinationNative[Index] = SourceNative[Index];
    }
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryCopy(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    MO_UINTN CurrentDestination = (MO_UINTN)(Destination);
    MO_UINTN CurrentSource = (MO_UINTN)(Source);
    MO_UINTN RemainingLength = Length;

    // Make sure the destination address is aligned.
    {
        MO_UINTN AlignedDestination = MoRuntimeGetAlignedSize(
            CurrentDestination,
            sizeof(MO_UINTN));

        // If the destination address is not aligned, process the unaligned part
        // with generic implementation first.
        if (AlignedDestination != CurrentDestination)
        {
            MO_UINTN UnalignedLength = AlignedDestination - CurrentDestination;
            if (UnalignedLength > RemainingLength)
            {
                UnalignedLength = RemainingLength;
            }
            MoRuntimeInternalMemoryCopyUnaligned(
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
    }

    // If the buffer is not large enough, use the generic implementation.
    if (RemainingLength < sizeof(MO_UINTN))
    {
        MoRuntimeInternalMemoryCopyUnaligned(
            (MO_POINTER)(CurrentDestination),
            (MO_POINTER)(CurrentSource),
            RemainingLength);
        return;
    }

    // Make sure the source address is aligned.
    {
        MO_UINTN AlignedSource = MoRuntimeGetAlignedSize(
            CurrentSource,
            sizeof(MO_UINTN));

        // If the source address is not aligned, use the generic implementation.
        if (AlignedSource != CurrentSource)
        {
            MoRuntimeInternalMemoryCopyUnaligned(
                (MO_POINTER)(CurrentDestination),
                (MO_POINTER)(CurrentSource),
                RemainingLength);
            return;
        }
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeInternalMemoryCopyNativeAligned(
        (MO_POINTER)(CurrentDestination),
        (MO_POINTER)(CurrentSource),
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeInternalMemoryCopyUnaligned(
        (MO_POINTER)(CurrentDestination + AlignedLength),
        (MO_POINTER)(CurrentSource + AlignedLength),
        UnalignedLength);
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryBackwardCopyUnaligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINT8*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer.
    // Using 'volatile PMO_UINT8' would only make the pointer variable itself
    // volatile.

    volatile MO_UINT8* DestinationBytes = (volatile MO_UINT8*)Destination;
    volatile MO_UINT8* SourceBytes = (volatile MO_UINT8*)Source;

    for (MO_UINTN Index = Length; Index > 0u; --Index)
    {
        DestinationBytes[Index - 1u] = SourceBytes[Index - 1u];
    }
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryBackwardCopyNativeAligned(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINTN*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer.
    // Using 'volatile PMO_UINTN' would only make the pointer variable itself
    // volatile.

    volatile MO_UINTN* DestinationNative = (volatile MO_UINTN*)Destination;
    volatile MO_UINTN* SourceNative = (volatile MO_UINTN*)Source;

    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = NativeCount; Index > 0u; --Index)
    {
        DestinationNative[Index - 1u] = SourceNative[Index - 1u];
    }
}

MO_FORCEINLINE VOID MoRuntimeInternalMemoryBackwardCopy(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length)
{
    MO_UINTN RemainingLength = Length;
    MO_UINTN CurrentDestination = ((MO_UINTN)(Destination)) + RemainingLength;
    MO_UINTN CurrentSource = ((MO_UINTN)(Source)) + RemainingLength;

    // Make sure the destination address is aligned.
    {
        // Should use floor alignment for backward copy.
        MO_UINTN AlignedDestination =
            CurrentDestination & ~(sizeof(MO_UINTN) - 1);

        // If the destination address is not aligned, process the unaligned part
        // with generic implementation first.
        if (AlignedDestination != CurrentDestination)
        {
            MO_UINTN UnalignedLength = CurrentDestination - AlignedDestination;
            if (UnalignedLength > RemainingLength)
            {
                UnalignedLength = RemainingLength;
            }
            MoRuntimeInternalMemoryBackwardCopyUnaligned(
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
    }

    // If the buffer is not large enough, use the generic implementation.
    if (RemainingLength < sizeof(MO_UINTN))
    {
        MoRuntimeInternalMemoryBackwardCopyUnaligned(
            (MO_POINTER)(CurrentDestination - RemainingLength),
            (MO_POINTER)(CurrentSource - RemainingLength),
            RemainingLength);
        return;
    }

    // Make sure the source address is aligned.
    {
        // Should use floor alignment for backward copy.
        MO_UINTN AlignedSource = CurrentSource & ~(sizeof(MO_UINTN) - 1);

        // If the source address is not aligned, use the generic implementation.
        if (AlignedSource != CurrentSource)
        {
            MoRuntimeInternalMemoryBackwardCopyUnaligned(
                (MO_POINTER)(CurrentDestination - RemainingLength),
                (MO_POINTER)(CurrentSource - RemainingLength),
                RemainingLength);
            return;
        }
    }

    MO_UINTN UnalignedLength = RemainingLength % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = RemainingLength - UnalignedLength;

    // Process the aligned part with native type implementation.
    MoRuntimeInternalMemoryBackwardCopyNativeAligned(
        (MO_POINTER)(CurrentDestination - AlignedLength),
        (MO_POINTER)(CurrentSource - AlignedLength),
        AlignedLength);

    // Process the remaining unaligned part with generic implementation.
    MoRuntimeInternalMemoryBackwardCopyUnaligned(
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
    if (Destination == Source)
    {
        // Both are the same address, do nothing and return success.
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN DestinationStart = (MO_UINTN)(Destination);
    MO_UINTN SourceStart = (MO_UINTN)(Source);

    if (Length > (MO_UINTN_MAX - DestinationStart) ||
        Length > (MO_UINTN_MAX - SourceStart))
    {
        // Overflow detected.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    if (DestinationStart < SourceStart ||
        DestinationStart >= (SourceStart + Length))
    {
        // No overlap or safe to copy forward.
        MoRuntimeInternalMemoryCopy(
            Destination,
            Source,
            Length);
    }
    else
    {
        // Overlap and need to copy backward.
        MoRuntimeInternalMemoryBackwardCopy(
            Destination,
            Source,
            Length);
    }

    return MO_RESULT_SUCCESS_OK;
}

MO_FORCEINLINE MO_INTN MoRuntimeInternalMemoryCompareUnaligned(
    _In_ MO_POINTER Left,
    _In_ MO_POINTER Right,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINT8*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer.
    // Using 'volatile PMO_UINT8' would only make the pointer variable itself
    // volatile.

    volatile MO_UINT8* LeftBytes = (volatile MO_UINT8*)Left;
    volatile MO_UINT8* RightBytes = (volatile MO_UINT8*)Right;

    for (MO_UINTN Index = 0u; Index < Length; ++Index)
    {
        MO_UINT8 CurrentLeft = LeftBytes[Index];
        MO_UINT8 CurrentRight = RightBytes[Index];
        if (CurrentLeft != CurrentRight)
        {
            return CurrentLeft > CurrentRight ? 1 : -1;
        }
    }
    return 0;
}

MO_FORCEINLINE MO_INTN MoRuntimeInternalMemoryCompareNativeAligned(
    _In_ MO_POINTER Left,
    _In_ MO_POINTER Right,
    _In_ MO_UINTN Length)
{
    // This library is designed for freestanding environments where C standard
    // library functions (like memcpy) and SIMD intrinsics may not be available.
    // Use volatile to ensure no dependencies on such functions or intrinsics.
    // Use 'volatile MO_UINTN*' to ensure the compiler treats the data pointed
    // to as volatile, forcing actual memory writes to the framebuffer.
    // Using 'volatile PMO_UINTN' would only make the pointer variable itself
    // volatile.

    volatile MO_UINTN* LeftNative = (volatile MO_UINTN*)Left;
    volatile MO_UINTN* RightNative = (volatile MO_UINTN*)Right;

    MO_UINTN NativeCount = Length / sizeof(MO_UINTN);
    for (MO_UINTN Index = 0u; Index < NativeCount; ++Index)
    {
        MO_UINTN CurrentLeft = LeftNative[Index];
        MO_UINTN CurrentRight = RightNative[Index];
        if (CurrentLeft != CurrentRight)
        {
            return MoRuntimeInternalMemoryCompareUnaligned(
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
    if (Left == Right)
    {
        // Both are the same address, consider equal.
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

    // Make sure the left address is aligned.
    {
        MO_UINTN AlignedLeft = MoRuntimeGetAlignedSize(
            CurrentLeft,
            sizeof(MO_UINTN));

        // If the left address is not aligned, process the unaligned part with
        // generic implementation first.
        if (AlignedLeft != CurrentLeft)
        {
            MO_UINTN UnalignedLength = AlignedLeft - CurrentLeft;
            if (UnalignedLength > Length)
            {
                UnalignedLength = Length;
            }
            CurrentResult = MoRuntimeInternalMemoryCompareUnaligned(
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
            CurrentRight += UnalignedLength;
            Length -= UnalignedLength;
        }
    }

    // If the buffer is not large enough, use the generic implementation.
    if (Length < sizeof(MO_UINTN))
    {
        CurrentResult = MoRuntimeInternalMemoryCompareUnaligned(
            (MO_POINTER)(CurrentLeft),
            (MO_POINTER)(CurrentRight),
            Length);
        return CurrentResult;
    }

    // Make sure the right address is aligned.
    {
        MO_UINTN AlignedRight = MoRuntimeGetAlignedSize(
            CurrentRight,
            sizeof(MO_UINTN));

        // If the right address is not aligned, use the generic implementation.
        if (AlignedRight != CurrentRight)
        {
            CurrentResult = MoRuntimeInternalMemoryCompareUnaligned(
                (MO_POINTER)(CurrentLeft),
                (MO_POINTER)(CurrentRight),
                Length);
            return CurrentResult;
        }
    }

    MO_UINTN UnalignedLength = Length % sizeof(MO_UINTN);
    MO_UINTN AlignedLength = Length - UnalignedLength;

    // Process the aligned part with native type implementation.
    CurrentResult = MoRuntimeInternalMemoryCompareNativeAligned(
        (MO_POINTER)(CurrentLeft),
        (MO_POINTER)(CurrentRight),
        AlignedLength);
    if (CurrentResult != 0)
    {
        return CurrentResult;
    }

    // Process the remaining unaligned part with generic implementation.
    CurrentResult = MoRuntimeInternalMemoryCompareUnaligned(
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

            // This library is designed for freestanding environments where C
            // standard library functions (like memcpy) and SIMD intrinsics may
            // not be available. Use volatile to ensure no dependencies on such
            // functions or intrinsics. Use 'volatile MO_UINT8*' to ensure the
            // compiler treats the data pointed to as volatile, forcing actual
            // memory writes to the framebuffer. Using 'volatile PMO_UINT8'
            // would only make the pointer variable itself volatile.

            volatile MO_UINT8* LeftBytes = (volatile MO_UINT8*)Left;
            volatile MO_UINT8* RightBytes = (volatile MO_UINT8*)Right;

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

EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapTestRange(
    _In_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue)
{
    if (!Bitmap || !Length)
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    PMO_UINT8 Bytes = (PMO_UINT8)Bitmap;

    if (Length > (MO_UINTN_MAX - StartIndex))
    {
        // Overflow.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    if (1 == Length)
    {
        // Optimize for single bit test.
        MO_UINT8 Mask = (MO_UINT8)(1u << (StartIndex & 7u));
        MO_UINT8 ExpectedByteValue = ExpectedValue ? Mask : 0x00u;
        return (((Bytes[StartIndex >> 3u] & Mask) == ExpectedByteValue)
            ? MO_RESULT_SUCCESS_OK
            : MO_RESULT_SUCCESS_FALSE);
    }

    MO_UINTN EndIndex = StartIndex + Length;

    MO_UINTN FirstByteIndex = StartIndex >> 3u;
    MO_UINTN LastByteIndex = (EndIndex - 1u) >> 3u;
    MO_UINT8 FirstByteMask = (MO_UINT8)(0xFFu << (StartIndex & 7u));
    MO_UINT8 LastByteMask = (MO_UINT8)(0xFFu >> (7u - ((EndIndex - 1u) & 7u)));

    if (FirstByteIndex == LastByteIndex)
    {
        // Optimize for single byte test.
        MO_UINT8 Mask = (MO_UINT8)(FirstByteMask & LastByteMask);
        MO_UINT8 ExpectedByteValue = ExpectedValue ? Mask : 0x00u;
        return (((Bytes[FirstByteIndex] & Mask) == ExpectedByteValue)
            ? MO_RESULT_SUCCESS_OK
            : MO_RESULT_SUCCESS_FALSE);
    }

    MO_UINT8 ExpectedFirstByteValue = ExpectedValue ? FirstByteMask : 0x00u;
    MO_UINT8 ExpectedMiddleByteValue = ExpectedValue ? 0xFFu : 0x00u;
    MO_UINT8 ExpectedLastByteValue = ExpectedValue ? LastByteMask : 0x00u;

    if ((Bytes[FirstByteIndex] & FirstByteMask) != ExpectedFirstByteValue)
    {
        return MO_RESULT_SUCCESS_FALSE;
    }
    for (MO_UINTN Index = FirstByteIndex + 1u; Index < LastByteIndex; ++Index)
    {
        if (Bytes[Index] != ExpectedMiddleByteValue)
        {
            return MO_RESULT_SUCCESS_FALSE;
        }
    }
    if ((Bytes[LastByteIndex] & LastByteMask) != ExpectedLastByteValue)
    {
        return MO_RESULT_SUCCESS_FALSE;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapFillRange(
    _Inout_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue)
{
    if (!Bitmap || !Length)
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    PMO_UINT8 Bytes = (PMO_UINT8)Bitmap;

    if (Length > (MO_UINTN_MAX - StartIndex))
    {
        // Overflow.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    if (1 == Length)
    {
        // Optimize for single bit fill.
        MO_UINT8 Mask = (MO_UINT8)(1u << (StartIndex & 7u));
        if (ExpectedValue)
        {
            // Single bit set.
            Bytes[StartIndex >> 3u] |= Mask;
        }
        else
        {
            // Single bit clear.
            Bytes[StartIndex >> 3u] &= (MO_UINT8)(~Mask);
        }
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINTN EndIndex = StartIndex + Length;

    MO_UINTN FirstByteIndex = StartIndex >> 3u;
    MO_UINTN LastByteIndex = (EndIndex - 1u) >> 3u;
    MO_UINT8 FirstByteMask = (MO_UINT8)(0xFFu << (StartIndex & 7u));
    MO_UINT8 LastByteMask = (MO_UINT8)(0xFFu >> (7u - ((EndIndex - 1u) & 7u)));

    if (FirstByteIndex == LastByteIndex)
    {
        // Optimize for single byte fill.
        if (ExpectedValue)
        {
            // Single byte set.
            Bytes[FirstByteIndex] |= (FirstByteMask & LastByteMask);
        }
        else
        {
            // Single byte clear.
            Bytes[FirstByteIndex] &= ~(FirstByteMask & LastByteMask);
        }
        return MO_RESULT_SUCCESS_OK;
    }

    if (ExpectedValue)
    {
        // Fill with set for the first byte.
        Bytes[FirstByteIndex] |= FirstByteMask;
    }
    else
    {
        // Fill with clear for the first byte.
        Bytes[FirstByteIndex] &= ~FirstByteMask;
    }

    MO_UINTN MiddleLength = (LastByteIndex - FirstByteIndex - 1u);
    if (MiddleLength)
    {
        if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
            &Bytes[FirstByteIndex + 1u],
            ExpectedValue ? 0xFFu : 0x00u,
            MiddleLength))
        {
            return MO_RESULT_ERROR_UNEXPECTED;
        }
    }

    if (ExpectedValue)
    {
        // Fill with set for the last byte.
        Bytes[LastByteIndex] |= LastByteMask;
    }
    else
    {
        // Fill with clear for the last byte.
        Bytes[LastByteIndex] &= ~LastByteMask;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapQueryContinuousRunLength(
    _Out_opt_ PMO_UINTN RunLength,
    _Out_opt_ PMO_BOOL BitValue,
    _In_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN MaximumIndex)
{
    if (!Bitmap || !(StartIndex < MaximumIndex))
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    PMO_UINT8 Bytes = (PMO_UINT8)Bitmap;

    if (RunLength)
    {
        *RunLength = 0u;
    }

    if (BitValue)
    {
        *BitValue = MO_FALSE;
    }

    MO_UINTN CurrentIndex = StartIndex;

    MO_UINT8 CurrentBitMask = (MO_UINT8)(1u << (CurrentIndex & 7u));
    MO_BOOL CurrentBitValue =
        (Bytes[CurrentIndex >> 3u] & CurrentBitMask) != 0u;
    if (BitValue)
    {
        *BitValue = CurrentBitValue;
    }

    while (CurrentIndex < MaximumIndex)
    {
        MO_UINTN RemainingBitsInByte = 8u - (CurrentIndex & 7u);
        MO_UINTN MaximumBitsInByte = MaximumIndex - CurrentIndex;
        MO_UINTN BitsToCheck = (RemainingBitsInByte < MaximumBitsInByte)
            ? RemainingBitsInByte
            : MaximumBitsInByte;

        MO_UINTN RunLengthInByte = 0u;
        MO_UINT8 CurrentByte = Bytes[CurrentIndex >> 3u];
        MO_UINTN CurrentBitOffset = CurrentIndex & 7u;
        MO_BOOL UseBitScan = MO_TRUE;

        if (!CurrentBitOffset)
        {
            if (0x00u == CurrentByte)
            {
                RunLengthInByte = CurrentBitValue ? 0u : BitsToCheck;
                UseBitScan = MO_FALSE;
            }
            else if (0xFFu == CurrentByte)
            {
                RunLengthInByte = CurrentBitValue ? BitsToCheck : 0u;
                UseBitScan = MO_FALSE;
            }
        }

        if (UseBitScan)
        {
            MO_UINT8 Mask = (MO_UINT8)(1u << CurrentBitOffset);
            while (Mask != 0u && RunLengthInByte < BitsToCheck)
            {
                MO_BOOL BitValueInByte = (CurrentByte & Mask) != 0u;
                if (BitValueInByte != CurrentBitValue)
                {
                    break;
                }
                ++RunLengthInByte;
                Mask = (MO_UINT8)(Mask << 1u);
            }
        }

        CurrentIndex += RunLengthInByte;
        if (RunLengthInByte < BitsToCheck)
        {
            // Found different bit value.
            break;
        }
    }

    if (RunLength)
    {
        *RunLength = CurrentIndex - StartIndex;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeCalculateSumByte(
    _Out_ PMO_UINT8 SumByte,
    _In_ MO_POINTER Buffer,
    _In_ MO_UINTN Size)
{
    if (!SumByte || !Buffer || !Size)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *SumByte = 0u;

    MO_UINT8 Result = 0u;
    for (MO_UINTN Index = 0u; Index < Size; ++Index)
    {
        Result += ((PMO_UINT8)Buffer)[Index];
    }
    *SumByte = Result;

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeCalculateChecksumByte(
    _Out_ PMO_UINT8 ChecksumByte,
    _In_ MO_POINTER Buffer,
    _In_ MO_UINTN Size)
{
    MO_RESULT ResultCode = MoRuntimeCalculateSumByte(
        ChecksumByte,
        Buffer,
        Size);
    if (MO_RESULT_SUCCESS_OK != ResultCode)
    {
        return ResultCode;
    }
    *ChecksumByte = (MO_UINT8)(~(*ChecksumByte) + 1u);
    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeConvertUnsignedIntegerToHexString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_UINTN Value,
    _In_ MO_UINTN ValueWidth,
    _In_ MO_BOOL Uppercase,
    _In_ MO_BOOL Prefix)
{
    if (!Buffer && !RequiredBufferSize)
    {
        // At least one output parameter is required.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    CONST MO_UINTN MaximumValueWidth = sizeof(Value) * 8u;
    if (ValueWidth < 4u || ValueWidth > MaximumValueWidth || ValueWidth & 3u)
    {
        // The value width must be multiple of 4 and between 4 and the number of
        // bits of MO_UINTN.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    if (ValueWidth < MaximumValueWidth)
    {
        // Use mask to limit the value width.
        Value &= ((MO_UINTN)1u << ValueWidth) - 1u;
    }

    MO_UINTN NibbleCount = ValueWidth >> 2u;
    MO_UINTN TotalLength = NibbleCount;
    if (Prefix)
    {
        // With "0x" prefix.
        TotalLength += 2u;
    }
    // Including null terminator.
    TotalLength += 1u;

    if (RequiredBufferSize)
    {
        *RequiredBufferSize = TotalLength;
    }

    if (Buffer)
    {
        if (BufferSize < TotalLength)
        {
            // Buffer too small.
            return MO_RESULT_ERROR_OUT_OF_MEMORY;
        }

        MO_CHAR HexNonDigitCharacterBase = Uppercase ? 'A' : 'a';

        MO_UINTN CurrentIndex = 0u;
        if (Prefix)
        {
            Buffer[CurrentIndex++] = '0';
            Buffer[CurrentIndex++] = 'x';
        }
        for (MO_UINTN Index = 0u; Index < NibbleCount; ++Index)
        {
            MO_UINTN ShiftAmount = (NibbleCount - 1u - Index) * 4u;
            MO_UINT8 Nibble = (MO_UINT8)((Value >> ShiftAmount) & 0xFu);
            Buffer[CurrentIndex++] = (MO_CHAR)((Nibble < 10u)
                ? ('0' + Nibble)
                : (HexNonDigitCharacterBase + (Nibble - 10u)));
        }

        Buffer[CurrentIndex] = '\0';
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeConvertIntegerToDecimalString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_INTN Value)
{
    if (!Buffer && !RequiredBufferSize)
    {
        // At least one output parameter is required.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_BOOL IsNegative = (Value < 0);
    MO_UINTN AbsoluteValue = (MO_UINTN)Value;
    if (IsNegative)
    {
        AbsoluteValue = 0u - AbsoluteValue;
    }
    if (MO_INTN_MIN == Value)
    {
        // Handling MO_INTN_MIN case to avoid overflow.
        AbsoluteValue = (MO_UINTN)(-(MO_INTN_MIN + 1)) + 1u;
    }

    MO_UINTN DigitCount = 0u;
    MO_UINTN TempValue = AbsoluteValue;
    do
    {
        ++DigitCount;
        TempValue /= 10u;
    } while (TempValue != 0u);
    MO_UINTN TotalLength = DigitCount;
    if (IsNegative)
    {
        // For negative sign.
        ++TotalLength;
    }
    // Including null terminator.
    TotalLength += 1u;

    if (RequiredBufferSize)
    {
        *RequiredBufferSize = TotalLength;
    }

    if (Buffer)
    {
        if (BufferSize < TotalLength)
        {
            // Buffer too small.
            return MO_RESULT_ERROR_OUT_OF_MEMORY;
        }

        MO_UINTN CurrentIndex = TotalLength - 1u;
        Buffer[CurrentIndex--] = '\0';
        MO_UINTN CurrentValue = AbsoluteValue;
        do
        {
            MO_UINT8 Digit = CurrentValue % 10u;
            Buffer[CurrentIndex--] = (MO_CHAR)('0' + Digit);
            CurrentValue /= 10u;
        } while (CurrentValue != 0u);
        if (IsNegative)
        {
            Buffer[CurrentIndex] = '-';
        }
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeConvertUnsignedIntegerToDecimalString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_UINTN Value)
{
    if (!Buffer && !RequiredBufferSize)
    {
        // At least one output parameter is required.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_UINTN DigitCount = 0u;
    MO_UINTN TempValue = Value;
    do
    {
        ++DigitCount;
        TempValue /= 10u;
    } while (TempValue != 0u);
    MO_UINTN TotalLength = DigitCount;
    // Including null terminator.
    TotalLength += 1u;

    if (RequiredBufferSize)
    {
        *RequiredBufferSize = TotalLength;
    }

    if (Buffer)
    {
        if (BufferSize < TotalLength)
        {
            // Buffer too small.
            return MO_RESULT_ERROR_OUT_OF_MEMORY;
        }

        MO_UINTN CurrentIndex = TotalLength - 1u;
        Buffer[CurrentIndex--] = '\0';
        MO_UINTN CurrentValue = Value;
        do
        {
            MO_UINT8 Digit = CurrentValue % 10u;
            Buffer[CurrentIndex--] = (MO_CHAR)('0' + Digit);
            CurrentValue /= 10u;
        } while (CurrentValue != 0u);
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeStringValidate(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN MaximumLength)
{
    if (!String || !MaximumLength)
    {
        // We need a non-null string and a non-zero maximum length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MaximumLength < MO_RUNTIME_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR)
    {
        // Limit maximum length to prevent overflow.
        MaximumLength = MO_RUNTIME_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR;
    }

    if (Length)
    {
        // Initialize output length if provided.
        *Length = 0u;
    }

    MO_UINTN CurrentLength = 0u;
    while (CurrentLength < MaximumLength)
    {
        if ('\0' == String[CurrentLength])
        {
            // It's a valid null-terminated string.
            if (Length)
            {
                *Length = CurrentLength;
            }
            return MO_RESULT_SUCCESS_OK;
        }
        ++CurrentLength;
    }

    // Reached maximum length without finding null terminator.
    return MO_RESULT_ERROR_OUT_OF_BOUNDS;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringValidate(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN MaximumLength)
{
    if (!WideString || !MaximumLength)
    {
        // We need a non-null wide string and a non-zero maximum length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MaximumLength < MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR)
    {
        // Limit maximum length to prevent overflow.
        MaximumLength = MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR;
    }

    if (Length)
    {
        // Initialize output length if provided.
        *Length = 0u;
    }

    MO_UINTN CurrentLength = 0u;
    while (CurrentLength < MaximumLength)
    {
        if (L'\0' == WideString[CurrentLength])
        {
            // It's a valid null-terminated wide string.
            if (Length)
            {
                *Length = CurrentLength;
            }
            return MO_RESULT_SUCCESS_OK;
        }
        ++CurrentLength;
    }

    // Reached maximum length without finding null terminator.
    return MO_RESULT_ERROR_OUT_OF_BOUNDS;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeStringCopy(
    _Out_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source,
    _In_ MO_UINTN SourceLength)
{
    if (!Destination || !MaximumLength || !Source)
    {
        // We need non-null destination and source, and non-zero destination
        // length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MO_RUNTIME_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR < MaximumLength ||
        MO_RUNTIME_STRING_MAXIMUM_LENGTH < SourceLength)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    CONST MO_UINTN SourceSize = SourceLength * sizeof(MO_CHAR);
    CONST MO_UINTN RequiredSize = SourceSize + sizeof(MO_CHAR);

    if (SourceLength > MaximumLength - 1u)
    {
        // Not enough space in destination for source and null terminator.
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    MO_UINTN DestinationStart = (MO_UINTN)(Destination);
    MO_UINTN SourceStart = (MO_UINTN)(Source);

    if (RequiredSize > (MO_UINTN_MAX - DestinationStart) ||
        RequiredSize > (MO_UINTN_MAX - SourceStart))
    {
        // Overflow source or destination range is not allowed.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    if ((DestinationStart < (SourceStart + RequiredSize)) &&
        (SourceStart < (DestinationStart + RequiredSize)))
    {
        // Overlap source or destination is not allowed.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (SourceLength)
    {
        // Copy the source string to destination.
        MoRuntimeInternalMemoryCopy(
            (MO_POINTER)(Destination),
            (MO_POINTER)(Source),
            SourceSize);
    }

    // Null terminate the destination string.
    Destination[SourceLength] = '\0';

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringCopy(
    _Out_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source,
    _In_ MO_UINTN SourceLength)
{
    if (!Destination || !MaximumLength || !Source)
    {
        // We need non-null destination and source, and non-zero destination
        // length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR < MaximumLength ||
        MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH < SourceLength)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    CONST MO_UINTN SourceSize = SourceLength * sizeof(MO_WIDE_CHAR);
    CONST MO_UINTN RequiredSize = SourceSize + sizeof(MO_WIDE_CHAR);

    if (SourceLength > MaximumLength - 1u)
    {
        // Not enough space in destination for source and null terminator.
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    MO_UINTN DestinationStart = (MO_UINTN)(Destination);
    MO_UINTN SourceStart = (MO_UINTN)(Source);

    if (RequiredSize > (MO_UINTN_MAX - DestinationStart) ||
        RequiredSize > (MO_UINTN_MAX - SourceStart))
    {
        // Overflow source or destination range is not allowed.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    if ((DestinationStart < (SourceStart + RequiredSize)) &&
        (SourceStart < (DestinationStart + RequiredSize)))
    {
        // Overlap source or destination is not allowed.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (SourceLength)
    {
        // Copy the source string to destination.
        MoRuntimeInternalMemoryCopy(
            (MO_POINTER)(Destination),
            (MO_POINTER)(Source),
            SourceSize);
    }

    // Null terminate the destination string.
    Destination[SourceLength] = L'\0';

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeStringConcatenate(
    _Inout_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source,
    _In_ MO_UINTN SourceLength)
{
    if (!Destination || !MaximumLength || !Source)
    {
        // We need non-null destination and source, and non-zero destination
        // length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MO_RUNTIME_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR < MaximumLength ||
        MO_RUNTIME_STRING_MAXIMUM_LENGTH < SourceLength)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    // Disallow any overlap between the whole destination buffer range and the
    // source range (including null terminator).
    {
        MO_UINTN DestinationStart = (MO_UINTN)Destination;
        MO_UINTN DestinationSize = MaximumLength * sizeof(MO_CHAR);

        MO_UINTN SourceStart = (MO_UINTN)Source;
        MO_UINTN SourceSize = (SourceLength + 1u) * sizeof(MO_CHAR);

        if (DestinationSize > (MO_UINTN_MAX - DestinationStart) ||
            SourceSize > (MO_UINTN_MAX - SourceStart))
        {
            // Overflow source or destination range is not allowed.
            return MO_RESULT_ERROR_OUT_OF_BOUNDS;
        }

        if ((DestinationStart < (SourceStart + SourceSize)) &&
            (SourceStart < (DestinationStart + DestinationSize)))
        {
            // Overlap source or destination is not allowed.
            return MO_RESULT_ERROR_INVALID_PARAMETER;
        }
    }

    MO_UINTN DestinationLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeStringValidate(
        &DestinationLength,
        Destination,
        MaximumLength))
    {
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN SourceActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeStringValidate(
        &SourceActualLength,
        Source,
        SourceLength + 1u))
    {
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (SourceActualLength != SourceLength)
    {
        // Source length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    return MoRuntimeStringCopy(
        &Destination[DestinationLength],
        MaximumLength - DestinationLength,
        Source,
        SourceActualLength);
}

EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringConcatenate(
    _Inout_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source,
    _In_ MO_UINTN SourceLength)
{
    if (!Destination || !MaximumLength || !Source)
    {
        // We need non-null destination and source, and non-zero destination
        // length.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH_WITH_TERMINATOR < MaximumLength ||
        MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH < SourceLength)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    // Disallow any overlap between the whole destination buffer range and the
    // source range (including null terminator).
    {
        MO_UINTN DestinationStart = (MO_UINTN)Destination;
        MO_UINTN DestinationSize = MaximumLength * sizeof(MO_WIDE_CHAR);

        MO_UINTN SourceStart = (MO_UINTN)Source;
        MO_UINTN SourceSize = (SourceLength + 1u) * sizeof(MO_WIDE_CHAR);

        if (DestinationSize > (MO_UINTN_MAX - DestinationStart) ||
            SourceSize > (MO_UINTN_MAX - SourceStart))
        {
            // Overflow source or destination range is not allowed.
            return MO_RESULT_ERROR_OUT_OF_BOUNDS;
        }

        if ((DestinationStart < (SourceStart + SourceSize)) &&
            (SourceStart < (DestinationStart + DestinationSize)))
        {
            // Overlap source or destination is not allowed.
            return MO_RESULT_ERROR_INVALID_PARAMETER;
        }
    }

    MO_UINTN DestinationLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeWideStringValidate(
        &DestinationLength,
        Destination,
        MaximumLength))
    {
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN SourceActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeWideStringValidate(
        &SourceActualLength,
        Source,
        SourceLength + 1u))
    {
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (SourceActualLength != SourceLength)
    {
        // Source length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    return MoRuntimeWideStringCopy(
        &Destination[DestinationLength],
        MaximumLength - DestinationLength,
        Source,
        SourceActualLength);
}

EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindFirstCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN Length,
    _In_ MO_CHAR Character)
{
    if (!Index || !String)
    {
        // We need non-null output index and string.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *Index = MO_UINTN_MAX;

    if (MO_RUNTIME_STRING_MAXIMUM_LENGTH < Length)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN ActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeStringValidate(
        &ActualLength,
        String,
        Length + 1u))
    {
        // String is not valid within the specified length.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (Length != ActualLength)
    {
        // Length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    for (MO_UINTN CurrentIndex = 0u; CurrentIndex < Length; ++CurrentIndex)
    {
        if (String[CurrentIndex] == Character)
        {
            *Index = CurrentIndex;
            return MO_RESULT_SUCCESS_OK;
        }
    }

    return MO_RESULT_SUCCESS_FALSE;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindFirstCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN Length,
    _In_ MO_WIDE_CHAR WideCharacter)
{
    if (!Index || !WideString)
    {
        // We need non-null output index and wide string.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *Index = MO_UINTN_MAX;

    if (MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH < Length)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN ActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeWideStringValidate(
        &ActualLength,
        WideString,
        Length + 1u))
    {
        // String is not valid within the specified length.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (Length != ActualLength)
    {
        // Length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    for (MO_UINTN CurrentIndex = 0u; CurrentIndex < Length; ++CurrentIndex)
    {
        if (WideString[CurrentIndex] == WideCharacter)
        {
            *Index = CurrentIndex;
            return MO_RESULT_SUCCESS_OK;
        }
    }

    return MO_RESULT_SUCCESS_FALSE;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindLastCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN Length,
    _In_ MO_CHAR Character)
{
    if (!Index || !String)
    {
        // We need non-null output index and string.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *Index = MO_UINTN_MAX;

    if (MO_RUNTIME_STRING_MAXIMUM_LENGTH < Length)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN ActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeStringValidate(
        &ActualLength,
        String,
        Length + 1u))
    {
        // String is not valid within the specified length.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (Length != ActualLength)
    {
        // Length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    for (MO_UINTN CurrentIndex = Length; CurrentIndex > 0u; --CurrentIndex)
    {
        if (String[CurrentIndex - 1u] == Character)
        {
            *Index = CurrentIndex - 1u;
            return MO_RESULT_SUCCESS_OK;
        }
    }

    return MO_RESULT_SUCCESS_FALSE;
}

EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindLastCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN Length,
    _In_ MO_WIDE_CHAR WideCharacter)
{
    if (!Index || !WideString)
    {
        // We need non-null output index and wide string.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *Index = MO_UINTN_MAX;

    if (MO_RUNTIME_WIDE_STRING_MAXIMUM_LENGTH < Length)
    {
        // Prevent overflow when calculating required size.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }

    MO_UINTN ActualLength = 0u;
    if (MO_RESULT_SUCCESS_OK != MoRuntimeWideStringValidate(
        &ActualLength,
        WideString,
        Length + 1u))
    {
        // String is not valid within the specified length.
        return MO_RESULT_ERROR_OUT_OF_BOUNDS;
    }
    if (Length != ActualLength)
    {
        // Length mismatch.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    for (MO_UINTN CurrentIndex = Length; CurrentIndex > 0u; --CurrentIndex)
    {
        if (WideString[CurrentIndex - 1u] == WideCharacter)
        {
            *Index = CurrentIndex - 1u;
            return MO_RESULT_SUCCESS_OK;
        }
    }

    return MO_RESULT_SUCCESS_FALSE;
} 
