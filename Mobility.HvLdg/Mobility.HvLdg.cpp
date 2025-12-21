/*
 * PROJECT:    Mobility
 * FILE:       Mobility.HvLdg.cpp
 * PURPOSE:    Implementation for Mobility Hyper-V Lightweight Debugger for
 *             Guests
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#include <Mile.Project.Version.h>

#include <Mobility.Uefi.Core.h>
#include <Mobility.Uefi.Acpi.h>
#include <Mobility.Runtime.Core.h>
#include <Mobility.Memory.InternalHeap.h>

#include <IndustryStandard/Acpi30.h>

#define MOBILITY_MINUAP_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

void SimpleDemo(
    _In_ EFI_SYSTEM_TABLE* SystemTable)
{
    if (MO_RESULT_SUCCESS_OK != ::MoMemoryInternalHeapInitialize())
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Failed to initialize Internal Heap.\r\n");
        return;
    }
    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "Internal Heap is initialized successfully.\r\n");

    MO_UINT64 ExtendedSystemDescriptionTable = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryExtendedSystemDescriptionTable(
        &ExtendedSystemDescriptionTable,
        SystemTable))
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Unable to locate ACPI XSDT.\r\n");
        return;
    }
    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "ACPI XSDT is located successfully.\r\n");

    MO_UINT64 SystemResourceAffinityTable = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryDescriptionTable(
        &SystemResourceAffinityTable,
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE,
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION,
        ExtendedSystemDescriptionTable))
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Unable to locate ACPI SRAT.\r\n");
        return;
    }
    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "ACPI SRAT is located successfully.\r\n");

    using TableHeaderType = EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER;
    TableHeaderType* TableHeader = reinterpret_cast<TableHeaderType*>(
        SystemResourceAffinityTable);

    MO_UINT16 MemoryAffinityCount = 0;
    {
        MO_UINTN CurrentItem = reinterpret_cast<MO_UINTN>(&TableHeader[1]);
        MO_UINT32 ProcessedSize = sizeof(TableHeaderType);
        while (ProcessedSize < TableHeader->Header.Length)
        {
            using CandidateType = EFI_ACPI_3_0_MEMORY_AFFINITY_STRUCTURE;
            CandidateType* Candidate =
                reinterpret_cast<CandidateType*>(CurrentItem);
            if (EFI_ACPI_3_0_MEMORY_AFFINITY == Candidate->Type)
            {
                ++MemoryAffinityCount;
            }
            ProcessedSize += Candidate->Length;
            CurrentItem += Candidate->Length;
        }
    }

    if (MemoryAffinityCount == 0)
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "No Memory Affinity Structure found in ACPI SRAT.\r\n");
        return;
    }

    struct MemoryRangeItem
    {
        MO_UINT64 AddressBase;
        MO_UINT64 Length;
    };
    MemoryRangeItem* MemoryRanges = nullptr;
    if (MO_RESULT_SUCCESS_OK != ::MoMemoryInternalHeapAllocate(
        reinterpret_cast<PMO_POINTER>(&MemoryRanges),
        static_cast<MO_UINT16>(
            sizeof(MemoryRangeItem) * MemoryAffinityCount)))
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Failed to allocate memory for Memory Range.\r\n");
        return;
    }


    {
        MO_UINTN CurrentIndex = 0;
        MO_UINTN CurrentItem = reinterpret_cast<MO_UINTN>(&TableHeader[1]);
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
                MemoryRanges[CurrentIndex].AddressBase = AddressBase;
                MemoryRanges[CurrentIndex].Length = Length;
                ++CurrentIndex;
            }
            ProcessedSize += Candidate->Length;
            CurrentItem += Candidate->Length;
        }
    }

    ::MoRuntimeElementSort(
        MemoryRanges,
        MemoryAffinityCount,
        sizeof(MemoryRangeItem),
        [](
            _In_ MO_POINTER Left,
            _In_ MO_POINTER Right,
            _In_ MO_POINTER Context) -> MO_INTN MOAPI
    {
        Context; // Unused parameter.
        MemoryRangeItem* LeftItem = reinterpret_cast<MemoryRangeItem*>(Left);
        MemoryRangeItem* RightItem = reinterpret_cast<MemoryRangeItem*>(Right);
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
        nullptr);

    // Merge adjacent or overlapping ranges.
    MO_UINT16 MergedRangeCount = 0;
    {
        for (MO_UINT16 i = 0; i < MemoryAffinityCount; ++i)
        {
            if (MergedRangeCount == 0)
            {
                MemoryRanges[MergedRangeCount] = MemoryRanges[i];
                ++MergedRangeCount;
            }
            else
            {
                MemoryRangeItem* LastMergedRange =
                    &MemoryRanges[MergedRangeCount - 1];
                MO_UINT64 LastMergedEnd =
                    LastMergedRange->AddressBase + LastMergedRange->Length;
                MO_UINT64 CurrentStart = MemoryRanges[i].AddressBase;
                MO_UINT64 CurrentEnd =
                    MemoryRanges[i].AddressBase + MemoryRanges[i].Length;
                if (CurrentStart <= LastMergedEnd)
                {
                    // Merge the ranges.
                    if (CurrentEnd > LastMergedEnd)
                    {
                        LastMergedRange->Length =
                            CurrentEnd - LastMergedRange->AddressBase;
                    }
                }
                else
                {
                    MemoryRanges[MergedRangeCount] = MemoryRanges[i];
                    ++MergedRangeCount;
                }
            }
        }
    }

    // Find the hole above the range.
    MemoryRangeItem* MemoryHoleRanges = nullptr;
    if (MO_RESULT_SUCCESS_OK == ::MoMemoryInternalHeapAllocate(
        reinterpret_cast<PMO_POINTER>(&MemoryHoleRanges),
        static_cast<MO_UINT16>(
            sizeof(MemoryRangeItem) * MergedRangeCount)))
    {
        MO_UINT16 MemoryHoleRangeCount = 0;
        for (MO_UINT16 i = 0; i < MergedRangeCount - 1; ++i)
        {
            MO_UINT64 CurrentEnd =
                MemoryRanges[i].AddressBase + MemoryRanges[i].Length;
            MO_UINT64 NextStart = MemoryRanges[i + 1].AddressBase;
            if (NextStart > CurrentEnd)
            {
                MemoryHoleRanges[MemoryHoleRangeCount].AddressBase = CurrentEnd;
                MemoryHoleRanges[MemoryHoleRangeCount].Length =
                    NextStart - CurrentEnd;
                ++MemoryHoleRangeCount;
            }
        }
        if (MemoryHoleRangeCount > 0)
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "Memory Holes found between Memory Affinity Ranges:\r\n");
            for (MO_UINT16 i = 0; i < MemoryHoleRangeCount; ++i)
            {
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    "Hole Address: 0x");
                {
                    MO_UINT64 PrintValue = MemoryHoleRanges[i].AddressBase;
                    char AddressBuffer[17];
                    ::MoRuntimeMemoryFillByte(
                        AddressBuffer,
                        0,
                        sizeof(AddressBuffer));
                    const char HexDigits[] = "0123456789ABCDEF";
                    for (int j = 0; j < 16; ++j)
                    {
                        AddressBuffer[15 - j] = HexDigits[PrintValue & 0x0F];
                        PrintValue >>= 4;
                    }
                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        AddressBuffer);
                }
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    ", Length: 0x");
                {
                    MO_UINT64 PrintValue = MemoryHoleRanges[i].Length;
                    char LengthBuffer[17];
                    ::MoRuntimeMemoryFillByte(
                        LengthBuffer,
                        0,
                        sizeof(LengthBuffer));
                    const char HexDigits[] = "0123456789ABCDEF";
                    for (int j = 0; j < 16; ++j)
                    {
                        LengthBuffer[15 - j] = HexDigits[PrintValue & 0x0F];
                        PrintValue >>= 4;
                    }
                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        LengthBuffer);
                }
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    "\r\n");
            }
        }
        else
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "No Memory Holes found between Memory Affinity Ranges.\r\n");
        }
        ::MoMemoryInternalHeapFree(MemoryHoleRanges);
    }
}

/**
 * Entry point to UEFI Application.
 *
 * @param ImageHandle The image handle of the UEFI Application.
 * @param SystemTable A pointer to the EFI System Table.
 * @return EFI_STATUS. If the function succeeds, the return value is
 *         EFI_SUCCESS.
 */
EFI_STATUS EFIAPI UefiMain(
    _In_ EFI_HANDLE ImageHandle,
    _In_ EFI_SYSTEM_TABLE* SystemTable)
{
    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "Mobility Hyper-V Lightweight Debugger for Guests"
        " " MOBILITY_MINUAP_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n");

    ::SimpleDemo(SystemTable);

    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "Hello World!\r\n");

    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "\r\n"
        "Press any key to return to the boot menu...\r\n");
    {
        UINTN Index = 0;
        SystemTable->BootServices->WaitForEvent(
            1,
            &SystemTable->ConIn->WaitForKey,
            &Index);
    }

    return EFI_SUCCESS;
}

#pragma comment(linker, "/entry:UefiMain")
