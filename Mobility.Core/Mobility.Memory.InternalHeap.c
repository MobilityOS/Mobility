/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.InternalHeap.c
 * PURPOSE:    Implementation for Mobility Memory Internal Heap
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Memory.InternalHeap.h"

#include "Mobility.Memory.SmallHeap.h"

MO_MEMORY_SMALL_HEAP g_InternalHeap;

EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapInitialize()
{
    return MoMemorySmallHeapInitialize(&g_InternalHeap);
}

EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapAllocate(
    _Out_ PMO_POINTER Block,
    _In_ MO_UINT16 Size)
{
    return MoMemorySmallHeapAllocate(Block, &g_InternalHeap, Size);
}

EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapFree(
    _In_ MO_POINTER Block)
{
    return MoMemorySmallHeapFree(&g_InternalHeap, Block);
}

EXTERN_C MO_RESULT MOAPI MoMemoryInternalHeapReallocate(
    _Out_ PMO_POINTER UpdatedBlock,
    _In_opt_ MO_POINTER Block,
    _In_ MO_UINT16 NewSize)
{
    return MoMemorySmallHeapReallocate(
        UpdatedBlock,
        &g_InternalHeap,
        Block,
        NewSize);
}
