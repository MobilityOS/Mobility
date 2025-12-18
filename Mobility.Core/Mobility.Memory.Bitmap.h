/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.Bitmap.h
 * PURPOSE:    Definition for Mobility Memory Bitmap
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_MEMORY_BITMAP
#define MOBILITY_MEMORY_BITMAP

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Test a range of bits in a bitmap for an expected value.
 * @param BitMap The bitmap to be tested. The caller must ensure that the bitmap
 *               pointer is valid and that the buffer is large enough to cover
 *               the specified bit range.
 * @param StartIndex The starting index of the range to be tested.
 * @param Length The length of the range to be tested.
 * @param ExpectedValue The expected value of the bits in the range.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryBitmapTestRange(
    _In_ MO_POINTER BitMap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue);

/**
 * @brief Fill a range of bits in a bitmap with a specified value.
 * @param BitMap The bitmap to be filled. The caller must ensure that the bitmap
 *               pointer is valid and that the buffer is large enough to cover
 *               the specified bit range.
 * @param StartIndex The starting index of the range to be filled.
 * @param Length The length of the range to be filled.
 * @param ExpectedValue The value to assign to the bits in the range.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryBitmapFillRange(
    _In_ MO_POINTER BitMap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue);

#endif // !MOBILITY_MEMORY_BITMAP
