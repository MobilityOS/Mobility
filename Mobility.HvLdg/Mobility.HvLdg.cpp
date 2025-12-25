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

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM MemoryHoleRanges = nullptr;
    MO_UINTN MemoryHolesCount = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryMemoryHoles(
        &MemoryHoleRanges,
        &MemoryHolesCount,
        SystemResourceAffinityTable))
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "No Memory Holes found from ACPI SRAT.\r\n");
        return;
    }
    ::MoUefiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "Memory Holes found from ACPI SRAT:\r\n");

    for (MO_UINTN i = 0; i < MemoryHolesCount; ++i)
    {
        // 19 characters: "0x" + 16 hex digits + '\0'
        MO_CHAR AddressBuffer[19];

        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Hole Address: ");
        if (MO_RESULT_SUCCESS_OK == ::MoRuntimeConvertUnsignedIntegerToHexString(
            AddressBuffer,
            nullptr,
            sizeof(AddressBuffer),
            MemoryHoleRanges[i].AddressBase,
            64u,
            MO_TRUE,
            MO_TRUE))
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                AddressBuffer);
        }
        else
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "<Conversion Error>");
        }
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            ", Length: ");
        if (MO_RESULT_SUCCESS_OK == ::MoRuntimeConvertUnsignedIntegerToHexString(
            AddressBuffer,
            nullptr,
            sizeof(AddressBuffer),
            MemoryHoleRanges[i].Length,
            64u,
            MO_TRUE,
            MO_TRUE))
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                AddressBuffer);
        }
        else
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "<Conversion Error>");
        }
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "\r\n");
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
