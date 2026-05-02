/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Platform.Interface.h
 * PURPOSE:    Definition for Mobility Platform Abstraction Layer Interface
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_PLATFORM_INTERFACE
#define MOBILITY_PLATFORM_INTERFACE

#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Allocates a memory block from the Platform Heap instance.
 * @param Block Receives the pointer to the allocated memory block.
 * @param Size The size in bytes to be allocated.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
MO_EXTERN_C MO_RESULT MOAPI MoPlatformHeapAllocate(
    _Mo_Out_ PMO_POINTER Block,
    _Mo_In_ MO_UINTN Size);

/**
 * @brief Frees a memory block back to the Platform Heap instance.
 * @param Block The pointer to the memory block to be freed.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
MO_EXTERN_C MO_RESULT MOAPI MoPlatformHeapFree(
    _Mo_In_ MO_POINTER Block);

/**
 * @brief Reallocates a memory block in the Platform Heap instance.
 * @param UpdatedBlock Receives the pointer to the reallocated memory block.
 * @param Block The pointer to the memory block to be reallocated. If this
 *              parameter is nullptr, a new block will be allocated.
 * @param NewSize The new size in bytes for the memory block.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
MO_EXTERN_C MO_RESULT MOAPI MoPlatformHeapReallocate(
    _Mo_Out_ PMO_POINTER UpdatedBlock,
    _Mo_In_Opt_ MO_POINTER Block,
    _Mo_In_ MO_UINTN NewSize);

#endif // !MOBILITY_PLATFORM_INTERFACE
