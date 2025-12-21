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
