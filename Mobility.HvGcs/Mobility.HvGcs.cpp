/*
 * PROJECT:   Mobility
 * FILE:      Mobility.HvGcs.cpp
 * PURPOSE:   Implementation for Mobility Hyper-V Guest Compatibility Shim
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

#include <sal.h>
#include <string.h>

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
        L"Mobility Hyper-V Guest Compatibility Shim\r\n"
        L"================================================================\r\n"
        L"Hello World!\r\n"
        L"================================================================\r\n"
        L"Press any key to return to the boot menu...");
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
