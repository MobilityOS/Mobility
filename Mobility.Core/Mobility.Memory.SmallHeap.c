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

#include <Mile.Mobility.Utilities.FixedInteger.h>

MO_FORCEINLINE MO_BOOL MoMemorySmallHeapInstanceValidate(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    MO_UINTN InstanceStart = (MO_UINTN)Instance;
    if (!InstanceStart ||
        (InstanceStart & (MO_MEMORY_SMALL_HEAP_UNIT_SIZE - 1u)))
    {
        return MO_FALSE;
    }

    MO_UINTN InstanceLastByte = 0u;
    return MoMileFixedIntegerCheckedAddition(
        &InstanceLastByte,
        MO_FALSE,
        InstanceStart,
        MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE - 1u);
}

MO_EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapInitialize(
    _Mo_Out_ PMO_MEMORY_SMALL_HEAP Instance)
{
    if (!MoMemorySmallHeapInstanceValidate(Instance))
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
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
        Instance->UserArea,
        MO_MEMORY_SMALL_HEAP_USER_AREA_INITIAL_BYTE,
        MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
        Instance->Bitmap,
        0u,
        Instance->Header.AllocatedUnits,
        MO_TRUE))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    return MO_RESULT_SUCCESS_OK;
}

MO_FORCEINLINE MO_BOOL MoMemorySmallHeapHeaderValidate(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    if (!MoMemorySmallHeapInstanceValidate(Instance))
    {
        return MO_FALSE;
    }

    PMO_MEMORY_SMALL_HEAP_HEADER Header = &Instance->Header;

    if (MO_MEMORY_SMALL_HEAP_SIGNATURE != Header->Signature)
    {
        return MO_FALSE;
    }

    if (MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS > Header->AllocatedUnits ||
        MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS < Header->AllocatedUnits)
    {
        return MO_FALSE;
    }

    if (MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS > Header->HintUnit ||
        MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS < Header->HintUnit)
    {
        return MO_FALSE;
    }

    return MO_TRUE;
}

MO_EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapSummary(
    _Mo_Out_ PMO_MEMORY_SMALL_HEAP_SUMMARY Summary,
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance)
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
            // This function should not fail here.
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
    _Mo_In_ PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader)
{
    MO_UINT32 Checksum = 0;
    Checksum += ItemHeader->HeapHeaderOffsetUnits;
    Checksum += ItemHeader->AllocatedUnits;
    Checksum += ItemHeader->RequestedSize;
    return (MO_UINT16)(~Checksum);
}

MO_FORCEINLINE MO_UINT16 MoMemorySmallHeapFindSuitableBlock(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance,
    _Mo_In_ MO_UINT16 RequiredUnits,
    _Mo_In_ MO_UINT16 StartIndex)
{
    MO_UINT16 CurrentIndex = StartIndex;
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
            // Unexpected error.
            return 0;
        }
        if (!RunUnits)
        {
            // No more blocks available.
            return 0;
        }
        if (!BitValue && RunUnits >= RequiredUnits)
        {
            // Suitable block found.
            return CurrentIndex;
        }
        CurrentIndex += (MO_UINT16)RunUnits;
    }
    // No suitable block found.
    return 0;
}

MO_FORCEINLINE MO_VOID MoMemorySmallHeapUpdateHintUnit(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance)
{
    MO_UINT16 FirstSuitableBlockIndex = MoMemorySmallHeapFindSuitableBlock(
        Instance,
        MO_MEMORY_SMALL_HEAP_USER_AREA_MINIMUM_ALLOCATION_UNITS,
        MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS);
    if (FirstSuitableBlockIndex)
    {
        Instance->Header.HintUnit = FirstSuitableBlockIndex;
    }
}

