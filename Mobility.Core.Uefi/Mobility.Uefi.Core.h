/*
 * PROJECT:   Mobility
 * FILE:      Mobility.Uefi.Core.h
 * PURPOSE:   Definition for Mobility Uefi Core
 *
 * LICENSE:   The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_UEFI_CORE
#define MOBILITY_UEFI_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#undef NULL
#include <Uefi.h>

/**
 * @brief Write a UCS-2 string to the console.
 * @param Output The pointer to the UEFI Simple Text Output Protocol.
 * @param String The UCS-2 string to be written.
 */
EXTERN_C VOID MOAPI MoUefiConsoleWriteUcs2String(
    _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
    _In_ MO_CONSTANT_WIDE_STRING String);

/**
 * @brief Write an ASCII string to the console.
 * @param Output The pointer to the UEFI Simple Text Output Protocol.
 * @param String The ASCII string to be written.
 */
EXTERN_C VOID MOAPI MoUefiConsoleWriteAsciiString(
    _In_ EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL* Output,
    _In_ MO_CONSTANT_STRING String);

#endif // !MOBILITY_UEFI_CORE

