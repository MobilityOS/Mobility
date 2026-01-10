/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Uefi.Core.cpp
 * PURPOSE:    Implementation for Mobility UEFI Core functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Uefi.Core.h"

#include <Mobility.Runtime.Core.h>
#include <Mobility.Unicode.Core.h>

#include <Protocol/GraphicsOutput.h>

EXTERN_C VOID MOAPI MoUefiConsoleWriteUcs2String(
    _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
    _In_ MO_CONSTANT_WIDE_STRING String)
{
    Output->OutputString(
        Output,
        const_cast<CHAR16*>(reinterpret_cast<const CHAR16*>(String)));
}

EXTERN_C VOID MOAPI MoUefiConsoleWriteAsciiString(
    _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
    _In_ MO_CONSTANT_STRING String)
{
    MO_UINTN StringLength = ::MoRuntimeStringLength(String);

    MO_WIDE_CHAR WideStringTemplate[2] = { 0, 0 };
    for (MO_UINTN Index = 0; Index < StringLength; ++Index)
    {
        WideStringTemplate[0] = String[Index] > MO_UNICODE_DELETE
            ? MO_UNICODE_SPACE
            : String[Index];
        ::MoUefiConsoleWriteUcs2String(Output, WideStringTemplate);
    }
}

EXTERN_C EFI_STATUS MOAPI MoUefiInitializeDisplayFrameBuffer(
    _Out_ PMO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer,
    _In_ EFI_BOOT_SERVICES* BootServices)
{
    if (!DisplayFrameBuffer || !BootServices)
    {
        return EFI_INVALID_PARAMETER;
    }
    ::MoRuntimeMemoryFillByte(
        DisplayFrameBuffer,
        0u,
        sizeof(MO_DISPLAY_BGRA32_FRAMEBUFFER));

    EFI_GRAPHICS_OUTPUT_PROTOCOL* GraphicsOutputProtocol = nullptr;
    EFI_STATUS Status = BootServices->LocateProtocol(
        &gEfiGraphicsOutputProtocolGuid,
        nullptr,
        reinterpret_cast<void**>(&GraphicsOutputProtocol));
    if (EFI_SUCCESS != Status)
    {
        return Status;
    }
    if (PixelBlueGreenRedReserved8BitPerColor != 
        GraphicsOutputProtocol->Mode->Info->PixelFormat)
    {
        return EFI_UNSUPPORTED;
    }

    DisplayFrameBuffer->FrameBufferBase = reinterpret_cast<PMO_UINT32>(
        GraphicsOutputProtocol->Mode->FrameBufferBase);
    DisplayFrameBuffer->HorizontalResolution =
        GraphicsOutputProtocol->Mode->Info->HorizontalResolution;
    DisplayFrameBuffer->VerticalResolution =
        GraphicsOutputProtocol->Mode->Info->VerticalResolution;
    DisplayFrameBuffer->PixelsPerScanLine =
        GraphicsOutputProtocol->Mode->Info->PixelsPerScanLine;

    if (!::MoDisplayFrameBufferValidate(DisplayFrameBuffer))
    {
        return EFI_UNSUPPORTED;
    }

    return EFI_SUCCESS;
}

EXTERN_C MO_RESULT MOAPI MoUefiQuerySystemConfigurationTable(
    _Out_ PMO_POINTER Table,
    _In_ EFI_SYSTEM_TABLE* SystemTable,
    _In_ EFI_GUID* Guid)
{
    if (!Table || !SystemTable || !Guid)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }
    *Table = nullptr;

    EFI_CONFIGURATION_TABLE* Tables = SystemTable->ConfigurationTable;
    if (!Tables)
    {
        return MO_RESULT_ERROR_NO_INTERFACE;
    }

    for (MO_UINTN i = 0; i < SystemTable->NumberOfTableEntries; ++i)
    {
        if (0 != ::MoRuntimeMemoryCompare(
            &Tables[i].VendorGuid,
            Guid,
            sizeof(EFI_GUID)))
        {
            continue;
        }

        // Reference: https://uefi.org/specs/UEFI/2.11/04_EFI_System_Table.html
        // According to the reference, we will know that the EFI Configuration
        // Table may contain at most once instance of each table type.

        *Table = reinterpret_cast<MO_POINTER>(Tables[i].VendorTable);
        return MO_RESULT_SUCCESS_OK;
    }

    return MO_RESULT_ERROR_NO_INTERFACE;
}
