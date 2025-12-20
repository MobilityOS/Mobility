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
 * @param Alignment The size value to which it wants to align. This value must
 *                  be the power of two.
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
 *               nullptr, the function returns
 *               MO_RESULT_ERROR_INVALID_PARAMETER.
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

/**
 * @brief Moves contents from the source memory buffer to the destination memory
 *        buffer. This function correctly handles overlapping memory regions,
 *        and is implemented with alignment-aware logic to improve performance
 *        on native word boundaries.
 * @param Destination The destination memory buffer where the contents will be
 *                    moved to. If this parameter is nullptr, the function
 *                    returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Source The source memory buffer from which the contents will be moved.
 *               If this parameter is nullptr, the function returns
 *               MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Length The length of the memory to be moved in bytes. If this
 *               parameter is zero, the function does nothing and returns
 *               MO_RESULT_SUCCESS_OK.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeMemoryMove(
    _Out_ MO_POINTER Destination,
    _In_ MO_POINTER Source,
    _In_ MO_UINTN Length);

/**
 * @brief Defines the comparison handler used for sorting operations.
 * @param Left The pointer to the left element to be compared.
 * @param Right The pointer to the right element to be compared.
 * @param Context The user-defined context pointer that can be used to pass
 *                through additional data to the comparison function.
 * @return A value less than zero if Left is less than Right, zero if they are
 *         equal, and a value greater than zero if Left is greater than Right.
 */
typedef MO_INTN(MOAPI* PMO_RUNTIME_SORT_COMPARE_HANDLER)(
    _In_ MO_POINTER Left,
    _In_ MO_POINTER Right,
    _In_ MO_POINTER Context);

/**
 * @brief Sorts an array of elements in ascending order using the specified
 *        comparison handler. This function uses the bubble sort algorithm for
 *        sorting.
 * @param ElementArray The pointer to the element array to be sorted.
 * @param ElementCount The number of elements in the element array. If this
 *                     parameter is zero, the function returns
 *                     MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param ElementSize The size of each element in bytes. If this parameter is
 *                    zero, the function returns
 *                    MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param CompareHandler The comparison handler used to determine the order of
 *                       elements. If this parameter is nullptr, the function
 *                       returns MO_RESULT_ERROR_INVALID_PARAMETER. The handler
 *                       should return a value less than zero if the left
 *                       element is less than the right element, zero if they
 *                       are equal, and a value greater than zero if the left
 *                       element is greater than the right element.
 * @param Context The user-defined context pointer that will be passed to the
 *                comparison handler. This parameter is optional and can be
 *                nullptr.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeElementSort(
    _Inout_ MO_POINTER ElementArray,
    _In_ MO_UINTN ElementCount,
    _In_ MO_UINTN ElementSize,
    _In_ PMO_RUNTIME_SORT_COMPARE_HANDLER CompareHandler,
    _In_opt_ MO_POINTER Context);

#endif // !MOBILITY_RUNTIME_CORE
