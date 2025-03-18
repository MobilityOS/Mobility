/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Retrovisor.cpp
 * PURPOSE:   Implementation for Mobility Retrovisor
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#include <Mile.Project.Version.h>

#include <Uefi.h>

#define MOBILITY_RETROVISOR_VERSION_STRING \
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
        L"Mobility Retrovisor"
        L" " MOBILITY_RETROVISOR_VERSION_STRING L"\r\n"
        L"(c) Kenji Mouri. All rights reserved.\r\n"
        L"\r\n");

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
