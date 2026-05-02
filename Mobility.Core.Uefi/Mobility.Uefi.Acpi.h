/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Uefi.Acpi.h
 * PURPOSE:    Definition for Mobility UEFI ACPI functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_UEFI_ACPI
#define MOBILITY_UEFI_ACPI

#include "Mobility.Uefi.Core.h"

/**
 * @brief Validates an ACPI structure by checking its checksum.
 * @param Structure The pointer to the ACPI structure to be validated.
 * @param Size The size of the ACPI structure in bytes.
 * @return If the ACPI structure is valid, the return value is MO_TRUE.
 *         Otherwise, the return value is MO_FALSE.
 */
MO_EXTERN_C MO_BOOL MoUefiAcpiStructureValidate(
    _Mo_In_ MO_POINTER Structure,
    _Mo_In_ MO_UINTN Size);

/**
 * @brief Validates an ACPI description table by checking its signature,
 *        revision, and checksum.
 * @param DescriptionTable The pointer to the ACPI description table to be
 *                         validated.
 * @param ExpectedSignature The expected signature of the ACPI description
 *                          table.
 * @param MinimumRevision The minimum revision of the ACPI description table.
 * @return If the ACPI description table is valid, the return value is MO_TRUE.
 *         Otherwise, the return value is MO_FALSE.
 */
MO_EXTERN_C MO_BOOL MoUefiAcpiDescriptionTableValidate(
    _Mo_In_ MO_POINTER DescriptionTable,
    _Mo_In_ MO_UINT32 ExpectedSignature,
    _Mo_In_ MO_UINT8 MinimumRevision);

/**
 * @brief Acquires the physical address of the Extended System Description Table
 *        (XSDT) from the UEFI system table.
 * @param TableAddress The pointer to receive the physical address of the
 *                     Extended System Description Table (XSDT). If this
 *                     parameter is nullptr, the function returns
 *                     MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param SystemTable The pointer to the UEFI system table. If this parameter is
 *                    nullptr, the function returns
 *                    MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the Extended System Description Table (XSDT) is not found, the
 *          function returns MO_RESULT_ERROR_NO_INTERFACE.
 */
MO_EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryExtendedSystemDescriptionTable(
    _Mo_Out_ PMO_UINT64 TableAddress,
    _Mo_In_ EFI_SYSTEM_TABLE* SystemTable);

/**
 * @brief Acquires the physical address of an ACPI description table from the
 *        Extended System Description Table (XSDT).
 * @param TableAddress The pointer to receive the physical address of the ACPI
 *                     description table. If this parameter is nullptr, the
 *                     function returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param ExpectedSignature The expected signature of the ACPI description
 *                          table.
 * @param MinimumRevision The minimum revision of the ACPI description table.
 * @param ExtendedSystemDescriptionTable The physical address of the Extended
 *                                       System Description Table (XSDT). If
 *                                       this parameter is zero, the function
 *                                       returns
 *                                       MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the specified ACPI description table is not found, the function
 *          returns MO_RESULT_ERROR_NO_INTERFACE.
 *          If the Extended System Description Table (XSDT) is invalid, the
 *          function returns MO_RESULT_ERROR_INVALID_POINTER.
 */
MO_EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryDescriptionTable(
    _Mo_Out_ PMO_UINT64 TableAddress,
    _Mo_In_ MO_UINT32 ExpectedSignature,
    _Mo_In_ MO_UINT8 MinimumRevision,
    _Mo_In_ MO_UINT64 ExtendedSystemDescriptionTable);

/**
 * @brief The simple memory range item structure for ACPI memory queries.
 */
typedef struct _MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM
{
    /**
     * @brief The physical address base of the memory range.
     */
    MO_UINT64 AddressBase;
    /**
     * @brief The length in bytes of the memory range.
     */
    MO_UINT64 Length;
} MO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM, *PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM;

