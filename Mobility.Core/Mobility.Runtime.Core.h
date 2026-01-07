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
 *                  be the power of two. This value must not be zero.
 * @return The output size value aligned to the specified alignment.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeGetAlignedSize(
    _In_ MO_UINTN Size,
    _In_ MO_UINTN Alignment);

/**
 * @brief Calculate the maximum valid length of an array address that can be
 *        used for memory operations without causing overflow. It's not a way
 *        to get the actual length of an array.
 * @param ElementArray The base address of the element array. The caller must
 *                     ensure this pointer is valid.
 * @param ElementSize The size of each element in the array. The caller must
 *                    ensure this value is non-zero.
 * @return The maximum length of the array. Returns 0 on invalid parameters or
 *         no valid length.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeMemoryCalculateMaximumValidLength(
    _In_ MO_CONSTANT_POINTER ElementArray,
    _In_ MO_UINTN ElementSize);

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

/**
 * @brief Calculate the maximum valid length of a string that can be used for
 *        memory operations without causing overflow. It's not a way to get the
 *        actual length of a string.
 * @param String The base address of the string. The caller must ensure this
 *               pointer is valid.
 * @return The maximum length of the string including the null terminator.
 *         Returns 0 on invalid parameters or no valid length.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeStringCalculateMaximumValidLength(
    _In_ MO_CONSTANT_STRING String);

/**
 * @brief Calculate the maximum valid length of a wide string that can be used
 *        for memory operations without causing overflow. It's not a way to get
 *        the actual length of a wide string.
 * @param WideString The base address of the wide string. The caller must ensure
 *                   this pointer is valid.
 * @return The maximum length of the wide string including the null terminator.
 *         Returns 0 on invalid parameters or no valid length.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeWideStringCalculateMaximumValidLength(
    _In_ MO_CONSTANT_WIDE_STRING WideString);

/**
 * @brief Validate a null-terminated string within a maximum length, and
 *        optionally retrieve the length of the string.
 * @param Length The pointer to receive the length of the string without the
 *               null terminator. This parameter is optional if the caller only
 *               wants to validate the string. If the string is invalid, the
 *               output length will be set to zero.
 * @param String The non-null pointer to the string to validate.
 * @param MaximumLength The non-zero maximum number of characters readable from
 *                      String, which is the accessible buffer length in
 *                      characters.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the string is invalid which is not null-terminated within the
 *          maximum length, the function returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringValidate(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN MaximumLength);

/**
 * @brief Validate a null-terminated wide string within a maximum length, and
 *        optionally retrieve the length of the wide string.
 * @param Length The pointer to receive the length of the wide string without
 *               the null terminator. This parameter is optional if the caller
 *               only wants to validate the wide string. If the wide string is
 *               invalid, the output length will be set to zero.
 * @param WideString The non-null pointer to the wide string to validate.
 * @param MaximumLength The non-zero maximum number of wide characters readable
 *                      from WideString, which is the accessible buffer length
 *                      in wide characters.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the wide string is invalid which is not null-terminated within
 *          the maximum length, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringValidate(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN MaximumLength);

/**
 * @brief Copies a string from source to destination with specified lengths.
 * @param Destination The non-null destination string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source string to copy from.
 * @param SourceLength The number of characters to copy from the source,
 *                     excluding the null terminator.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the number of characters in the source string plus the null
 *          terminator exceeds the maximum number of characters of the
 *          destination buffer, the function returns
 *          MO_RESULT_ERROR_OUT_OF_MEMORY. Overlapping source and destination
 *          is not allowed and results in MO_RESULT_ERROR_INVALID_PARAMETER.
 *          If the address ranges of source or destination overflow, the
 *          function returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringCopy(
    _Out_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source,
    _In_ MO_UINTN SourceLength);

/**
 * @brief Copies a wide string from source to destination with specified
 *        lengths.
 * @param Destination The non-null destination wide string buffer.
 * @param MaximumLength The non-zero maximum number of wide characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source wide string to copy from.
 * @param SourceLength The number of wide characters to copy from the source,
 *                     excluding the null terminator.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the number of characters in the source wide string plus the null
 *          terminator exceeds the maximum number of wide characters of the
 *          destination buffer, the function returns
 *          MO_RESULT_ERROR_OUT_OF_MEMORY. Overlapping source and destination
 *          is not allowed and results in MO_RESULT_ERROR_INVALID_PARAMETER.
 *          If the address ranges of source or destination overflow, the
 *          function returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringCopy(
    _Out_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source,
    _In_ MO_UINTN SourceLength);

/**
 * @brief Concatenate source string to the end of destination string.
 * @param Destination The non-null destination string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source string to concatenate.
 * @param SourceLength The number of characters to concatenate from source,
 *                     excluding the null terminator.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the destination string is not a valid null-terminated string
 *          within the maximum length, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS. If the source string is not a valid
 *          null-terminated string which the number of characters in the source
 *          string without the null terminator is not equal, the function
 *          returns MO_RESULT_ERROR_OUT_OF_BOUNDS. If the number of characters
 *          in the destination string plus the number of characters to
 *          concatenate from source string plus the null terminator exceeds the
 *          maximum number of characters of the destination buffer, the function
 *          returns MO_RESULT_ERROR_OUT_OF_MEMORY. Overlapping source and
 *          destination is not allowed and results in
 *          MO_RESULT_ERROR_INVALID_PARAMETER. If the address ranges of source
 *          or destination overflow, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringConcatenate(
    _Inout_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source,
    _In_ MO_UINTN SourceLength);

/**
 * @brief Concatenate source wide string to the end of destination wide string.
 * @param Destination The non-null destination wide string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source wide string to concatenate.
 * @param SourceLength The number of characters to concatenate from source,
 *                     excluding the null terminator.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the destination wide string is not a valid null-terminated wide
 *          string within the maximum length, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS. If the source wide string is not a
 *          valid null-terminated wide string which the number of characters in
 *          the source wide string without the null terminator is not equal, the
 *          function returns MO_RESULT_ERROR_OUT_OF_BOUNDS. If the number of
 *          characters in the destination wide string plus the number of
 *          characters to concatenate from source wide string plus the null
 *          terminator exceeds the maximum number of characters of the
 *          destination buffer, the function returns
 *          MO_RESULT_ERROR_OUT_OF_MEMORY. Overlapping source and destination is
 *          not allowed and results in MO_RESULT_ERROR_INVALID_PARAMETER. If the
 *          address ranges of source or destination overflow, the function
 *          returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringConcatenate(
    _Inout_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source,
    _In_ MO_UINTN SourceLength);

/**
 * @brief Find the first occurrence of a character in a string.
 * @param Index The non-null pointer to receive the index of the first
 *              occurrence of the character.
 * @param String The non-null string to search.
 * @param Length The number of characters of the string to search, excluding the
 *               null terminator.
 * @param Character The character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the character is not found in the string, the function returns
 *          MO_RESULT_SUCCESS_FALSE, and Index is set to MO_UINTN_MAX. If the
 *          string is not valid with the specified length, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindFirstCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN Length,
    _In_ MO_CHAR Character);

/**
 * @brief Find the first occurrence of a wide character in a wide string.
 * @param Index The non-null pointer to receive the index of the first
 *              occurrence of the wide character.
 * @param WideString The non-null wide string to search.
 * @param Length The number of wide characters of the wide string to search,
 *               excluding the null terminator.
 * @param WideCharacter The wide character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the wide character is not found in the wide string, the function
 *          returns MO_RESULT_SUCCESS_FALSE, and Index is set to MO_UINTN_MAX.
 *          If the wide string is not valid with the specified length, the
 *          function returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindFirstCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN Length,
    _In_ MO_WIDE_CHAR WideCharacter);

/**
 * @brief Find the last occurrence of a character in a string.
 * @param Index The non-null pointer to receive the index of the last
 *              occurrence of the character.
 * @param String The non-null string to search.
 * @param Length The number of characters of the string to search, excluding the
 *               null terminator.
 * @param Character The character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the character is not found in the string, the function returns
 *          MO_RESULT_SUCCESS_FALSE, and Index is set to MO_UINTN_MAX. If the
 *          string is not valid with the specified length, the function returns
 *          MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindLastCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_UINTN Length,
    _In_ MO_CHAR Character);

/**
 * @brief Find the last occurrence of a wide character in a wide string.
 * @param Index The non-null pointer to receive the index of the last
 *              occurrence of the wide character.
 * @param WideString The non-null wide string to search.
 * @param Length The number of wide characters of the wide string to search,
 *               excluding the null terminator.
 * @param WideCharacter The wide character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the wide character is not found in the wide string, the function
 *          returns MO_RESULT_SUCCESS_FALSE, and Index is set to MO_UINTN_MAX.
 *          If the wide string is not valid with the specified length, the
 *          function returns MO_RESULT_ERROR_OUT_OF_BOUNDS.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindLastCharacter(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_UINTN Length,
    _In_ MO_WIDE_CHAR WideCharacter);

/**
 * @brief Validate a null-terminated string within a maximum length, and
 *        optionally retrieve the length of the string.
 * @param Length The pointer to receive the length of the string without the
 *               null terminator. This parameter is optional if the caller only
 *               wants to validate the string. If the string is invalid, the
 *               output length will be set to zero.
 * @param String The non-null pointer to the string to validate.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeStringValidate.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringValidateSimple(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_STRING String);

/**
 * @brief Validate a null-terminated wide string within a maximum length, and
 *        optionally retrieve the length of the wide string.
 * @param Length The pointer to receive the length of the wide string without
 *               the null terminator. This parameter is optional if the caller
 *               only wants to validate the wide string. If the wide string is
 *               invalid, the output length will be set to zero.
 * @param WideString The non-null pointer to the wide string to validate.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeWideStringValidate.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringValidateSimple(
    _Out_opt_ PMO_UINTN Length,
    _In_ MO_CONSTANT_WIDE_STRING WideString);

/**
 * @brief Copies a string from source to destination.
 * @param Destination The non-null destination string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source string to copy from.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeStringCopy.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringCopySimple(
    _Out_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source);

/**
 * @brief Copies a wide string from source to destination.
 * @param Destination The non-null destination wide string buffer.
 * @param MaximumLength The non-zero maximum number of wide characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source wide string to copy from.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeWideStringCopy.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringCopySimple(
    _Out_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source);

/**
 * @brief Concatenate source string to the end of destination string.
 * @param Destination The non-null destination string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source string to concatenate.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeStringConcatenate.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringConcatenateSimple(
    _Inout_ MO_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_STRING Source);

/**
 * @brief Concatenate source wide string to the end of destination wide string.
 * @param Destination The non-null destination wide string buffer.
 * @param MaximumLength The non-zero maximum number of characters of the
 *                      destination buffer, including the null terminator.
 * @param Source The non-null source wide string to concatenate.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeWideStringConcatenate.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringConcatenateSimple(
    _Inout_ MO_WIDE_STRING Destination,
    _In_ MO_UINTN MaximumLength,
    _In_ MO_CONSTANT_WIDE_STRING Source);

/**
 * @brief Find the first occurrence of a character in a string.
 * @param Index The non-null pointer to receive the index of the first
 *              occurrence of the character.
 * @param String The non-null string to search.
 * @param Character The character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeStringFindFirstCharacter.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindFirstCharacterSimple(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_CHAR Character);

/**
 * @brief Find the first occurrence of a wide character in a wide string.
 * @param Index The non-null pointer to receive the index of the first
 *              occurrence of the wide character.
 * @param WideString The non-null wide string to search.
 * @param WideCharacter The wide character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of
 *          MoRuntimeWideStringFindFirstCharacter.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindFirstCharacterSimple(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_WIDE_CHAR WideCharacter);

/**
 * @brief Find the last occurrence of a character in a string.
 * @param Index The non-null pointer to receive the index of the last
 *              occurrence of the character.
 * @param String The non-null string to search.
 * @param Character The character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of MoRuntimeStringFindLastCharacter.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeStringFindLastCharacterSimple(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_CHAR Character);

/**
 * @brief Find the last occurrence of a wide character in a wide string.
 * @param Index The non-null pointer to receive the index of the last
 *              occurrence of the wide character.
 * @param WideString The non-null wide string to search.
 * @param WideCharacter The wide character to find.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks This is the simplified version of
 *          MoRuntimeWideStringFindLastCharacter.
 */
