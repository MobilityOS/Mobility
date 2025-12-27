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
 * @brief Compares two memory buffers byte by byte.
 * @param Left The pointer to the first memory buffer.
 * @param Right The pointer to the second memory buffer.
 * @param Length The length of the memory buffers in bytes.
 * @return Returns 0 if one of the following conditions is met:
 *         - Left is equal to Right.
 *         - Left and Right are both nullptr.
 *         - Length is zero.
 *         Returns a value less than 0 if one of the following conditions is
 *         met:
 *         - Left is less than Right.
 *         - Left is nullptr and Right is not nullptr.
 *         Returns a value greater than 0 if one of the following conditions is
 *         met:
 *         - Left is greater than Right.
 *         - Right is nullptr and Left is not nullptr.
 */
EXTERN_C MO_INTN MOAPI MoRuntimeMemoryCompare(
    _In_opt_ MO_POINTER Left,
    _In_opt_ MO_POINTER Right,
    _In_opt_ MO_UINTN Length);

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

/**
 * @brief Test a range of bits in a bitmap for an expected value.
 * @param Bitmap The bitmap to be tested. The caller must ensure that the bitmap
 *               pointer is valid and that the buffer is large enough to cover
 *               the specified bit range.
 * @param StartIndex The starting index of the range to be tested.
 * @param Length The length of the range to be tested.
 * @param ExpectedValue The expected value of the bits in the range.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapTestRange(
    _In_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue);

/**
 * @brief Fill a range of bits in a bitmap with a specified value.
 * @param Bitmap The bitmap to be filled. The caller must ensure that the bitmap
 *               pointer is valid and that the buffer is large enough to cover
 *               the specified bit range.
 * @param StartIndex The starting index of the range to be filled.
 * @param Length The length of the range to be filled.
 * @param ExpectedValue The value to assign to the bits in the range.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapFillRange(
    _Inout_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN Length,
    _In_ MO_BOOL ExpectedValue);

/**
 * @brief Query the length of the continuous run of the same bit value starting
 *        from a specified index.
 * @param RunLength Receives the continuous run length. Optional.
 * @param BitValue Receives the bit value at StartIndex. Optional.
 * @param Bitmap The bitmap to be queried. The caller must ensure that the bitmap
 *               pointer is valid and that the buffer is large enough to cover
 *               the specified bit range.
 * @param StartIndex The start bit index to query.
 * @param MaximumIndex The maximum bit index (exclusive).
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeBitmapQueryContinuousRunLength(
    _Out_opt_ PMO_UINTN RunLength,
    _Out_opt_ PMO_BOOL BitValue,
    _In_ MO_POINTER Bitmap,
    _In_ MO_UINTN StartIndex,
    _In_ MO_UINTN MaximumIndex);

/**
 * @brief Calculates the 8-bit sum for the requested region.
 * @param SumByte The pointer to store the calculated sum byte. If this
 *                parameter is nullptr, the function returns
 *                MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Buffer The pointer to the target buffer. If this parameter is
 *               nullptr, the function returns
 *               MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Size The size of the target buffer in bytes. If this parameter is
 *             not greater than zero, the function returns
 *             MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeCalculateSumByte(
    _Out_ PMO_UINT8 SumByte,
    _In_ MO_POINTER Buffer,
    _In_ MO_UINTN Size);

/**
 * @brief Calculates the 8-bit checksum for the requested region.
 * @param ChecksumByte The pointer to store the calculated checksum byte. If
 *                     this parameter is nullptr, the function returns
 *                     MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Buffer The pointer to the target buffer. If this parameter is
 *               nullptr, the function returns
 *               MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param Size The size of the target buffer in bytes. If this parameter is
 *             not greater than zero, the function returns
 *             MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeCalculateChecksumByte(
    _Out_ PMO_UINT8 ChecksumByte,
    _In_ MO_POINTER Buffer,
    _In_ MO_UINTN Size);

/**
 * @brief Convert an unsigned integer to a hexadecimal string.
 * @param Buffer The buffer to receive the hexadecimal string. This parameter
 *               can be nullptr if only the required buffer size is queried.
 * @param RequiredBufferSize The pointer to receive the required buffer size in
 *                           characters, including the null terminator. This
 *                           parameter can be nullptr if the required buffer
 *                           size is not needed like only converting the value.
 * @param BufferSize The size of the Buffer in characters, including the null
 *                   terminator. If the size is insufficient, the function
 *                   returns MO_RESULT_ERROR_OUT_OF_MEMORY.
 * @param Value The unsigned integer value to convert.
 * @param ValueWidth The width of the value in bits. It must be multiple of 4
 *                   and between 4 and the number of bits of MO_UINTN.
 * @param Uppercase If this parameter is MO_TRUE, the hexadecimal digits A-F are
 *                  used. Otherwise, the hexadecimal digits a-f are used.
 * @param Prefix If this parameter is MO_TRUE, the "0x" prefix is added before
 *               the hexadecimal digits.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeConvertUnsignedIntegerToHexString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_UINTN Value,
    _In_ MO_UINTN ValueWidth,
    _In_ MO_BOOL Uppercase,
    _In_ MO_BOOL Prefix);

/**
 * @brief Convert a signed integer to a decimal string.
 * @param Buffer The buffer to receive the converted string. This parameter can
 *               be nullptr if only the required buffer size is queried.
 * @param RequiredBufferSize The pointer to receive the required buffer size in
 *                           characters, including the null terminator. This
 *                           parameter can be nullptr if the required buffer
 *                           size is not needed like only converting the value.
 * @param BufferSize The size of the Buffer in characters, including the null
 *                   terminator. If the size is insufficient, the function
 *                   returns MO_RESULT_ERROR_OUT_OF_MEMORY.
 * @param Value The signed integer value to convert.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeConvertIntegerToDecimalString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_INTN Value);

/**
 * @brief Convert an unsigned integer to a decimal string.
 * @param Buffer The buffer to receive the converted string. This parameter can
 *               be nullptr if only the required buffer size is queried.
 * @param RequiredBufferSize The pointer to receive the required buffer size in
 *                           characters, including the null terminator. This
 *                           parameter can be nullptr if the required buffer
 *                           size is not needed like only converting the value.
 * @param BufferSize The size of the Buffer in characters, including the null
 *                   terminator. If the size is insufficient, the function
 *                   returns MO_RESULT_ERROR_OUT_OF_MEMORY.
 * @param Value The unsigned integer value to convert.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeConvertUnsignedIntegerToDecimalString(
    _Out_opt_ PMO_CHAR Buffer,
    _Out_opt_ PMO_UINTN RequiredBufferSize,
    _In_ MO_UINTN BufferSize,
    _In_ MO_UINTN Value);

#endif // !MOBILITY_RUNTIME_CORE