/**
 * @brief Queries the sorted original memory ranges definitions from the System
 *        Resource Affinity Table (SRAT).
 * @param MemoryRanges The pointer to receive the allocated memory block which
 *                     contains an array of memory ranges found in the System
 *                     Resource Affinity Table (SRAT). The caller is responsible
 *                     for freeing the allocated memory block using
 *                     MoPlatformHeapFree. If this parameter is nullptr,
 *                     the function returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param MemoryRangesCount The pointer to receive the count of memory ranges
 *                          found in the System Resource Affinity Table (SRAT).
 * @param SystemResourceAffinityTable The physical address of the System
 *                                    Resource Affinity Table (SRAT). If this
 *                                    parameter is zero, the function returns
 *                                    MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If no memory ranges are found, the function returns
 *          MO_RESULT_ERROR_NO_INTERFACE.
 *          If the System Resource Affinity Table (SRAT) is invalid, the
 *          function returns MO_RESULT_ERROR_INVALID_POINTER.
 */
MO_EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMemoryRanges(
    _Mo_Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MemoryRanges,
    _Mo_Out_ PMO_UINTN MemoryRangesCount,
    _Mo_In_ MO_UINT64 SystemResourceAffinityTable);

/**
 * @brief Queries the sorted merged memory ranges definitions from the System
 *        Resource Affinity Table (SRAT).
 * @param MemoryRanges The pointer to receive the allocated memory block which
 *                     contains an array of memory ranges found in the System
 *                     Resource Affinity Table (SRAT). The caller is responsible
 *                     for freeing the allocated memory block using
 *                     MoPlatformHeapFree. If this parameter is nullptr,
 *                     the function returns MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param MemoryRangesCount The pointer to receive the count of memory ranges
 *                          found in the System Resource Affinity Table (SRAT).
 * @param SystemResourceAffinityTable The physical address of the System
 *                                    Resource Affinity Table (SRAT). If this
 *                                    parameter is zero, the function returns
 *                                    MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If no memory ranges are found, the function returns
 *          MO_RESULT_ERROR_NO_INTERFACE.
 *          If the System Resource Affinity Table (SRAT) is invalid, the
 *          function returns MO_RESULT_ERROR_INVALID_POINTER.
 */
MO_EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMergedMemoryRanges(
    _Mo_Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MergedMemoryRanges,
    _Mo_Out_ PMO_UINTN MergedMemoryRangesCount,
    _Mo_In_ MO_UINT64 SystemResourceAffinityTable);

/**
 * @brief Queries the sorted merged memory holes definitions from the System
 *        Resource Affinity Table (SRAT).
 * @param MemoryHoleRanges The pointer to receive the allocated memory block
 *                         which contains an array of memory holes found in the
 *                         System Resource Affinity Table (SRAT). The caller is
 *                         responsible for freeing the allocated memory block
 *                         using MoPlatformHeapFree. If this parameter is
 *                         nullptr, the function returns
 *                         MO_RESULT_ERROR_INVALID_PARAMETER.
 * @param MemoryHoleRangesCount The pointer to receive the count of memory holes
 *                              found in the System Resource Affinity Table
 *                              (SRAT).
 * @param SystemResourceAffinityTable The physical address of the System
 *                                    Resource Affinity Table (SRAT). If this
 *                                    parameter is zero, the function returns
 *                                    MO_RESULT_ERROR_INVALID_PARAMETER.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If no memory holes are found, the function returns
 *          MO_RESULT_ERROR_NO_INTERFACE.
 *          If the System Resource Affinity Table (SRAT) is invalid, the
 *          function returns MO_RESULT_ERROR_INVALID_POINTER.
 */
MO_EXTERN_C MO_RESULT MOAPI MoUefiAcpiQueryMemoryHoles(
    _Mo_Out_ PMO_UEFI_ACPI_SIMPLE_MEMORY_RANGE_ITEM* MemoryHoleRanges,
    _Mo_Out_ PMO_UINTN MemoryHoleRangesCount,
    _Mo_In_ MO_UINT64 SystemResourceAffinityTable);

#endif // !MOBILITY_UEFI_ACPI
