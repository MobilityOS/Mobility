/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Uefi.Core.h
 * PURPOSE:    Definition for Mobility UEFI Core functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_UEFI_CORE
#define MOBILITY_UEFI_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#undef NULL
#include <Uefi.h>

#include <Mobility.Display.Core.h>

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

/**
 * @brief Initializes the BGRA32 display frame buffer information from UEFI Boot
 *        Services.
 * @param DisplayFrameBuffer The pointer to receive the initialized BGRA32
 *                           display frame buffer information. If this parameter
 *                           is nullptr, the function returns
 *                           EFI_INVALID_PARAMETER.
 * @param BootServices The pointer to the UEFI Boot Services table. If this
 *                     parameter is nullptr, the function returns
 *                     EFI_INVALID_PARAMETER.
 * @return If the function succeeds, it returns EFI_SUCCESS. Otherwise, it
 *         returns an EFI_STATUS error code.
 * @remarks If the display frame buffer does not support BGRA32 pixel format or
 *          the display frame buffer information is invalid, the function
 *          returns EFI_UNSUPPORTED.
 */
EXTERN_C EFI_STATUS MOAPI MoUefiInitializeDisplayFrameBuffer(
    _Out_ PMO_DISPLAY_BGRA32_FRAMEBUFFER DisplayFrameBuffer,
    _In_ EFI_BOOT_SERVICES* BootServices);

#endif // !MOBILITY_UEFI_CORE