EXTERN_C MO_RESULT MOAPI MoRuntimeWideStringFindLastCharacterSimple(
    _Out_ PMO_UINTN Index,
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_WIDE_CHAR WideCharacter);

/**
 * @brief Get the length of a null-terminated string.
 * @param String The input null-terminated string.
 * @return The length of the string excluding the null terminator, or 0 if the
 *         string is empty or invalid.
 * @remarks This is the easy version of MoRuntimeStringValidateSimple.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeStringLength(
    _In_ MO_CONSTANT_STRING String);

/**
 * @brief Get the length of a null-terminated wide string.
 * @param WideString The input null-terminated wide string.
 * @return The wide length of the string excluding the null terminator, or 0 if
 *         the string is empty or invalid.
 * @remarks This is the easy version of MoRuntimeWideStringValidateSimple.
 */
EXTERN_C MO_UINTN MOAPI MoRuntimeWideStringLength(
    _In_ MO_CONSTANT_WIDE_STRING WideString);

/**
 * @brief Find the first occurrence of a character in a string.
 * @param String The non-null string to search.
 * @param Character The character to find.
 * @return The substring starting from the first occurrence of the character, or
 *         nullptr if the character is not found or an error occurs.
 * @remarks This is the easy version of MoRuntimeStringFindFirstCharacterSimple.
 */
