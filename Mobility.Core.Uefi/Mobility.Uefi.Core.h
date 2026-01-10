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

/**
 * @brief Acquires the specified system configuration table from the UEFI System
 *        Table based on the provided GUID.
 * @param Table The pointer to receive the address of the requested system
 *              configuration table. If this parameter is nullptr, the function
 *              returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param SystemTable The pointer to the UEFI system table. If this parameter is
 *                    nullptr, the function returns
 *                    MO_RESULT_ERROR_INVALID_PARAMETER
 * @param Guid The pointer to the GUID that identifies the desired system
 *             configuration table. If this parameter is nullptr, the function
 *             returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the specified system configuration table is not found, the function
 *          returns MO_RESULT_ERROR_NO_INTERFACE.
 */
EXTERN_C MO_RESULT MOAPI MoUefiQuerySystemConfigurationTable(
    _Out_ PMO_POINTER Table,
    _In_ EFI_SYSTEM_TABLE* SystemTable,
    _In_ EFI_GUID* Guid);

#endif // !MOBILITY_UEFI_CORE

