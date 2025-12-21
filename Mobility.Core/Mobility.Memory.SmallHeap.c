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

    if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
        Instance->Bitmap,
        0u,
        Instance->Header.AllocatedUnits,
        MO_TRUE))
    {
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    return MO_RESULT_SUCCESS_OK;
}

MO_FORCEINLINE MO_BOOL MoMemorySmallHeapHeaderValidate(
    _In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    PMO_MEMORY_SMALL_HEAP_HEADER Header = &Instance->Header;

    if (MO_MEMORY_SMALL_HEAP_SIGNATURE != Header->Signature)
    {
        return MO_FALSE;
    }

    if (MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS > Header->AllocatedUnits)
    {
        return MO_FALSE;
    }

    if (MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS > Header->HintUnit)
    {
        return MO_FALSE;
    }

    return MO_TRUE;
}

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapSummary(
    _Out_ PMO_MEMORY_SMALL_HEAP_SUMMARY Summary,
    _In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    if (!Summary || !Instance)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!MoMemorySmallHeapHeaderValidate(Instance))
    {
        return MO_RESULT_ERROR_INVALID_HANDLE;
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
        if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapQueryContinuousRunLength(
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

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapAllocate(
    _Out_ PMO_POINTER Block,
    _In_ PMO_MEMORY_SMALL_HEAP Instance,
    _In_ MO_UINT16 Size)
{
    if (!Block || !Instance || Size == 0)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE - MO_MEMORY_SMALL_HEAP_HEADER_SIZE
        < Size)
    {
        // Exceeds the maximum allocatable size.
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    if (!MoMemorySmallHeapHeaderValidate(Instance))
    {
        return MO_RESULT_ERROR_INVALID_HANDLE;
    }
    MO_UINTN InstanceStart = (MO_UINTN)Instance;

    MO_UINT16 RequiredSize = (MO_UINT16)MoRuntimeGetAlignedSize(
        MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE + Size,
        MO_MEMORY_SMALL_HEAP_UNIT_SIZE);
    MO_UINT16 RequiredUnits = MO_MEMORY_SMALL_HEAP_SIZE_TO_UNITS(RequiredSize);

    MO_UINT16 CurrentIndex = Instance->Header.HintUnit;
    while (CurrentIndex < MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS)
    {
        MO_UINTN RunUnits = 0u;
        MO_BOOL BitValue = MO_FALSE;
        if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapQueryContinuousRunLength(
            &RunUnits,
            &BitValue,
            Instance->Bitmap,
            CurrentIndex,
            MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS))
        {
            return MO_RESULT_ERROR_UNEXPECTED;
        }
        if (!BitValue && RunUnits >= RequiredUnits)
        {
            // Mark the units as allocated if found.
            if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
                Instance->Bitmap,
                CurrentIndex,
                RequiredUnits,
                MO_TRUE))
            {
                return MO_RESULT_ERROR_UNEXPECTED;
            }

            MO_UINT16 ItemHeaderOffset = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
                CurrentIndex);

            MO_UINTN ItemHeaderStart = InstanceStart + ItemHeaderOffset;

            // Create the item header.

            PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader =
                (PMO_MEMORY_SMALL_HEAP_ITEM_HEADER)(ItemHeaderStart);
            ItemHeader->HeapHeaderOffsetUnits = CurrentIndex;
            ItemHeader->AllocatedUnits = RequiredUnits;
            ItemHeader->RequestedSize = Size;
            ItemHeader->Checksum =
                MoMemorySmallHeapCalculateItemHeaderChecksum(ItemHeader);

            // Update the heap header.

            Instance->Header.AllocatedUnits += RequiredUnits;
            MO_UINT16 CandidateHintUnit =
                (MO_UINT16)(CurrentIndex + RequiredUnits);
            if (CandidateHintUnit >= MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS &&
                CandidateHintUnit < Instance->Header.HintUnit)
            {
                // Only update the hint if it's smaller.
                Instance->Header.HintUnit = CandidateHintUnit;
            }

            // Return the pointer to the user area.
            *Block = (MO_POINTER)(
                ItemHeaderStart + MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE);

            return MO_RESULT_SUCCESS_OK;
        }
        CurrentIndex += (MO_UINT16)RunUnits;
    }

    // No suitable block found.
    return MO_RESULT_ERROR_OUT_OF_MEMORY;
}

