/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Uefi.Core.cpp
 * PURPOSE:    Implementation for Mobility Uefi Core
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Uefi.Core.h"

#include <Mobility.Unicode.Core.h>

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
    MO_WIDE_CHAR WideStringTemplate[2] = { 0, 0 };
    while (*String)
    {
        WideStringTemplate[0] = *String > MO_UNICODE_DELETE
            ? MO_UNICODE_SPACE
            : *String++;
        ::MoUefiConsoleWriteUcs2String(Output, WideStringTemplate);
    }
}