MO_EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapAllocate(
    _Mo_Out_ PMO_POINTER Block,
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance,
    _Mo_In_ MO_UINT16 Size)
{
    if (!Block || !Instance || Size == 0)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (Size > MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE
        - MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE)
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

    MO_UINT16 SuitableBlockIndex = MoMemorySmallHeapFindSuitableBlock(
        Instance,
        RequiredUnits,
        Instance->Header.HintUnit);
    if (SuitableBlockIndex)
    {
        // Mark the units as allocated if found.
        if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
            Instance->Bitmap,
            SuitableBlockIndex,
            RequiredUnits,
            MO_TRUE))
        {
            // This function should not fail here.
            return MO_RESULT_ERROR_UNEXPECTED;
        }

        MO_UINT16 ItemHeaderOffset = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
            SuitableBlockIndex);

        MO_UINTN ItemHeaderStart = InstanceStart + ItemHeaderOffset;

        // Clear the allocated area.
        if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
            (MO_POINTER)ItemHeaderStart,
            MO_MEMORY_SMALL_HEAP_USER_AREA_ALLOCATED_BYTE,
            RequiredSize))
        {
            // This function should not fail here.
            return MO_RESULT_ERROR_UNEXPECTED;
        }

        // Create the item header.

        PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader =
            (PMO_MEMORY_SMALL_HEAP_ITEM_HEADER)(ItemHeaderStart);
        ItemHeader->HeapHeaderOffsetUnits = SuitableBlockIndex;
        ItemHeader->AllocatedUnits = RequiredUnits;
        ItemHeader->RequestedSize = Size;
        ItemHeader->Checksum =
            MoMemorySmallHeapCalculateItemHeaderChecksum(ItemHeader);

        // Update the heap header.

        Instance->Header.AllocatedUnits += RequiredUnits;
        MoMemorySmallHeapUpdateHintUnit(Instance);

        // Return the pointer to the user area.
        *Block = (MO_POINTER)(
            ItemHeaderStart + MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE);

        return MO_RESULT_SUCCESS_OK;
    }

    // No suitable block found.
    return MO_RESULT_ERROR_OUT_OF_MEMORY;
}

MO_FORCEINLINE PMO_MEMORY_SMALL_HEAP_ITEM_HEADER MoMemorySmallHeapGetItemHeader(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance,
    _Mo_In_ MO_POINTER Block)
{
    MO_UINTN HeaderAddress = 0u;
    if (!MoMileFixedIntegerCheckedSubtraction(
        &HeaderAddress,
        MO_FALSE,
        (MO_UINTN)Block,
        MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE))
    {
        return nullptr;
    }

    MO_UINTN ByteOffset = 0u;
    if (!MoMileFixedIntegerCheckedSubtraction(
        &ByteOffset,
        MO_FALSE,
        HeaderAddress,
        (MO_UINTN)Instance))
    {
        return nullptr;
    }

    if ((ByteOffset & (MO_MEMORY_SMALL_HEAP_UNIT_SIZE - 1u)) ||
        ByteOffset < MO_MEMORY_SMALL_HEAP_SERVICE_AREA_SIZE ||
        ByteOffset > (MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE -
            MO_MEMORY_SMALL_HEAP_USER_AREA_MINIMUM_ALLOCATION_SIZE))
    {
        return nullptr;
    }

    PMO_MEMORY_SMALL_HEAP_ITEM_HEADER Header =
        (PMO_MEMORY_SMALL_HEAP_ITEM_HEADER)HeaderAddress;

    MO_UINT16 Checksum = MoMemorySmallHeapCalculateItemHeaderChecksum(Header);
    if (Header->Checksum != Checksum)
    {
        return nullptr;
    }

    MO_UINTN OffsetUnits = MO_MEMORY_SMALL_HEAP_SIZE_TO_UNITS(ByteOffset);
    if (Header->HeapHeaderOffsetUnits != OffsetUnits)
    {
        return nullptr;
    }

    if (Header->AllocatedUnits <
        MO_MEMORY_SMALL_HEAP_USER_AREA_MINIMUM_ALLOCATION_UNITS ||
        Header->AllocatedUnits > MO_MEMORY_SMALL_HEAP_USER_AREA_UNITS)
    {
        return nullptr;
    }

    MO_UINT16 EndUnits = 0u;
    if (!MoMileFixedIntegerCheckedAddition16(
        &EndUnits,
        MO_FALSE,
        Header->HeapHeaderOffsetUnits,
        Header->AllocatedUnits) ||
        EndUnits > MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS)
    {
        return nullptr;
    }

    MO_UINT16 MaximumUserSize =
        MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(Header->AllocatedUnits) -
        MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE;
    if (!Header->RequestedSize || Header->RequestedSize > MaximumUserSize)
    {
        return nullptr;
    }

    return Header;
}

