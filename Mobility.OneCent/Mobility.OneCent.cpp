/*
 * PROJECT:   Mobility
 * FILE:      Mobility.OneCent.cpp
 * PURPOSE:   Implementation for Mobility OneCent
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#include "Mobility.Console.Core.h"

#include <Mile.Project.Version.h>

#undef NULL
#include <Uefi.h>
#include <Protocol/GraphicsOutput.h>

#define MOBILITY_ONECENT_VERSION_STRING \
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

static MO_WIDE_CHAR g_CharacterBuffer[80 * 25];

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
        L"Mobility OneCent"
        L" " MOBILITY_ONECENT_VERSION_STRING L"\r\n"
        L"(c) Kenji Mouri. All rights reserved.\r\n"
        L"\r\n");

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

    EFI_BOOT_SERVICES* BootServices = SystemTable->BootServices;

    // Disable UEFI watchdog timer
    BootServices->SetWatchdogTimer(0, 0, 0, nullptr);

    // Initialize the basic framebuffer information
    {
        EFI_GRAPHICS_OUTPUT_PROTOCOL* GraphicsOutputProtocol = nullptr;
        EFI_STATUS Status = BootServices->LocateProtocol(
            &gEfiGraphicsOutputProtocolGuid,
            nullptr,
            reinterpret_cast<void**>(&GraphicsOutputProtocol));
        if (EFI_SUCCESS != Status)
        {
            ::OutputWideString(
                SystemTable->ConOut,
                L"Failed to locate the Graphics Output Protocol.\r\n"
                L"\r\nPress any key to return to the boot menu...\r\n");
            {
                UINTN Index = 0;
                BootServices->WaitForEvent(
                    1,
                    &SystemTable->ConIn->WaitForKey,
                    &Index);
            }
            return Status;
        }

        MO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer;
        DisplayFrameBuffer.FrameBufferBase = reinterpret_cast<PMO_UINT32>(
            GraphicsOutputProtocol->Mode->FrameBufferBase);
        DisplayFrameBuffer.HorizontalResolution =
            GraphicsOutputProtocol->Mode->Info->HorizontalResolution;
        DisplayFrameBuffer.VerticalResolution =
            GraphicsOutputProtocol->Mode->Info->VerticalResolution;

        MO_CONSOLE_SCREEN_BUFFER ConsoleScreenBuffer;
        ::MoConsoleCoreInitializeScreenBuffer(
            &ConsoleScreenBuffer,
            80,
            25,
            MO_CONSOLE_DEFAULT_BACKGROUND_COLOR,
            MO_CONSOLE_DEFAULT_FOREGROUND_COLOR,
            g_CharacterBuffer);

        const wchar_t LogoString[] =
            L"Mobility OneCent"
            L" " MOBILITY_ONECENT_VERSION_STRING L"\r\n"
            L"(c) Kenji Mouri. All rights reserved.\r\n"
            L"\r\n"
            L"Hello World!\r\n"
            L"\r\n";

        wchar_t StringTemplate[] = L"0\r\n";

        for (MO_UINT32 i = 0; i < 0x00FFFFFF; i++)
        {
            if (0 == i % 10)
            {
                MoConsoleCoreWriteString(
                    &ConsoleScreenBuffer,
                    LogoString,
                    MO_ARRAY_SIZE(LogoString) - 1);
            }

            StringTemplate[0] = L'0' + (i % 10);

            MoConsoleCoreWriteString(
                &ConsoleScreenBuffer,
                StringTemplate,
                MO_ARRAY_SIZE(StringTemplate) - 1);

            MoConsoleCoreUpdateColorSettings(
                &ConsoleScreenBuffer,
                MO_CONSOLE_DEFAULT_BACKGROUND_COLOR,
                i);

            MoConsoleCoreRefreshScreen(
                &DisplayFrameBuffer,
                &ConsoleScreenBuffer);
        }
    }

    return EFI_SUCCESS;
}

#pragma comment(linker, "/entry:UefiMain")
