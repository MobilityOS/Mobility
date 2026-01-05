/*
 * PROJECT:    Mobility
 * FILE:       Mobility.HyperV.Core.cpp
 * PURPOSE:    Implementation for Mobility Hyper-V Core functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.HyperV.Core.h"

#include <Mobility.Runtime.Core.h>
#include <Mobility.Platform.x64.h>

EXTERN_C MO_RESULT MOAPI MoHyperVCheckAvailability()
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
    ::MoPlatformReadCpuid(
        reinterpret_cast<PMO_PLATFORM_X64_CPUID_RESULT>(&HvCpuIdResult),
        HvCpuIdFunctionVersionAndFeatures);
    if (!HvCpuIdResult.VersionAndFeatures.HypervisorPresent)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    ::MoRuntimeMemoryFillByte(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::MoPlatformReadCpuid(
        reinterpret_cast<PMO_PLATFORM_X64_CPUID_RESULT>(&HvCpuIdResult),
        HvCpuIdFunctionHvInterface);
    if (HvMicrosoftHypervisorInterface != HvCpuIdResult.HvInterface.Interface)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    ::MoRuntimeMemoryFillByte(&HvCpuIdResult, 0, sizeof(HV_CPUID_RESULT));
    ::MoPlatformReadCpuid(
        reinterpret_cast<PMO_PLATFORM_X64_CPUID_RESULT>(&HvCpuIdResult),
        HvCpuIdFunctionMsHvFeatures);
    if (!HvCpuIdResult.MsHvFeatures.PartitionPrivileges.AccessHypercallMsrs)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }
    if (!HvCpuIdResult.MsHvFeatures.PartitionPrivileges.AccessSynicRegs)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    return MO_RESULT_SUCCESS_OK;
}
