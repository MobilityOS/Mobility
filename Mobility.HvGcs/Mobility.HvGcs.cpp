/*
 * PROJECT:    Mobility
 * FILE:       Mobility.HvGcs.cpp
 * PURPOSE:    Implementation for Mobility Hyper-V Guest Compatibility Shim
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Mile.Mobility.Portable.Types.h>

#include <Mile.Project.Version.h>

#include <sal.h>

#include <Mobility.Runtime.Core.h>
#include <Mobility.Platform.Interface.h>

#include <Mobility.Uefi.Core.h>
#include <Mobility.Uefi.Acpi.h>

#include <IndustryStandard/Acpi20.h>
#include <IndustryStandard/Acpi30.h>
#include <Protocol/LoadedImage.h>

#include <Mobility.HyperV.Core.h>

#include <intrin.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

EXTERN_C MO_RESULT MOAPI MoPlatformHeapAllocate(
    _Out_ PMO_POINTER Block,
    _In_ MO_UINTN Size)
{
    MO_UNREFERENCED_PARAMETER(Block);
    MO_UNREFERENCED_PARAMETER(Size);
    return MO_RESULT_ERROR_NOT_IMPLEMENTED;
}

EXTERN_C MO_RESULT MOAPI MoPlatformHeapFree(
    _In_ MO_POINTER Block)
{
    MO_UNREFERENCED_PARAMETER(Block);
    return MO_RESULT_ERROR_NOT_IMPLEMENTED;
}

EXTERN_C MO_RESULT MOAPI MoPlatformHeapReallocate(
    _Out_ PMO_POINTER UpdatedBlock,
    _In_opt_ MO_POINTER Block,
    _In_ MO_UINTN NewSize)
{
    MO_UNREFERENCED_PARAMETER(UpdatedBlock);
    MO_UNREFERENCED_PARAMETER(Block);
    MO_UNREFERENCED_PARAMETER(NewSize);
    return MO_RESULT_ERROR_NOT_IMPLEMENTED;
}

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

    ::MoRuntimeMemoryFillByte(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::__cpuid(
        reinterpret_cast<int*>(&HvCpuIdResult),
        HvCpuIdFunctionVersionAndFeatures);
    if (!HvCpuIdResult.VersionAndFeatures.HypervisorPresent)
    {
        return false;
    }

    ::MoRuntimeMemoryFillByte(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::__cpuid(
        reinterpret_cast<int*>(&HvCpuIdResult),
        HvCpuIdFunctionHvInterface);
    if (HvMicrosoftHypervisorInterface != HvCpuIdResult.HvInterface.Interface)
    {
        return false;
    }

    ::MoRuntimeMemoryFillByte(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
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

#define MOBILITY_HVGCS_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

namespace
{
    static bool IsDevicePathEndNode(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node)
    {
        return Node
            && END_DEVICE_PATH_TYPE == Node->Type
            && END_ENTIRE_DEVICE_PATH_SUBTYPE == Node->SubType;
    }

    static bool IsDevicePathFilePathNode(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node)
    {
        return Node
            && MEDIA_DEVICE_PATH == Node->Type
            && MEDIA_FILEPATH_DP == Node->SubType;
    }

    static UINT16 GetDevicePathNodeLength(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node)
    {
        return Node
            ? (static_cast<UINT16>(Node->Length[1]) << 8) | Node->Length[0]
            : 0;
    }

    static bool SetDevicePathNodeLength(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node,
        _In_ UINT16 Length)
    {
        if (!Node)
        {
            return false;
        }
        Node->Length[0] = static_cast<UINT8>(Length);
        Node->Length[1] = static_cast<UINT8>(Length >> 8);
        return true;
    }

    static EFI_DEVICE_PATH_PROTOCOL* GetNextDevicePathNode(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node)
    {
        return Node
            ? reinterpret_cast<EFI_DEVICE_PATH_PROTOCOL*>(
                reinterpret_cast<UINT8*>(Node)
                + ::GetDevicePathNodeLength(Node))
            : nullptr;
    }

    static FILEPATH_DEVICE_PATH* DevicePathNodeToFilePathNode(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Node)
    {
        return (Node && ::IsDevicePathFilePathNode(Node))
            ? reinterpret_cast<FILEPATH_DEVICE_PATH*>(Node)
            : nullptr;
    }

    static UINTN GetFilePathLengthFromDevicePath(
        _In_ EFI_DEVICE_PATH_PROTOCOL* Source)
    {
        // Initial length includes the null terminator.
        UINTN Length = 1;
        while (Source)
        {
            FILEPATH_DEVICE_PATH* FilePath =
                ::DevicePathNodeToFilePathNode(Source);
            if (!FilePath)
            {
                break;
            }

            // Add the length of the path separator if needed.
            if (L'\\' != *FilePath->PathName)
            {
                ++Length;
            }

            // Add the file path string.
            Length += ::wcslen(reinterpret_cast<wchar_t*>(FilePath->PathName));

            Source = ::GetNextDevicePathNode(Source);
        }
        return Length;
    }

    static bool GetFilePathFromDevicePath(
        _Out_ wchar_t* Destination,
        _In_ UINTN DestinationLength,
        _In_ EFI_DEVICE_PATH_PROTOCOL* Source)
    {
        if (!Destination || !DestinationLength || !Source)
        {
            return false;
        }

        UINTN RequiredLength = ::GetFilePathLengthFromDevicePath(Source);
        if (RequiredLength > DestinationLength)
        {
            return false;
        }

        ::MoRuntimeMemoryFillByte(
            Destination,
            0,
            DestinationLength * sizeof(wchar_t));
        while (Source)
        {
            FILEPATH_DEVICE_PATH* FilePath =
                ::DevicePathNodeToFilePathNode(Source);
            if (!FilePath)
            {
                break;
            }

            ::wcscat_s(
                Destination,
                DestinationLength,
                reinterpret_cast<wchar_t*>(FilePath->PathName));

            Source = ::GetNextDevicePathNode(Source);
        }

        return true;
    }

    EFI_DEVICE_PATH_PROTOCOL* GetDevicePathFromDeviceHandle(
        _In_ EFI_BOOT_SERVICES* BootServices,
        _In_ EFI_HANDLE DeviceHandle)
    {
        EFI_DEVICE_PATH_PROTOCOL* DevicePath = nullptr;
        EFI_STATUS Status = BootServices->HandleProtocol(
            DeviceHandle,
            &gEfiDevicePathProtocolGuid,
            reinterpret_cast<void**>(&DevicePath));
        return EFI_SUCCESS == Status ? DevicePath : nullptr;
    }

    static UINTN GetAbsoluteDevicePathLengthForFile(
        _In_ EFI_DEVICE_PATH_PROTOCOL* RootDevicePath,
        _In_ wchar_t* RelativeFilePath)
    {
        if (!RootDevicePath || !RelativeFilePath)
        {
            return 0;
        }

        UINTN Length = 0;

        EFI_DEVICE_PATH_PROTOCOL* Current = RootDevicePath;
        while (Current && !::IsDevicePathEndNode(Current))
        {
            Length += ::GetDevicePathNodeLength(Current);
            Current = ::GetNextDevicePathNode(Current);
        }

        while (*RelativeFilePath)
        {
            Length += L'\\' == *RelativeFilePath++
                ? sizeof(FILEPATH_DEVICE_PATH) // Node Header with L'\0'
                : sizeof(CHAR16);
        }
        Length += sizeof(FILEPATH_DEVICE_PATH); // End Node

        return Length;
    }

    static bool CreateAbsoluteDevicePathForFile(
        _Out_ EFI_DEVICE_PATH_PROTOCOL* DevicePathBuffer,
        _In_ size_t DevicePathBufferLength,
        _In_ EFI_DEVICE_PATH_PROTOCOL* RootDevicePath,
        _In_ wchar_t* RelativeFilePath)
    {
        if (!DevicePathBuffer || !DevicePathBufferLength ||
            !RootDevicePath || !RelativeFilePath)
        {
            return false;
        }

        UINTN RequiredLength = ::GetAbsoluteDevicePathLengthForFile(
            RootDevicePath,
            RelativeFilePath);
        if (RequiredLength > DevicePathBufferLength)
        {
            return false;
        }

        ::MoRuntimeMemoryFillByte(
            DevicePathBuffer,
            0,
            DevicePathBufferLength);
        EFI_DEVICE_PATH_PROTOCOL* Current = DevicePathBuffer;
        while (RootDevicePath && !::IsDevicePathEndNode(RootDevicePath))
        {
            UINTN NodeLength = ::GetDevicePathNodeLength(RootDevicePath);
            ::MoRuntimeMemoryMove(Current, RootDevicePath, NodeLength);
            Current = ::GetNextDevicePathNode(Current);
            RootDevicePath = ::GetNextDevicePathNode(RootDevicePath);
        }

        UINT16 RelativeFilePathLength = static_cast<UINT16>(
            ::wcslen(RelativeFilePath) + 1);
        FILEPATH_DEVICE_PATH* FilePath =
            reinterpret_cast<FILEPATH_DEVICE_PATH*>(Current);
        FilePath->Header.Type = MEDIA_DEVICE_PATH;
        FilePath->Header.SubType = MEDIA_FILEPATH_DP;
        UINT16 NodeLength = sizeof(FILEPATH_DEVICE_PATH);
        NodeLength += RelativeFilePathLength * sizeof(CHAR16);
        ::SetDevicePathNodeLength(&FilePath->Header, NodeLength);
        ::MoRuntimeMemoryMove(
            FilePath->PathName,
            RelativeFilePath,
            RelativeFilePathLength * sizeof(CHAR16));
        Current = ::GetNextDevicePathNode(Current);

        Current->Type = END_DEVICE_PATH_TYPE;
        Current->SubType = END_ENTIRE_DEVICE_PATH_SUBTYPE;
        ::SetDevicePathNodeLength(Current, sizeof(EFI_DEVICE_PATH_PROTOCOL));

        return true;
    }

    static EFI_STATUS LaunchImage(
        _In_ EFI_BOOT_SERVICES* BootServices,
        _In_ EFI_HANDLE ParentImageHandle,
        _In_ EFI_DEVICE_PATH_PROTOCOL* ImageDevicePath)
    {
        EFI_HANDLE ImageHandle = nullptr;
        EFI_STATUS Status = BootServices->LoadImage(
            TRUE,
            ParentImageHandle,
            ImageDevicePath,
            nullptr,
            0,
            &ImageHandle);
        if (EFI_SUCCESS != Status)
        {
            return Status;
        }

        EFI_LOADED_IMAGE_PROTOCOL* ImageInformation = nullptr;
        Status = BootServices->HandleProtocol(
            ImageHandle,
            &gEfiLoadedImageProtocolGuid,
            reinterpret_cast<void**>(&ImageInformation));
        if (EFI_SUCCESS == Status &&
            EfiLoaderCode == ImageInformation->ImageCodeType)
        {
            Status = BootServices->StartImage(ImageHandle, nullptr, nullptr);
        }
        else
        {
            BootServices->UnloadImage(ImageHandle);
        }

        return Status;
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
        "Mobility Hyper-V Guest Compatibility Shim"
        " " MOBILITY_HVGCS_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n");

    {
        // 19 characters: "0x" + 16 hex digits + '\0'
        MO_CHAR AddressBuffer[19];

        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "SystemTable = ");
        if (MO_RESULT_SUCCESS_OK == ::MoRuntimeConvertUnsignedIntegerToHexString(
            AddressBuffer,
            nullptr,
            sizeof(AddressBuffer),
            (MO_UINTN)SystemTable,
            sizeof(SystemTable) * 8u,
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

    MO_UINT64 ExtendedSystemDescriptionTable = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryExtendedSystemDescriptionTable(
        &ExtendedSystemDescriptionTable,
        SystemTable))
    {
        ExtendedSystemDescriptionTable = 0u;
    }
    if (::MoHvCheckAvailability() && ExtendedSystemDescriptionTable)
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Hyper-V Generation 2 Virtual Machine detected, "
            "starting to patch ACPI description tables...\r\n");

        MO_UINT64 MultipleApicDescriptionTable = 0u;
        if (MO_RESULT_SUCCESS_OK == ::MoUefiAcpiQueryDescriptionTable(
            &MultipleApicDescriptionTable,
            EFI_ACPI_2_0_MULTIPLE_SAPIC_DESCRIPTION_TABLE_SIGNATURE,
            EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_REVISION,
            ExtendedSystemDescriptionTable))
        {
            using TableType = EFI_ACPI_2_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER;
            TableType* MadtHeader = reinterpret_cast<TableType*>(
                MultipleApicDescriptionTable);
            MadtHeader->Flags |= EFI_ACPI_2_0_PCAT_COMPAT;
            MadtHeader->Header.Checksum = 0;
            ::MoRuntimeCalculateChecksumByte(
                &MadtHeader->Header.Checksum,
                MadtHeader,
                MadtHeader->Header.Length);

            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "ACPI MADT PC-AT Compatibility flags bit is applied.\r\n");
        }

        MO_UINT64 FixedAcpiDescriptionTable = 0u;
        if (MO_RESULT_SUCCESS_OK == ::MoUefiAcpiQueryDescriptionTable(
            &FixedAcpiDescriptionTable,
            EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_SIGNATURE,
            EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE_REVISION,
            ExtendedSystemDescriptionTable))
        {
            using TableType = EFI_ACPI_2_0_FIXED_ACPI_DESCRIPTION_TABLE;
            TableType* Fadt = reinterpret_cast<TableType*>(
                FixedAcpiDescriptionTable);
            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPm1aEvtBlk.AddressSpaceId)
            {
                if (!Fadt->Pm1aEvtBlk)
                {
                    Fadt->Pm1aEvtBlk = static_cast<UINT32>(
                        Fadt->XPm1aEvtBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Pm1aEvtBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPm1bEvtBlk.AddressSpaceId)
            {
                if (!Fadt->Pm1bEvtBlk)
                {
                    Fadt->Pm1bEvtBlk = static_cast<UINT32>(
                        Fadt->XPm1bEvtBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Pm1bEvtBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPm1aCntBlk.AddressSpaceId)
            {
                if (!Fadt->Pm1aCntBlk)
                {
                    Fadt->Pm1aCntBlk = static_cast<UINT32>(
                        Fadt->XPm1aCntBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Pm1aCntBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPm1bCntBlk.AddressSpaceId)
            {
                if (!Fadt->Pm1bCntBlk)
                {
                    Fadt->Pm1bCntBlk = static_cast<UINT32>(
                        Fadt->XPm1bCntBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Pm1bCntBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPm2CntBlk.AddressSpaceId)
            {
                if (!Fadt->Pm2CntBlk)
                {
                    Fadt->Pm2CntBlk = static_cast<UINT32>(
                        Fadt->XPm2CntBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Pm2CntBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XPmTmrBlk.AddressSpaceId)
            {
                if (!Fadt->PmTmrBlk)
                {
                    Fadt->PmTmrBlk = static_cast<UINT32>(
                        Fadt->XPmTmrBlk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT PmTmrBlk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XGpe0Blk.AddressSpaceId)
            {
                if (!Fadt->Gpe0Blk)
                {
                    Fadt->Gpe0Blk = static_cast<UINT32>(
                        Fadt->XGpe0Blk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Gpe0Blk workaround is applied.\r\n");
                }
            }

            if (EFI_ACPI_2_0_SYSTEM_IO == Fadt->XGpe1Blk.AddressSpaceId)
            {
                if (!Fadt->Gpe1Blk)
                {
                    Fadt->Gpe1Blk = static_cast<UINT32>(
                        Fadt->XGpe1Blk.Address);

                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "ACPI FADT Gpe1Blk workaround is applied.\r\n");
                }
            }

            Fadt->Header.Checksum = 0;
            ::MoRuntimeCalculateChecksumByte(
                &Fadt->Header.Checksum,
                Fadt,
                Fadt->Header.Length);
        }

        MO_UINT64 SystemResourceAffinityTable = 0u;
        if (MO_RESULT_SUCCESS_OK == ::MoUefiAcpiQueryDescriptionTable(
            &SystemResourceAffinityTable,
            EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE,
            EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION,
            ExtendedSystemDescriptionTable))
        {
            using TableType = EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER;
            TableType* SratHeader = reinterpret_cast<TableType*>(
                SystemResourceAffinityTable);
            uint8_t* CurrentSratItemEntry = reinterpret_cast<uint8_t*>(
                &SratHeader[1]);
            uint32_t ProcessedSize =
                sizeof(EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_HEADER);
            while (ProcessedSize < SratHeader->Header.Length)
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
                        CandidateItem->Flags = 0;
                    }
                }
                ProcessedSize += CandidateItem->Length;
                CurrentSratItemEntry += CandidateItem->Length;
            }

            SratHeader->Header.Checksum = 0;
            ::MoRuntimeCalculateChecksumByte(
                &SratHeader->Header.Checksum,
                SratHeader,
                SratHeader->Header.Length);

            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "ACPI SRAT workaround is applied.\r\n");
        }

        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "All needed ACPI description tables are patched.\r\n");
    }

    EFI_STATUS Status = EFI_SUCCESS;

    EFI_LOADED_IMAGE_PROTOCOL* CurrentImageInformation = nullptr;
    Status = SystemTable->BootServices->HandleProtocol(
        ImageHandle,
        &gEfiLoadedImageProtocolGuid,
        reinterpret_cast<void**>(&CurrentImageInformation));
    if (EFI_SUCCESS != Status)
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Failed to open the EFI_LOADED_IMAGE_PROTOCOL.\r\n");
    }
    else
    {
        wchar_t TargetFileBuffer[260];
        if (!::GetFilePathFromDevicePath(
            TargetFileBuffer,
            sizeof(TargetFileBuffer) / sizeof(*TargetFileBuffer),
            CurrentImageInformation->FilePath))
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "Failed to get the file path from the device path.\r\n");
        }
        else
        {
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "Current EFI Image Path = ");
            ::MoUefiConsoleWriteUcs2String(
                SystemTable->ConOut,
                TargetFileBuffer);
            ::MoUefiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "\r\n");
            wchar_t* ReplacementStart = ::wcsrchr(TargetFileBuffer, L'\\');
            if (ReplacementStart)
            {
                ReplacementStart = ::wcsrchr(ReplacementStart, L'.');
            }
            if (!ReplacementStart)
            {
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    "Failed to get the valid file path.\r\n");
            }
            else
            {
                ++ReplacementStart;
                size_t BufferLength = sizeof(TargetFileBuffer) - sizeof(wchar_t);
                BufferLength -= ReplacementStart - TargetFileBuffer;
                BufferLength /= sizeof(wchar_t);
                ::wcscpy_s(ReplacementStart, BufferLength, L"original.efi");
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    "Target EFI Image Path = ");
                ::MoUefiConsoleWriteUcs2String(
                    SystemTable->ConOut,
                    TargetFileBuffer);
                ::MoUefiConsoleWriteAsciiString(
                    SystemTable->ConOut,
                    "\r\n");

                EFI_DEVICE_PATH_PROTOCOL* RootDevicePath =
                    ::GetDevicePathFromDeviceHandle(
                        SystemTable->BootServices,
                        CurrentImageInformation->DeviceHandle);
                if (!RootDevicePath)
                {
                    ::MoUefiConsoleWriteAsciiString(
                        SystemTable->ConOut,
                        "Failed to get the root device path.\r\n");
                }
                else
                {
                    UINT8 DevicePathBuffer[4096];
                    ::MoRuntimeMemoryFillByte(
                        DevicePathBuffer,
                        0,
                        sizeof(DevicePathBuffer));
                    EFI_DEVICE_PATH_PROTOCOL* TargetDevicePath =
                        reinterpret_cast<EFI_DEVICE_PATH_PROTOCOL*>(
                            DevicePathBuffer);
                    if (!::CreateAbsoluteDevicePathForFile(
                        TargetDevicePath,
                        sizeof(DevicePathBuffer),
                        RootDevicePath,
                        TargetFileBuffer))
                    {
                        ::MoUefiConsoleWriteAsciiString(
                            SystemTable->ConOut,
                            "Failed to create the absolute device path.\r\n");
                    }
                    else
                    {
                        Status = ::LaunchImage(
                            SystemTable->BootServices,
                            ImageHandle,
                            TargetDevicePath);
                        if (EFI_SUCCESS != Status)
                        {
                            ::MoUefiConsoleWriteAsciiString(
                                SystemTable->ConOut,
                                "Failed to launch the target image.\r\n");
                        }
                    }
                }
            }
        }
    }

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

    return Status;
}

#pragma comment(linker, "/entry:UefiMain")
