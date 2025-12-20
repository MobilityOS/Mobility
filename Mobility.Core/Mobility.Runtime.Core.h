/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Runtime.Core.h
 * PURPOSE:    Definition for Mobility Runtime Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_CORE
#define MOBILITY_RUNTIME_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Calculates the aligned size based on the specified size and alignment.
 * @param Size The input size value which needs to be aligned.
 * @param Alignment The size value to which it wants to align.
 * @return The output size value aligned to the specified alignment.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeGetAlignedSize(
    _In_ MO_UINTN Size,
    _In_ MO_UINTN Alignment);

/**
 * @brief Fills a memory buffer with the specified byte value. This function is
 *        implemented with alignment-aware logic to improve performance on
 *        native word boundaries.
 * @param Buffer The target memory buffer to be filled. If this parameter is
 *               NULL, the function returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Value The byte value used to fill the target memory buffer.
 * @param Length The length of the target memory buffer in bytes. If this
 *               parameter is zero, the function does nothing and returns
 *               MO_RESULT_SUCCESS_OK.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeMemoryFillByte(
    _Out_ MO_POINTER Buffer,
    _In_ MO_UINT8 Value,
    _In_ MO_UINTN Length);

#endif // !MOBILITY_RUNTIME_CORE
