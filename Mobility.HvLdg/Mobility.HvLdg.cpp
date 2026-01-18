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

#include <Mobility.Runtime.Core.h>
#include "Mobility.Console.Core.h"
#include <Mobility.Platform.x64.h>
#include <Mobility.Platform.Interface.h>
#include <Mobility.Memory.SmallHeap.h>

#include <Mobility.Uefi.Core.h>
#include <Mobility.Uefi.Acpi.h>

#include <IndustryStandard/Acpi30.h>

#include <Mobility.HyperV.Core.h>

#define MOBILITY_MINUAP_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

/**
 * @brief The segment types for the GDT entries.
 */
typedef enum _MO_PLATFORM_X64_SEGMENT_TYPE
{
    MO_PLATFORM_X64_SEGMENT_NULL = 0x00,
    MO_PLATFORM_X64_SEGMENT_KERNEL_CODE = 0x08,
    MO_PLATFORM_X64_SEGMENT_KERNEL_DATA = 0x10,
    MO_PLATFORM_X64_SEGMENT_USER_CODE = 0x18,
    MO_PLATFORM_X64_SEGMENT_USER_DATA = 0x20,
    MO_PLATFORM_X64_SEGMENT_TSS = 0x28,
} MO_PLATFORM_X64_SEGMENT_TYPE, *PMO_PLATFORM_X64_SEGMENT_TYPE;

/**
 * @brief The GDT descriptors for x64 architecture.
 */
typedef struct _MO_PLATFORM_X64_GDT_DESCRIPTORS
{
    MO_PLATFORM_X64_SEGMENT_DESCRIPTOR Null;
    MO_PLATFORM_X64_SEGMENT_DESCRIPTOR KernelCode;
    MO_PLATFORM_X64_SEGMENT_DESCRIPTOR KernelData;
    MO_PLATFORM_X64_SEGMENT_DESCRIPTOR UserCode;
    MO_PLATFORM_X64_SEGMENT_DESCRIPTOR UserData;
    MO_PLATFORM_X64_SYSTEM_SEGMENT_DESCRIPTOR Tss;
} MO_PLATFORM_X64_GDT_DESCRIPTORS, *PMO_PLATFORM_X64_GDT_DESCRIPTORS;

#ifndef MO_PLATFORM_X64_PAGE_SIZE
#define MO_PLATFORM_X64_PAGE_SIZE 0x1000
#endif // !MO_PLATFORM_X64_PAGE_SIZE

#ifndef MO_PLATFORM_X64_PAGE_ALIGNED
#define MO_PLATFORM_X64_PAGE_ALIGNED \
    MO_DECLSPEC_ALIGN(MO_PLATFORM_X64_PAGE_SIZE)
#endif // !MO_PLATFORM_X64_PAGE_ALIGNED

#define MO_PLATFORM_X64_CONSOLE_WIDTH 128
#define MO_PLATFORM_X64_CONSOLE_HEIGHT 40

#define MO_PLATFORM_X64_CONSOLE_SIZE \
    (MO_PLATFORM_X64_CONSOLE_WIDTH * MO_PLATFORM_X64_CONSOLE_HEIGHT)

/**
 * @brief The platform-specific context for x64 architecture.
 */
