/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.SmallHeap.c
 * PURPOSE:    Implementation for Mobility Memory Small Heap (v1)
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Memory.SmallHeap.h"

#include "Mobility.Runtime.Core.h"
#include "Mobility.Memory.Bitmap.h"

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapInitialize(
    _Out_ PMO_MEMORY_SMALL_HEAP Instance)
{
    if (!Instance)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    Instance->Header.Signature = MO_MEMORY_SMALL_HEAP_SIGNATURE;
    Instance->Header.AllocatedUnits =
        MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS;
    Instance->Header.HintUnit =
        MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS;

    if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
        Instance->Bitmap,
        0u,
        MO_MEMORY_SMALL_HEAP_BITMAP_SIZE))
    {
        return MO_RESULT_ERROR_UNEXPECTED;
    }
    
    if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
        Instance->UserArea,
        0u,
        MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE))
    {
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    if (MO_RESULT_SUCCESS_OK != MoMemoryBitmapFillRange(
        Instance->Bitmap,
        0u,
        Instance->Header.AllocatedUnits,
        MO_TRUE))
    {
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapSummary(
    _Out_ PMO_MEMORY_SMALL_HEAP_SUMMARY Summary,
    _In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    if (!Summary || !Instance)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    Summary->AllocatedSize = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
        Instance->Header.AllocatedUnits);

    Summary->FreeSize = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
        MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS - Instance->Header.AllocatedUnits);

    Summary->LargestFreeBlockSize = 0u;
    MO_UINT16 LargestFreeBlockUnits = 0u;
    MO_UINTN CurrentIndex = Instance->Header.HintUnit;
    while (CurrentIndex < MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS)
    {
        MO_UINTN RunUnits = 0u;
        MO_BOOL BitValue = MO_FALSE;
        if (MO_RESULT_SUCCESS_OK != MoMemoryBitmapQueryContinuousRunLength(
            &RunUnits,
            &BitValue,
            Instance->Bitmap,
            CurrentIndex,
            MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS))
        {
            return MO_RESULT_ERROR_UNEXPECTED;
        }
        if (!BitValue)
        {
            // Free block found.
            if (RunUnits > LargestFreeBlockUnits)
            {
                LargestFreeBlockUnits = (MO_UINT16)RunUnits;
            }
        }
        CurrentIndex += RunUnits;
    }
    Summary->LargestFreeBlockSize =
        MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(LargestFreeBlockUnits);

    return MO_RESULT_SUCCESS_OK;
}

MO_FORCEINLINE MO_UINT16 MoMemorySmallHeapCalculateItemHeaderChecksum(
    _In_ PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader)
{
    MO_UINT32 Checksum = 0;
    Checksum += ItemHeader->HeapHeaderOffsetUnits;
    Checksum += ItemHeader->AllocatedUnits;
    Checksum += ItemHeader->RequestedSize;
    return (MO_UINT16)(~Checksum);
}

// TODO: MoMemorySmallHeapAllocate
// TODO: MoMemorySmallHeapReallocate
// TODO: MoMemorySmallHeapFree
