/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.Bitmap.c
 * PURPOSE:    Implementation for Mobility Memory Bitmap
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Memory.Bitmap.h"

EXTERN_C MO_RESULT MOAPI MoMemoryBitmapTestRange(
    _In_ MO_POINTER BitMap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue)
{
    if (!BitMap || !Length)
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    PMO_UINT8 Bytes = (PMO_UINT8)BitMap;

    MO_UINTN EndIndex = StartIndex + Length;
    if (EndIndex < StartIndex)
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

EXTERN_C MO_RESULT MOAPI MoMemoryBitmapFillRange(
    _In_ MO_POINTER BitMap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue)
{
    if (!BitMap || !Length)
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    // Use volatile to prevent generate memset calls in compiler optimization.
    // Also, ensure the write operations are actually performed to the bitmap.
    volatile PMO_UINT8 Bytes = (volatile PMO_UINT8)BitMap;

    MO_UINTN EndIndex = StartIndex + Length;
    if (EndIndex < StartIndex)
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

    MO_UINT8 ExpectedMiddleByteValue = ExpectedValue ? 0xFFu : 0x00u;
    for (MO_UINTN Index = FirstByteIndex + 1u; Index < LastByteIndex; ++Index)
    {
        Bytes[Index] = ExpectedMiddleByteValue;
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

EXTERN_C MO_RESULT MOAPI MoMemoryBitmapQueryContinuousRunLength(
    _Out_opt_ PMO_UINTN RunLength,
    _Out_opt_ PMO_BOOL BitValue,
    _In_ MO_POINTER BitMap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN MaximumIndex)
{
    if (!BitMap || !(StartIndex < MaximumIndex))
    {
        // Invalid parameters.
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    PMO_UINT8 Bytes = (PMO_UINT8)BitMap;

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