typedef struct MO_PLATFORM_X64_PAGE_ALIGNED _MO_PLATFORM_X64_PLATFORM_CONTEXT
{
    // Area 1 (64 KiB)

    MO_MEMORY_SMALL_HEAP InternalHeap;

    // Area 2 (64 KiB)

    // Due to the x86 page table design, we can use PML4 Entry (PML4E) page as
    // the protection page for protecting the platform context from the
    // corrupted internal heap.
    MO_PLATFORM_X64_PAGE_DIRECTORY_ENTRY PageMapLevel4Entry[512];

    MO_PLATFORM_X64_PAGE_DIRECTORY_ENTRY PageDirectoryPointerEntry[512];

    MO_PLATFORM_X64_PAGE_TABLE_ENTRY PageTableEntry[512 * 4];

    HV_MESSAGE InterruptMessagePage[HV_SYNIC_SINT_COUNT];

    HV_SYNIC_EVENT_FLAGS InterruptEventFlagsPage[HV_SYNIC_SINT_COUNT];

    MO_UINT8 Reserved0_0[MO_PLATFORM_X64_PAGE_SIZE * 2];

    MO_UINT8 Reserved0_1[2048];
    PMO_PLATFORM_X64_INTERRUPT_HANDLER MoPlatformInterruptHandlers[256];

    MO_PLATFORM_X64_IDT_GATE_DESCRIPTOR InterruptDescriptorTable[256];

    MO_PLATFORM_X64_GDT_DESCRIPTORS GlobalDescriptorTable;
    MO_PLATFORM_X64_TASK_STATE_SEGMENT TaskStateSegment;
    MO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer;
    bool HypercallInitialized;
    MO_UINT8 Reserved1_0[7];
    MO_UINT8 Reserved1_1[40];
    MO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer;
    MO_UINT8 Reserved1_2[512];
    MO_UINT8 Reserved2[1024];
    MO_WIDE_CHAR ConsoleCharacterBuffer[MO_PLATFORM_X64_CONSOLE_SIZE];

    // According to the Hyper-V hypercall page are readable and executable
    // but not writable by the guest from Hypervisor Top Level Functional
    // Specification 6.0b, we can use Hyper-V hypercall page here as the
    // protection page for protecting the platform context from corrupting
    // via the kernel stack overflow.
    MO_UINT8 HypercallPage[MO_PLATFORM_X64_PAGE_SIZE];

    // Area 3 (64 KiB)

    MO_UINT8 KernelStack[MO_PLATFORM_X64_PAGE_SIZE * 16];
} MO_PLATFORM_X64_PLATFORM_CONTEXT, *PMO_PLATFORM_X64_PLATFORM_CONTEXT;

namespace
{
    MO_PLATFORM_X64_PLATFORM_CONTEXT g_PlatformContext;
    const char g_LogoString[] =
        "Mobility Hyper-V Lightweight Debugger for Guests"
        " " MOBILITY_MINUAP_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n";
}

