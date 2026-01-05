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

#endif // !MOBILITY_HYPERV_CORE
