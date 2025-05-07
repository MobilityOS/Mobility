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

EXTERN_C VOID MOAPI MoEfiConsoleWriteAsciiString(
    _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
    _In_ MO_CONSTANT_STRING String)
{
    CHAR16 WideStringTemplate[2] = { 0, 0 };
    while (*String)
    {
        WideStringTemplate[0] = *String > MO_UNICODE_DELETE
            ? MO_UNICODE_SPACE
            : *String++;
        Output->OutputString(Output, WideStringTemplate);
    }
}

#define MOBILITY_ONECENT_VERSION_UTF8_STRING \
    MILE_PROJECT_VERSION_UTF8_STRING " (Build " \
    MILE_PROJECT_MACRO_TO_UTF8_STRING(MILE_PROJECT_VERSION_BUILD) ")"

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
    ::MoEfiConsoleWriteAsciiString(
        SystemTable->ConOut,
        "Mobility OneCent"
        " " MOBILITY_ONECENT_VERSION_UTF8_STRING "\r\n"
        "(c) Kenji Mouri. All rights reserved.\r\n"
        "\r\n");

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
            ::MoEfiConsoleWriteAsciiString(
                SystemTable->ConOut,
                "Failed to locate the Graphics Output Protocol.\r\n"
                "\r\nPress any key to return to the boot menu...\r\n");
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

        const char LogoString[] =
            "Mobility OneCent"
            " " MOBILITY_ONECENT_VERSION_UTF8_STRING "\r\n"
            "(c) Kenji Mouri. All rights reserved.\r\n"
            "\r\n"
            "Hello World!\r\n"
            "\r\n";

        char StringTemplate[] = "0\r\n";

        for (MO_UINT32 i = 0; i < 0x00FFFFFF; i++)
        {
            if (0 == i % 10)
            {
                MoConsoleCoreWriteString(
                    &ConsoleScreenBuffer,
                    LogoString,
                    MO_ARRAY_SIZE(LogoString) - 1);
            }

            StringTemplate[0] = '0' + (i % 10);

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