EXTERN_C MO_RESULT MOAPI MoPlatformHeapAllocate(
    _Out_ PMO_POINTER Block,
    _In_ MO_UINTN Size)
{
    // Additional check to ensure Size fits in MO_UINT16.
    if (Size > MO_UINT16_MAX)
    {
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    return ::MoMemorySmallHeapAllocate(
        Block,
        &g_PlatformContext.InternalHeap,
        static_cast<MO_UINT16>(Size));
}

EXTERN_C MO_RESULT MOAPI MoPlatformHeapFree(
    _In_ MO_POINTER Block)
{
    return ::MoMemorySmallHeapFree(&g_PlatformContext.InternalHeap, Block);
}

EXTERN_C MO_RESULT MOAPI MoPlatformHeapReallocate(
    _Out_ PMO_POINTER UpdatedBlock,
    _In_opt_ MO_POINTER Block,
    _In_ MO_UINTN NewSize)
{
    // Additional check to ensure NewSize fits in MO_UINT16.
    if (NewSize > MO_UINT16_MAX)
    {
        return MO_RESULT_ERROR_OUT_OF_MEMORY;
    }

    return ::MoMemorySmallHeapReallocate(
        UpdatedBlock,
        &g_PlatformContext.InternalHeap,
        Block,
        static_cast<MO_UINT16>(NewSize));
}

EXTERN_C VOID MOAPI MoPlatformWriteAsciiString(
    _In_ MO_CONSTANT_STRING String)
{
    ::MoConsoleCoreWriteString(
        &g_PlatformContext.ConsoleScreenBuffer,
        String,
        static_cast<MO_UINT32>(::MoRuntimeStringLength(String)));
    ::MoConsoleCoreRefreshScreen(
        &g_PlatformContext.DisplayFrameBuffer,
        &g_PlatformContext.ConsoleScreenBuffer);
}

EXTERN_C MO_RESULT MOAPI MoPlatformInitialize(
    _In_ EFI_BOOT_SERVICES* BootServices)
{
    if (MO_RESULT_SUCCESS_OK != ::MoRuntimeMemoryFillByte(
        &g_PlatformContext,
        0,
        sizeof(MO_PLATFORM_X64_PLATFORM_CONTEXT)))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    if (MO_RESULT_SUCCESS_OK != ::MoMemorySmallHeapInitialize(
        &g_PlatformContext.InternalHeap))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    // PageMapLevel4Entry
    // PageDirectoryPointerEntry
    // PageTableEntry
    // KernelStack
    // InterruptDescriptorTable
    // GlobalDescriptorTable
    // TaskStateSegment

    if (EFI_SUCCESS != ::MoUefiInitializeDisplayFrameBuffer(
        &g_PlatformContext.DisplayFrameBuffer,
        BootServices))
    {
        // This function should not fail here.
        return MO_RESULT_ERROR_UNEXPECTED;
    }

    ::MoConsoleCoreInitializeScreenBuffer(
        &g_PlatformContext.ConsoleScreenBuffer,
        MO_PLATFORM_X64_CONSOLE_WIDTH,
        MO_PLATFORM_X64_CONSOLE_HEIGHT,
        MO_CONSOLE_DEFAULT_BACKGROUND_COLOR,
        MO_CONSOLE_DEFAULT_FOREGROUND_COLOR,
        g_PlatformContext.ConsoleCharacterBuffer);

    MoConsoleCoreRefreshScreen(
        &g_PlatformContext.DisplayFrameBuffer,
        &g_PlatformContext.ConsoleScreenBuffer);

    return MO_RESULT_SUCCESS_OK;
}

void SimpleDemo(
    _In_ EFI_SYSTEM_TABLE* SystemTable)
{
    if (MO_RESULT_SUCCESS_OK != ::MoPlatformInitialize(
        SystemTable->BootServices))
    {
        ::MoUefiConsoleWriteAsciiString(
            SystemTable->ConOut,
            "Failed to initialize Mobility Platform.\r\n");
        return;
    }
    ::MoPlatformWriteAsciiString(g_LogoString);

    MO_UINT64 ExtendedSystemDescriptionTable = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryExtendedSystemDescriptionTable(
        &ExtendedSystemDescriptionTable,
        SystemTable))
    {
        ::MoPlatformWriteAsciiString(
            "Unable to locate ACPI XSDT.\r\n");
        return;
    }
    ::MoPlatformWriteAsciiString(
        "ACPI XSDT is located successfully.\r\n");

    MO_UINT64 SystemResourceAffinityTable = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryDescriptionTable(
        &SystemResourceAffinityTable,
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_SIGNATURE,
        EFI_ACPI_3_0_SYSTEM_RESOURCE_AFFINITY_TABLE_REVISION,
        ExtendedSystemDescriptionTable))
    {
        ::MoPlatformWriteAsciiString(
            "Unable to locate ACPI SRAT.\r\n");
        return;
    }
    ::MoPlatformWriteAsciiString(
        "ACPI SRAT is located successfully.\r\n");

    PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM MemoryHoleRanges = nullptr;
    MO_UINTN MemoryHolesCount = 0u;
    if (MO_RESULT_SUCCESS_OK != ::MoUefiAcpiQueryMemoryHoles(
        &MemoryHoleRanges,
        &MemoryHolesCount,
        SystemResourceAffinityTable))
    {
        ::MoPlatformWriteAsciiString(
            "No Memory Holes found from ACPI SRAT.\r\n");
        return;
    }
    ::MoPlatformWriteAsciiString(
        "Memory Holes found from ACPI SRAT:\r\n");

    for (MO_UINTN i = 0; i < MemoryHolesCount; ++i)
    {
        // 19 characters: "0x" + 16 hex digits + '\0'
        MO_CHAR AddressBuffer[19];

        ::MoPlatformWriteAsciiString(
            "Hole Address: ");
        if (MO_RESULT_SUCCESS_OK ==
            ::MoRuntimeConvertUnsignedIntegerToHexString(
                AddressBuffer,
                nullptr,
                sizeof(AddressBuffer),
                (MO_UINTN)MemoryHoleRanges[i].AddressBase,
                sizeof(MemoryHoleRanges[i].AddressBase) * 8u,
                MO_TRUE,
                MO_TRUE))
        {
            ::MoPlatformWriteAsciiString(
                AddressBuffer);
        }
        else
        {
            ::MoPlatformWriteAsciiString(
                "<Conversion Error>");
        }
        ::MoPlatformWriteAsciiString(
            ", Length: ");
        if (MO_RESULT_SUCCESS_OK ==
            ::MoRuntimeConvertUnsignedIntegerToDecimalString(
                AddressBuffer,
                nullptr,
                sizeof(AddressBuffer),
                MemoryHoleRanges[i].Length))
        {
            ::MoPlatformWriteAsciiString(
                AddressBuffer);
        }
        else
        {
            ::MoPlatformWriteAsciiString(
                "<Conversion Error>");
        }
        ::MoPlatformWriteAsciiString(
            " Bytes.\r\n");
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
        g_LogoString);

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