MO_FORCEINLINE MO_BOOL MoMemorySmallHeapItemHeaderValidate(
    _In_ PMO_MEMORY_SMALL_HEAP Instance,
    _In_ PMO_MEMORY_SMALL_HEAP_ITEM_HEADER Header)
{
    MO_UINT16 Checksum = MoMemorySmallHeapCalculateItemHeaderChecksum(Header);
    if (Header->Checksum != Checksum)
    {
        return MO_FALSE;
    }

    if (Header->HeapHeaderOffsetUnits < MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS)
    {
        return MO_FALSE;
    }

    MO_UINTN OffsetUnits = MO_MEMORY_SMALL_HEAP_SIZE_TO_UNITS(
        ((MO_UINTN)Header) - ((MO_UINTN)Instance));
    if (Header->HeapHeaderOffsetUnits != OffsetUnits)
    {
        return MO_FALSE;
    }
    return MO_TRUE;
}

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapFree(
    _In_ PMO_MEMORY_SMALL_HEAP Instance,
    _In_ MO_POINTER Block)
{
    if (!Instance || !Block)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!MoMemorySmallHeapHeaderValidate(Instance))
    {
        return MO_RESULT_ERROR_INVALID_HANDLE;
    }

    // Get the item header from the block.
    PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader =
        (PMO_MEMORY_SMALL_HEAP_ITEM_HEADER)(
            (MO_UINTN)Block - MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE);

    if (!MoMemorySmallHeapItemHeaderValidate(
        Instance,
        ItemHeader))
    {
        return MO_RESULT_ERROR_INVALID_POINTER;
    }

    MO_UINT16 AllocatedUnits = ItemHeader->AllocatedUnits;
    MO_UINT16 HeapHeaderOffsetUnits = ItemHeader->HeapHeaderOffsetUnits;

    // Mark the units as free.
    if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
        Instance->Bitmap,
        HeapHeaderOffsetUnits,
        AllocatedUnits,
        MO_FALSE))
    {
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    // Update the heap header.
    Instance->Header.AllocatedUnits -= AllocatedUnits;
    if (HeapHeaderOffsetUnits >= MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS &&
        HeapHeaderOffsetUnits < Instance->Header.HintUnit)
    {
        // Only update the hint if it's smaller.
        Instance->Header.HintUnit = HeapHeaderOffsetUnits;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapReallocate(
    _Out_ PMO_POINTER UpdatedBlock,
    _In_ PMO_MEMORY_SMALL_HEAP Instance,
    _In_opt_ MO_POINTER Block,
    _In_ MO_UINT16 NewSize)
{
    if (!UpdatedBlock || !Instance || !NewSize)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!MoMemorySmallHeapHeaderValidate(Instance))
    {
        return MO_RESULT_ERROR_INVALID_HANDLE;
    }

    if (!Block)
    {
        // Allocate new block if the original block is nullptr.
        return MoMemorySmallHeapAllocate(
            UpdatedBlock,
            Instance,
            NewSize);
    }

    // Get the item header from the original block.
    PMO_MEMORY_SMALL_HEAP_ITEM_HEADER OriginalItemHeader =
        (PMO_MEMORY_SMALL_HEAP_ITEM_HEADER)(
            (MO_UINTN)Block - MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE);

    if (!MoMemorySmallHeapItemHeaderValidate(
        Instance,
        OriginalItemHeader))
    {
        return MO_RESULT_ERROR_INVALID_POINTER;
    }

    MO_UINT16 OriginalAllocatedSize = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
        OriginalItemHeader->AllocatedUnits);
    MO_UINT16 OriginalRequestedSize = OriginalItemHeader->RequestedSize;

    if (NewSize <= OriginalRequestedSize)
    {
        // If the new size is less than or equal to the original size,
        // return the same block.
        *UpdatedBlock = Block;
        return MO_RESULT_SUCCESS_OK;
    }

    MO_UINT16 MaximumSizeWithoutReallocation =
        OriginalAllocatedSize - MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE;
    if (NewSize <= MaximumSizeWithoutReallocation)
    {
        // If the new size can fit in the original allocated size,
        // update the requested size in the item header.
        OriginalItemHeader->RequestedSize = NewSize;
        OriginalItemHeader->Checksum =
            MoMemorySmallHeapCalculateItemHeaderChecksum(
                OriginalItemHeader);
        *UpdatedBlock = Block;
        return MO_RESULT_SUCCESS_OK;
    }

    // Allocate a new block.
    MO_POINTER NewBlock = nullptr;
    MO_RESULT Result = MoMemorySmallHeapAllocate(&NewBlock, Instance, NewSize);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    Result = MoRuntimeMemoryMove(NewBlock, Block, OriginalRequestedSize);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    Result = MoMemorySmallHeapFree(Instance, Block);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    *UpdatedBlock = NewBlock;

    return MO_RESULT_SUCCESS_OK;
}
