/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.InternalHeap.h
 * PURPOSE:    Definition for Mobility Memory Internal Heap
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_MEMORY_INTERNALHEAP
#define MOBILITY_MEMORY_INTERNALHEAP

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

/**
 * @brief Initializes the Internal Heap instance.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapInitialize();

/**
 * @brief Allocates a memory block from the Internal Heap instance.
 * @param Block Receives the pointer to the allocated memory block.
 * @param Size The size in bytes to be allocated.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapAllocate(
    _Out_ PMO_POINTER Block,
    _In_ MO_UINT16 Size);

/**
 * @brief Frees a memory block back to the Internal Heap instance.
 * @param Block The pointer to the memory block to be freed.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapFree(
    _In_ MO_POINTER Block);

/**
 * @brief Reallocates a memory block in the Internal Heap instance.
 * @param UpdatedBlock Receives the pointer to the reallocated memory block.
 * @param Block The pointer to the memory block to be reallocated. If this
 *              parameter is nullptr, a new block will be allocated.
 * @param NewSize The new size in bytes for the memory block.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 */
EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapReallocate(
    _Out_ PMO_POINTER UpdatedBlock,
    _In_opt_ MO_POINTER Block,
    _In_ MO_UINT16 NewSize);

#endif // !MOBILITY_MEMORY_INTERNALHEAP
