/*
 * PROJECT:   Mobility
 * FILE:      Mobility.HvGcs.cpp
 * PURPOSE:   Implementation for Mobility Hyper-V Guest Compatibility Shim
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Mile.Project.Version.h>

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>
#include <Guid/Acpi.h>
#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Acpi30.h>

#include <Mile.HyperV.VMBus.h>

#include <intrin.h>
#include <sal.h>
#include <string.h>

/**
 * @brief Checks the Hyper-V guest support availability.
 * @return If the Hyper-V guest support is available to use, the return value
 *         is true, or the return value is false.
 */
extern "C" bool MoHvCheckAvailability()
{
    // Reference: Requirements for Implementing the Microsoft Hypervisor
    //            Interface
    //
    // The minimal interface set required by compliant hypervisors in order to
    // support Windows operating systems when running in a guest virtual machine
    // is summarized below. Details of each requirement are provided in
    // subsequent sections.
    // - Hypervisor discovery via the CPUID instruction
    // - Hypervisor CPUID leaves 0x40000000 - 0x40000005
    // - Hypervisor interface signature equal to "Hv#1"
    // - Partition privileges AccessVpIndex, AccessHypercallMsrs
    // - Hypervisor synthetic MSRs HV_X64_MSR_GUEST_OS_ID, HV_X64_MSR_HYPERCALL
    //   and HV_X64_MSR_VP_INDEX.
    // - A minimal implementation of the hypercall interface
    // 
    // Software determines the presence of a hypervisor through the CPUID
    // instruction. Processors conforming to the Intel® 64 architecture have
    // reserved a feature flag in CPUID Function 0x01 - Feature Information for
    // this purpose.Bit 31 returned in ECX is defined as Not Used, and will
    // always return 0 from the physical CPU. A hypervisor conformant with the
    // Microsoft hypervisor interface will set CPUID.1:ECX[bit 31] = 1 to
    // indicate its presence to software.
    // 
    // The Intel® 64 architecture reserves CPUID leaves 0x40000000-0x400000FF
    // for use by system software. A Microsoft-compliant hypervisor guarantees
    // leaves 0x40000000 and 0x40000001 are always available.
    // The hypervisor interface identification is provided at CPUID leaf
    // 0x40000001. Hypervisors conforming to the Microsoft hypervisor interface
    // will return the hypervisor interface identification signature 'Hv#1'
    // (0x31237648) in CPUID.40000001:EAX.   

    HV_CPUID_RESULT HvCpuIdResult;

    ::memset(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::__cpuid(
        reinterpret_cast<int*>(&HvCpuIdResult),
        HvCpuIdFunctionVersionAndFeatures);
    if (!HvCpuIdResult.VersionAndFeatures.HypervisorPresent)
    {
        return false;
    }

    ::memset(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::__cpuid(
        reinterpret_cast<int*>(&HvCpuIdResult),
        HvCpuIdFunctionHvInterface);
    if (HvMicrosoftHypervisorInterface != HvCpuIdResult.HvInterface.Interface)
    {
        return false;
    }

    ::memset(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::__cpuid(
        reinterpret_cast<int*>(&HvCpuIdResult),
        HvCpuIdFunctionMsHvFeatures);
    if (!HvCpuIdResult.MsHvFeatures.PartitionPrivileges.AccessHypercallMsrs)
    {
        return false;
    }
    if (!HvCpuIdResult.MsHvFeatures.PartitionPrivileges.AccessSynicRegs)
    {
        return false;
    }

    return true;
}

/**
 * @brief Calculates the 8-bit sum for the requested region.
 * @param Buffer Pointer to buffer containing byte data of component.
 * @param Size Size of the buffer.
 * @return The 8-bit checksum value needed.
 */
extern "C" uint8_t MoCalculateSum8(
    _In_ uint8_t* Buffer,
    _In_ size_t Size)
{
    uint8_t Result = 0;
    for (size_t i = 0; i < Size; ++i)
    {
        Result += Buffer[i];
    }
    return Result;
}

/**
 * @brief Calculates the value needed for a valid 8-bit checksum.
 * @param Buffer Pointer to buffer containing byte data of component.
 * @param Size Size of the buffer.
 * @return The 8-bit checksum value needed.
 */
extern "C" uint8_t MoCalculateChecksum8(
    _In_ uint8_t* Buffer,
    _In_ size_t Size)
{
    return static_cast<uint8_t>(0x100 - ::MoCalculateSum8(Buffer, Size));
}

typedef struct _MO_ACPI_DESCRIPTION_TABLES
{
    EFI_ACPI_DESCRIPTION_HEADER* XsdtHeader;
    EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER* MadtHeader;
    EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE* Fadt;
    EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER* SratHeader;
} MO_ACPI_DESCRIPTION_TABLES, *PMO_ACPI_DESCRIPTION_TABLES;

/**
 * @brief Retrieves the ACPI description tables information structure from the
 *        UEFI system table.
 * @param SystemTable Pointer to the UEFI system table.
 * @param DescriptionTables Pointer to the ACPI description tables information
 *                          structure which will be initialized to zero at the
 *                          beginning.
 */
extern "C" void MoAcpiGetDescriptionTables(
    _In_ EFI_SYSTEM_TABLE* SystemTable,
    _Out_ PMO_ACPI_DESCRIPTION_TABLES DescriptionTables)
{
    if (!DescriptionTables)
    {
        return;
    }
    ::memset(DescriptionTables, 0, sizeof(MO_ACPI_DESCRIPTION_TABLES));

    for (UINTN i = 0; i < SystemTable->NumberOfTableEntries; ++i)
    {
        if (0 != ::memcmp(
            &SystemTable->ConfigurationTable[i].VendorGuid,
            &gEfiAcpiTableGuid,
            sizeof(EFI_GUID)))
        {
            continue;
        }

        EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER* RsdpCandidate =
            reinterpret_cast<EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER*>(
                SystemTable->ConfigurationTable[i].VendorTable);
        if (EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_SIGNATURE
            != RsdpCandidate->Signature)
        {
            continue;
        }
        if (EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER_REVISION
            != RsdpCandidate->Revision)
        {
            continue;
        }
        if (0 != ::MoCalculateSum8(
            reinterpret_cast<uint8_t*>(RsdpCandidate),
            OFFSET_OF(EFI_ACPI_2_0_ROOT_SYSTEM_DESCRIPTION_POINTER, Length)))
        {
            continue;
        }
        if (0 != ::MoCalculateSum8(
            reinterpret_cast<uint8_t*>(RsdpCandidate),
            RsdpCandidate->Length))
        {
            continue;
        }

        EFI_ACPI_DESCRIPTION_HEADER* XsdtHeaderCandidate =
            reinterpret_cast<EFI_ACPI_DESCRIPTION_HEADER*>(
                RsdpCandidate->XsdtAddress);
        if (EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_SIGNATURE
            != XsdtHeaderCandidate->Signature)
        {
            continue;
        }
        if (XsdtHeaderCandidate->Revision
            < EFI_ACPI_2_0_EXTENDED_SYSTEM_DESCRIPTION_TABLE_REVISION)
        {
            continue;
        }
        if (0 != ::MoCalculateSum8(
            reinterpret_cast<uint8_t*>(XsdtHeaderCandidate),
            XsdtHeaderCandidate->Length))
        {
            continue;
        }

        DescriptionTables->XsdtHeader = XsdtHeaderCandidate;
        break;
    }

    if (!DescriptionTables->XsdtHeader)
    {
        return;
    }

    uint64_t* XsdtEntryArray = reinterpret_cast<uint64_t*>(
        &DescriptionTables->XsdtHeader[1]);
    size_t XsdtEntryArraySize = DescriptionTables->XsdtHeader->Length;
    XsdtEntryArraySize -= sizeof(EFI_ACPI_DESCRIPTION_HEADER);
    XsdtEntryArraySize /= sizeof(uint64_t);
    for (size_t i = 0; i < XsdtEntryArraySize; ++i)
    {
        EFI_ACPI_DESCRIPTION_HEADER* EntryCandidate =
            reinterpret_cast<EFI_ACPI_DESCRIPTION_HEADER*>(XsdtEntryArray[i]);
        if (0 != ::MoCalculateSum8(
            reinterpret_cast<uint8_t*>(EntryCandidate),
            EntryCandidate->Length))
        {
            break;
        }

        switch (EntryCandidate->Signature)
        {
        case EFI_ACPI_2_0_MULTIPLE_SAPIC_DESCRIPTION_TABLE_SIGNATURE:
            if (EntryCandidate->Revision
                >= EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION)
            {
                DescriptionTables->MadtHeader =
                    reinterpret_cast<EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER*>(
                        EntryCandidate);
            }
            break;
        case EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE:
            if (EntryCandidate->Revision
                >= EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION)
            {
                DescriptionTables->Fadt =
                    reinterpret_cast<EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE*>(
                        EntryCandidate);
            }
            break;
        case EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE:
            if (EntryCandidate->Revision
                >= EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION)
            {
                DescriptionTables->SratHeader =
                    reinterpret_cast<EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER*>(
                        EntryCandidate);
            }
            break;
        default:
            break;
        }
    }
}

#define MOBILITY_HVGCS_VERSION_STRING \
    MILE_PROJECT_VERSION_STRING L" (Build " \
    MILE_PROJECT_MACRO_TO_STRING(MILE_PROJECT_VERSION_BUILD) L")"

namespace
{
    static EFI_STATUS OutputWideString(
        _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
        _In_ const wchar_t* String)
    {
        return Output->OutputString(
            Output,
            const_cast<CHAR16*>(reinterpret_cast<const CHAR16*>(String)));
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
    ::OutputWideString(
        SystemTable->ConOut,
        L"Mobility Hyper-V Guest Compatibility Shim"
        L" " MOBILITY_HVGCS_VERSION_STRING L"\r\n"
        L"(c) Kenji Mouri. All rights reserved.\r\n"
        L"\r\n");

    if (::MoHvCheckAvailability())
    {
        ::OutputWideString(
            SystemTable->ConOut,
            L"Yolo!\r\n");

        MO_ACPI_DESCRIPTION_TABLES DescriptionTables;
        ::MoAcpiGetDescriptionTables(SystemTable, &DescriptionTables);

        if (DescriptionTables.MadtHeader)
        {
            DescriptionTables.MadtHeader->Flags |= EFI_ACPI_2_0_PCAT_COMPAT;
            DescriptionTables.MadtHeader->Header.Checksum = 0;
            DescriptionTables.MadtHeader->Header.Checksum =
                ::MoCalculateChecksum8(
                    reinterpret_cast<uint8_t*>(DescriptionTables.MadtHeader),
                    DescriptionTables.MadtHeader->Header.Length);
            /*if (!(EFI_ACPI_2_0_PCAT_COMPAT & DescriptionTables.MadtHeader->Flags))
            {
                ::OutputWideString(
                    SystemTable->ConOut,
                    L"Not Yolo!\r\n");
            }*/
        }

        if (DescriptionTables.Fadt)
        {
            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPm1aEvtBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Pm1aEvtBlk)
                {
                    DescriptionTables.Fadt->Pm1aEvtBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPm1aEvtBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPm1bEvtBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Pm1bEvtBlk)
                {
                    DescriptionTables.Fadt->Pm1bEvtBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPm1bEvtBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPm1aCntBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Pm1aCntBlk)
                {
                    DescriptionTables.Fadt->Pm1aCntBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPm1aCntBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPm1bCntBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Pm1bCntBlk)
                {
                    DescriptionTables.Fadt->Pm1bCntBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPm1bCntBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPm2CntBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Pm2CntBlk)
                {
                    DescriptionTables.Fadt->Pm2CntBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPm2CntBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XPmTmrBlk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->PmTmrBlk)
                {
                    DescriptionTables.Fadt->PmTmrBlk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XPmTmrBlk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XGpe0Blk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Gpe0Blk)
                {
                    DescriptionTables.Fadt->Gpe0Blk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XGpe0Blk.Address);
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO
                == DescriptionTables.Fadt->XGpe1Blk.AddressSpaceId)
            {
                if (!DescriptionTables.Fadt->Gpe1Blk)
                {
                    DescriptionTables.Fadt->Gpe1Blk = static_cast<UINT32>(
                        DescriptionTables.Fadt->XGpe1Blk.Address);
                }
            }

            DescriptionTables.Fadt->Header.Checksum = 0;
            DescriptionTables.Fadt->Header.Checksum =
                ::MoCalculateChecksum8(
                    reinterpret_cast<uint8_t*>(DescriptionTables.Fadt),
                    DescriptionTables.Fadt->Header.Length);
        }

        if (DescriptionTables.SratHeader)
        {
            uint8_t* CurrentSratItemEntry = reinterpret_cast<uint8_t*>(
                &DescriptionTables.SratHeader[1]);
            uint32_t ProcessedSize =
                sizeof(EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER);
            while (ProcessedSize < DescriptionTables.SratHeader->Header.Length)
            {
                EFI_ACPI_3_0_MEMORY_AFFINITY_STRUCTURE* CandidateItem =
                    reinterpret_cast<EFI_ACPI_3_0_MEMORY_AFFINITY_STRUCTURE*>(
                        CurrentSratItemEntry);
                if (EFI_ACPI_3_0_MEMORY_AFFINITY == CandidateItem->Type)
                {
                    uint64_t AddressBase = CandidateItem->AddressBaseHigh;
                    AddressBase <<= 32;
                    AddressBase |= CandidateItem->AddressBaseLow;
                    if (AddressBase >= 0x20000000000ULL)
                    {
                        /*CandidateItem->ProximityDomain = 0;
                        CandidateItem->AddressBaseLow = 0;
                        CandidateItem->AddressBaseHigh = 0;
                        CandidateItem->LengthLow = 0;
                        CandidateItem->LengthHigh = 0;*/
                        CandidateItem->Flags = 0;
                    }
                }
                ProcessedSize += CandidateItem->Length;
                CurrentSratItemEntry += CandidateItem->Length;
            }

            //DescriptionTables.SratHeader->Header.Length = ProcessedSize;
            DescriptionTables.SratHeader->Header.Checksum = 0;
            DescriptionTables.SratHeader->Header.Checksum =
                ::MoCalculateChecksum8(
                    reinterpret_cast<uint8_t*>(DescriptionTables.SratHeader),
                    DescriptionTables.SratHeader->Header.Length);
        }
    }

    ::OutputWideString(
        SystemTable->ConOut,
        L"Patched!\r\n");

    /*::OutputWideString(
        SystemTable->ConOut,
        L"Hello World!\r\n");

    ::OutputWideString(
        SystemTable->ConOut,
        L"\r\n"
        L"Press any key to return to the boot menu...\r\n");
    {
        UINTN Index = 0;
        SystemTable->BootServices->WaitForEvent(
            1,
            &SystemTable->ConIn->WaitForKey,
            &Index);
    }*/

    return EFI_SUCCESS;
}

#pragma comment(linker, "/entry:UefiMain")
