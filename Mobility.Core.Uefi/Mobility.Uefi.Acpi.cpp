/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Uefi.Acpi.cpp
 * PURPOSE:    Implementation for Mobility UEFI ACPI functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Uefi.Acpi.h"

#include <Mobility.Runtime.Core.h>
#include <Mobility.Memory.InternalHeap.h>

#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi20.h>

EXTERN_C MO_BOOL MoUefiAcpiStructureValidate(
    _In_ MO_POINTER Structure,
    _In_ MO_UINTN Size)
{
    if (!Structure || !Size)
    {
        return MO_FALSE;
    }
    MO_UINT8 SumByte = 0;
    if (MO_RESULT_SUCCESS_OK != ::MoRuntimeCalculateSumByte(
        &SumByte,
        Structure,
        Size))
    {
        return MO_FALSE;
    }
    if (0 != SumByte)
    {
        return MO_FALSE;
    }
    return MO_TRUE;
}

EXTERN_C MO_BOOL MoUefiAcpiDescriptionTableValidate(
    _In_ MO_POINTER DescriptionTable,
    _In_ MO_UINT32 ExpectedSignature,
    _In_ MO_UINT8 MinimumRevision)
{
    if (!DescriptionTable)
    {
        return MO_FALSE;
    }
    EFI_ACPI_DESCRIPTION_HEADER* TableHeader =
        reinterpret_cast<EFI_ACPI_DESCRIPTION_HEADER*>(DescriptionTable);
    if (!::MoUefiAcpiStructureValidate(TableHeader, TableHeader->Length))
    {
        return MO_FALSE;
    }
    if (ExpectedSignature != TableHeader->Signature)
    {
        return MO_FALSE;
    }
    if (TableHeader->Revision < MinimumRevision)
    {
        return MO_FALSE;
    }
    return MO_TRUE;
}

EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryExtendedSystemDescriptionTable(
    _Out_ PMO_UINT64 TableAddress,
    _In_ EFI_SYSTEM_TABLE* SystemTable)
{
    if (!TableAddress || !SystemTable)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *TableAddress = 0u;

    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; ++i)
    {
        if (0 != ::MoRuntimeMemoryCompare(
            &SystemTable->ConfigurationTable[i].VendorGuid,
            &gEfiAcpiTableGuid,
            sizeof(EFI_GUID)))
        {
            continue;
        }

        using RootTableType = EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER;
        const MO_UINT64 RootTableSignature = 
            EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE;
        const MO_UINT8 RootTableRevision =
            EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION;
        RootTableType* RootTable = reinterpret_cast<RootTableType*>(
            SystemTable->ConfigurationTable[i].VendorTable);
        if (!::MoUefiAcpiStructureValidate(
            RootTable,
            OFFSET_OF(RootTableType, Length)))
        {
            continue;
        }
        if (!::MoUefiAcpiStructureValidate(
            RootTable,
            RootTable->Length))
        {
            continue;
        }
        if (RootTableSignature != RootTable->Signature)
        {
            continue;
        }
        if (RootTableRevision != RootTable->Revision)
        {
            continue;
        }
        if (!RootTable->XsdtAddress)
        {
            continue;
        }

        if (!::MoUefiAcpiDescriptionTableValidate(
            reinterpret_cast<MO_POINTER>(RootTable->XsdtAddress),
            EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
            EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION))
        {
            continue;
        }

        *TableAddress = RootTable->XsdtAddress;

        // Break the for loop since we found the table.
        break;
    }

    if (!*TableAddress)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryDescriptionTable(
    _Out_ PMO_UINT64 TableAddress,
    _In_ MO_UINT32 ExpectedSignature,
    _In_ MO_UINT8 MinimumRevision,
    _In_ MO_UINT64 ExtendedSystemDescriptionTable)
{
    if (!TableAddress || !ExtendedSystemDescriptionTable)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *TableAddress = 0u;

    if (!::MoUefiAcpiDescriptionTableValidate(
        reinterpret_cast<MO_POINTER>(ExtendedSystemDescriptionTable),
        EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE,
        EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION))
    {
        return MO_RESULT_ERROR_INVALID_POINTER;
    }
    EFI_ACPI_DESCRIPTION_HEADER* TableHeader =
        reinterpret_cast<EFI_ACPI_DESCRIPTION_HEADER*>(
            ExtendedSystemDescriptionTable);

    PMO_UINT64 TableArray = reinterpret_cast<PMO_UINT64>(&TableHeader[1]);
    MO_UINTN TableCount = TableHeader->Length;
    TableCount -= sizeof(EFI_ACPI_DESCRIPTION_HEADER);
    TableCount /= sizeof(MO_UINT64);
    for (MO_UINTN i = 0; i < TableCount; ++i)
    {
        if (!::MoUefiAcpiDescriptionTableValidate(
            reinterpret_cast<MO_POINTER>(TableArray[i]),
            ExpectedSignature,
            MinimumRevision))
        {
            continue;
        }

        *TableAddress = TableArray[i];

        // Break the for loop since we found the table.
        break;
    }

    if (!*TableAddress)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMemoryRanges(
    _Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MemoryRanges,
    _Out_ PMO_UINTN MemoryRangesCount,
    _In_ MO_UINT64 SystemResourceAffinityTable)
{
    if (!MemoryRanges ||
        !MemoryRangesCount ||
        !SystemResourceAffinityTable)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *MemoryRanges = nullptr;
    *MemoryRangesCount = 0u;

    if (!::MoUefiAcpiDescriptionTableValidate(
        reinterpret_cast<MO_POINTER>(SystemResourceAffinityTable),
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE,
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION))
    {
        return MO_RESULT_ERROR_INVALID_POINTER;
    }

    using TableHeaderType = EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER;
    TableHeaderType* TableHeader = reinterpret_cast<TableHeaderType*>(
        SystemResourceAffinityTable);
    MO_UINTN TableEntriesStart = reinterpret_cast<MO_UINTN>(&TableHeader[1]);

    MO_UINT16 Count = 0;
    {
        MO_UINTN CurrentItem = TableEntriesStart;
        MO_UINT32 ProcessedSize = sizeof(TableHeaderType);
        while (ProcessedSize < TableHeader->Header.Length)
        {
            using CandidateType = EFI_ACPI_3_0_MEMORY_AFFINITY_STRUCTURE;
            CandidateType* Candidate =
                reinterpret_cast<CandidateType*>(CurrentItem);
            if (EFI_ACPI_3_0_MEMORY_AFFINITY == Candidate->Type)
            {
                ++Count;
            }
            ProcessedSize += Candidate->Length;
            CurrentItem += Candidate->Length;
        }
    }
    if (!Count)
    {
        // No Memory Affinity Structure found.
        return MO_RESULT_ERROR_NO_INTERFACE;
    }
    MO_UINT16 Size = static_cast<MO_UINT16>(
        sizeof(MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM) * Count);

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM Ranges = nullptr;
    if (MO_RESULT_SUCCESS_OK != ::MoMemoryInternalHeapAllocate(
        reinterpret_cast<PMO_POINTER>(&Ranges),
        Size))
    {
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    {
        MO_UINTN CurrentIndex = 0;
        MO_UINTN CurrentItem = TableEntriesStart;
        MO_UINT32 ProcessedSize = sizeof(TableHeaderType);
        while (ProcessedSize < TableHeader->Header.Length)
        {
            using CandidateType = EFI_ACPI_3_0_MEMORY_AFFINITY_STRUCTURE;
            CandidateType* Candidate =
                reinterpret_cast<CandidateType*>(CurrentItem);
            if (EFI_ACPI_3_0_MEMORY_AFFINITY == Candidate->Type)
            {
                MO_UINT64 AddressBase = Candidate->AddressBaseHigh;
                AddressBase <<= 32;
                AddressBase |= Candidate->AddressBaseLow;
                MO_UINT64 Length = Candidate->LengthHigh;
                Length <<= 32;
                Length |= Candidate->LengthLow;
                Ranges[CurrentIndex].AddressBase = AddressBase;
                Ranges[CurrentIndex].Length = Length;
                ++CurrentIndex;
            }
            ProcessedSize += Candidate->Length;
            CurrentItem += Candidate->Length;
        }
    }

    if (MO_RESULT_SUCCESS_OK != ::MoRuntimeElementSort(
        Ranges,
        Count,
        sizeof(MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM),
        [](
            _In_ MO_POINTER Left,
            _In_ MO_POINTER Right,
            _In_ MO_POINTER Context) -> MO_INTN MOAPI
    {
        Context; // Unused parameter.
        PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM LeftItem =
            reinterpret_cast<PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM>(Left);
        PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM RightItem =
            reinterpret_cast<PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM>(Right);
        if (LeftItem->AddressBase < RightItem->AddressBase)
        {
            return -1;
        }
        else if (LeftItem->AddressBase > RightItem->AddressBase)
        {
            return 1;
        }
        return 0;
    },
        nullptr))
    {
        // Cleanup on error.
        ::MoMemoryInternalHeapFree(Ranges);
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    *MemoryRanges = Ranges;
    *MemoryRangesCount = Count;

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMergedMemoryRanges(
    _Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MergedMemoryRanges,
    _Out_ PMO_UINTN MergedMemoryRangesCount,
    _In_ MO_UINT64 SystemResourceAffinityTable)
{
    if (!MergedMemoryRanges ||
        !MergedMemoryRangesCount ||
        !SystemResourceAffinityTable)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *MergedMemoryRanges = nullptr;
    *MergedMemoryRangesCount = 0u;

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM MemoryRanges = nullptr;
    MO_UINTN MemoryRangesCount = 0u;
    MO_RESULT Result = ::MoUefiAcpiQueryMemoryRanges(
        &MemoryRanges,
        &MemoryRangesCount,
        SystemResourceAffinityTable);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    // Merge adjacent or overlapping ranges.
    MO_UINTN Count = 0;
    {
        for (MO_UINTN i = 0; i < MemoryRangesCount; ++i)
        {
            if (!Count)
            {
                MemoryRanges[Count] = MemoryRanges[i];
                ++Count;
            }
            else
            {
                PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM LastMergedRange =
                    &MemoryRanges[Count - 1];
                MO_UINT64 LastMergedRangeEnd =
                    LastMergedRange->AddressBase + LastMergedRange->Length;
                MO_UINT64 CurrentRangeStart = MemoryRanges[i].AddressBase;
                if (CurrentRangeStart <= LastMergedRangeEnd)
                {
                    // Overlapping or adjacent ranges, merge them.
                    MO_UINT64 CurrentRangeEnd =
                        MemoryRanges[i].AddressBase + MemoryRanges[i].Length;
                    if (CurrentRangeEnd > LastMergedRangeEnd)
                    {
                        LastMergedRange->Length =
                            CurrentRangeEnd - LastMergedRange->AddressBase;
                    }
                }
                else
                {
                    // Non-overlapping range, add it to the merged list.
                    MemoryRanges[Count] = MemoryRanges[i];
                    ++Count;
                }
            }
        }
    }
    if (!Count)
    {
        // Should not happen.
        ::MoMemoryInternalHeapFree(MemoryRanges);
        return MO_RESULT_ERROR_UNEXPECTED;
    }
    MO_UINT16 Size = static_cast<MO_UINT16>(
        sizeof(MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM) * Count);

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM MergedRanges = nullptr;
    if (MO_RESULT_SUCCESS_OK != ::MoMemoryInternalHeapAllocate(
        reinterpret_cast<PMO_POINTER>(&MergedRanges),
        Size))
    {
        ::MoMemoryInternalHeapFree(MemoryRanges);
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    if (MO_RESULT_SUCCESS_OK != ::MoRuntimeMemoryMove(
        MergedRanges,
        MemoryRanges,
        Size))
    {
        ::MoMemoryInternalHeapFree(MemoryRanges);
        ::MoMemoryInternalHeapFree(MergedRanges);
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    ::MoMemoryInternalHeapFree(MemoryRanges);

    *MergedMemoryRanges = MergedRanges;
    *MergedMemoryRangesCount = Count;

    return MO_RESULT_SUCCESS_OK;
}

EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMemoryHoles(
    _Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MemoryHoleRanges,
    _Out_ PMO_UINTN MemoryHoleRangesCount,
    _In_ MO_UINT64 SystemResourceAffinityTable)
{
    if (!MemoryHoleRanges ||
        !MemoryHoleRangesCount ||
        !SystemResourceAffinityTable)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *MemoryHoleRanges = nullptr;
    *MemoryHoleRangesCount = 0u;

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM MergedMemoryRanges = nullptr;
    MO_UINTN MergedMemoryRangesCount = 0u;
    MO_RESULT Result = ::MoUefiAcpiQueryMergedMemoryRanges(
        &MergedMemoryRanges,
        &MergedMemoryRangesCount,
        SystemResourceAffinityTable);
    if (MO_RESULT_SUCCESS_OK != Result)
    {
        return Result;
    }

    if (MergedMemoryRangesCount < 2)
    {
        // No memory holes found.
        ::MoMemoryInternalHeapFree(MergedMemoryRanges);
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    MO_UINTN HoleCount = MergedMemoryRangesCount - 1;
    MO_UINT16 Size = static_cast<MO_UINT16>(
        sizeof(MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM) * HoleCount);
    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM Holes = nullptr;
    if (MO_RESULT_SUCCESS_OK != ::MoMemoryInternalHeapAllocate(
        reinterpret_cast<PMO_POINTER>(&Holes),
        Size))
    {
        ::MoMemoryInternalHeapFree(MergedMemoryRanges);
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    for (MO_UINTN i = 0; i < HoleCount; ++i)
    {
        MO_UINT64 PreviousRangeEnd =
            MergedMemoryRanges[i].AddressBase + MergedMemoryRanges[i].Length;
        MO_UINT64 CurrentRangeStart = MergedMemoryRanges[i + 1].AddressBase;
        Holes[i].AddressBase = PreviousRangeEnd;
        Holes[i].Length = CurrentRangeStart - PreviousRangeEnd;
    }

    ::MoMemoryInternalHeapFree(MergedMemoryRanges);

    *MemoryHoleRanges = Holes;
    *MemoryHoleRangesCount = HoleCount;

    return MO_RESULT_SUCCESS_OK;
}