MO_EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapFree(
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance,
    _Mo_In_ MO_POINTER Block)
{
    if (!Instance || !Block)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!MoMemorySmallHeapHeaderValidate(Instance))
    {
        return MO_RESULT_ERROR_INVALID_HANDLE;
    }

    PMO_MEMORY_SMALL_HEAP_ITEM_HEADER ItemHeader =
        MoMemorySmallHeapGetItemHeader(Instance, Block);
    if (!ItemHeader)
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
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    // Clear the item header and user area.
    if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
        ItemHeader,
        MO_MEMORY_SMALL_HEAP_USER_AREA_FREED_BYTE,
        MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE((MO_UINTN)(AllocatedUnits))))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    // Update the heap header.

    Instance->Header.AllocatedUnits -= AllocatedUnits;
    MoMemorySmallHeapUpdateHintUnit(Instance);

    return MO_RESULT_SUCCESS_OK;
}

MO_EXTERN_C MO_RESULT MOAPI MoMemorySmallHeapReallocate(
    _Mo_Out_ PMO_POINTER UpdatedBlock,
    _Mo_In_ PMO_MEMORY_SMALL_HEAP Instance,
    _Mo_In_Opt_ MO_POINTER Block,
    _Mo_In_ MO_UINT16 NewSize)
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

    PMO_MEMORY_SMALL_HEAP_ITEM_HEADER OriginalItemHeader =
        MoMemorySmallHeapGetItemHeader(Instance, Block);
    if (!OriginalItemHeader)
    {
        return MO_RESULT_ERROR_INVALID_POINTER;
    }

    if (NewSize > MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE
        - MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE)
    {
        // Exceeds the maximum allocatable size.
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    MO_UINT16 OriginalAllocatedSize = MO_MEMORY_SMALL_HEAP_UNITS_TO_SIZE(
        OriginalItemHeader->AllocatedUnits);
    MO_UINT16 OriginalRequestedSize = OriginalItemHeader->RequestedSize;
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

    MO_UINT16 NextBlockUnitIndex =
        OriginalItemHeader->HeapHeaderOffsetUnits +
        OriginalItemHeader->AllocatedUnits;
    if (NextBlockUnitIndex < MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS)
    {
        MO_UINT16 AdditionalRequiredSize = (MO_UINT16)MoRuntimeGetAlignedSize(
            NewSize - MaximumSizeWithoutReallocation,
            MO_MEMORY_SMALL_HEAP_UNIT_SIZE);
        MO_UINT16 AdditionalRequiredUnits =
            MO_MEMORY_SMALL_HEAP_SIZE_TO_UNITS(AdditionalRequiredSize);

        MO_UINTN RunUnits = 0u;
        MO_BOOL BitValue = MO_FALSE;
        if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapQueryContinuousRunLength(
            &RunUnits,
            &BitValue,
            Instance->Bitmap,
            NextBlockUnitIndex,
            MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS))
        {
            // This function should not fail here.
            return MO_RESULT_ERROR_UNEXPECTED;
        }

        if (!BitValue && RunUnits >= AdditionalRequiredUnits)
        {
            // Expand in place if the next block is free and sufficient.
            if (MO_RESULT_SUCCESS_OK != MoRuntimeBitmapFillRange(
                Instance->Bitmap,
                NextBlockUnitIndex,
                AdditionalRequiredUnits,
                MO_TRUE))
            {
                // This function should not fail here.
                return MO_RESULT_ERROR_UNEXPECTED;
            }

            // Clear the newly allocated area.
            if (MO_RESULT_SUCCESS_OK != MoRuntimeMemoryFillByte(
                (MO_POINTER)(
                    ((MO_UINTN)Block) + MaximumSizeWithoutReallocation),
                MO_MEMORY_SMALL_HEAP_USER_AREA_ALLOCATED_BYTE,
                AdditionalRequiredSize))
            {
                // This function should not fail here.
                return MO_RESULT_ERROR_UNEXPECTED;
            }

            // Update the item header.

            OriginalItemHeader->AllocatedUnits += AdditionalRequiredUnits;
            OriginalItemHeader->RequestedSize = NewSize;
            OriginalItemHeader->Checksum =
                MoMemorySmallHeapCalculateItemHeaderChecksum(
                    OriginalItemHeader);

            // Update the heap header.

            Instance->Header.AllocatedUnits += AdditionalRequiredUnits;
            MoMemorySmallHeapUpdateHintUnit(Instance);

            *UpdatedBlock = Block;
            return MO_RESULT_SUCCESS_OK;
        }
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
        // This function should not fail here.
        // Don't free the newly allocated empty block to help the analysis.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    // Return the new block after the content is copied.
    *UpdatedBlock = NewBlock;

    Result = MoMemorySmallHeapFree(Instance, Block);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        // This function should not fail here.
        // Don't free the newly allocated block to avoid data loss.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    return MO_RESULT_SUCCESS_OK;
}
