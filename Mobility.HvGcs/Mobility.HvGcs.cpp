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
    }

    ::OutputWideString(
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
    }

    return EFI_SUCCESS;
}

#pragma comment(linker, "/entry:UefiMain")