EXTERN_C MO_STRING MOAPI MoRuntimeStringFindFirstCharacterEasy(
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_CHAR Character);

/**
 * @brief Find the first occurrence of a wide character in a wide string.
 * @param WideString The non-null wide string to search.
 * @param WideCharacter The wide character to find.
 * @return The wide substring starting from the first occurrence of the
 *         character, or nullptr if the wide character is not found or an error
 *         occurs.
 * @remarks This is the easy version of
 *          MoRuntimeWideStringFindFirstCharacterSimple.
 */
EXTERN_C MO_WIDE_STRING MOAPI MoRuntimeWideStringFindFirstCharacterEasy(
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_WIDE_CHAR WideCharacter);

/**
 * @brief Find the last occurrence of a character in a string.
 * @param String The non-null string to search.
 * @param Character The character to find.
 * @return The substring starting from the last occurrence of the character, or
 *         nullptr if the character is not found or an error occurs.
 * @remarks This is the easy version of MoRuntimeStringFindLastCharacterSimple.
 */
EXTERN_C MO_STRING MOAPI MoRuntimeStringFindLastCharacterEasy(
    _In_ MO_CONSTANT_STRING String,
    _In_ MO_CHAR Character);

/**
 * @brief Find the last occurrence of a wide character in a wide string.
 * @param WideString The non-null wide string to search.
 * @param WideCharacter The wide character to find.
 * @return The wide substring starting from the last occurrence of the
 *         character, or nullptr if the wide character is not found or an error
 *         occurs.
 * @remarks This is the easy version of
 *          MoRuntimeWideStringFindLastCharacterSimple.
 */
EXTERN_C MO_WIDE_STRING MOAPI MoRuntimeWideStringFindLastCharacterEasy(
    _In_ MO_CONSTANT_WIDE_STRING WideString,
    _In_ MO_WIDE_CHAR WideCharacter);

#endif // !MOBILITY_RUNTIME_CORE
