/*
 * PROJECT:    Mobility
 * FILE:       Mobility.HyperV.Core.h
 * PURPOSE:    Definition for Mobility Hyper-V Core functions
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_HYPERV_CORE
#define MOBILITY_HYPERV_CORE

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#include <Mile.HyperV.VMBus.h>

/**
 * @brief Checks if the Hyper-V guest interface is available.
 * @return If the function succeeds, it returns MO_RESULT_SUCCESS_OK. Otherwise,
 *         it returns an MO_RESULT error code.
 * @remarks If the Hyper-V guest interface is not available, the function
 *          returns MO_RESULT_ERROR_NO_INTERFACE.
 */
EXTERN_C MO_RESULT MOAPI MoHyperVCheckAvailability();

/**
 * @brief Retrieves the counter value from Hyper-V Partition Reference Counter.
 * @return The number in 100 nanosecond units.
 */
EXTERN_C MO_UINT64 MoHyperVGetPartitionReferenceCounter();

/**
 * @brief Retrieves the number of milliseconds that have elapsed since the
 *        Hyper-V partition was created.
 * @return The number of milliseconds.
 */
EXTERN_C MO_UINT64 MoHyperVGetTickCount();

/**
 * @brief Sets hypercall page for the current Hyper-V partition.
 * @param PhysicalAddress The physical address for the page. If this value is 0,
 *                        the hypercall page will be disabled.
 * @return If the function succeeds, the return value is MO_TRUE. Otherwise, the
 *         return value is MO_FALSE.
 */
EXTERN_C MO_BOOL MoHyperVSetHypercallPage(
    _In_ MO_UINT64 PhysicalAddress);

/**
 * @brief Sets synthetic interrupt message page for the current Hyper-V
 *        partition.
 * @param PhysicalAddress The physical address for the page. If this value is 0,
 *                        the synthetic interrupt message page will be disabled.
 * @return If the function succeeds, the return value is MO_TRUE. Otherwise, the
 *         return value is MO_FALSE.
 */
EXTERN_C MO_BOOL MoHyperVSetInterruptMessagePage(
    _In_ MO_UINT64 PhysicalAddress);

/**
 * @brief Sets synthetic interrupt event flags page for the current Hyper-V
 *        partition.
 * @param PhysicalAddress The physical address for the page. If this value is 0,
 *                        the synthetic interrupt event flags page will be
 *                        disabled.
 * @return If the function succeeds, the return value is MO_TRUE. Otherwise, the
 *         return value is MO_FALSE.
 */
EXTERN_C MO_BOOL MoHyperVSetInterruptEventFlagsPage(
    _In_ MO_UINT64 PhysicalAddress);

#endif // !MOBILITY_HYPERV_CORE
