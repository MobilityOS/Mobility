/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.Memory.Unstaged.c
 * PURPOSE:    Implementation for Mobility Unstaged Memory Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mile.Mobility.Utilities.Memory.Unstaged.h"

MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeValidate(
    _Mo_Out_Opt_ PMO_CONSTANT_POINTER EndAddress,
    _Mo_In_Opt_ MO_CONSTANT_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Length)
{
    if (0 != Length && !BaseAddress)
    {
        return MO_FALSE;
    }

    {
        MO_UINTN BaseAddressValue = ((MO_UINTN)(BaseAddress));
        MO_UINTN EndAddressValue = 0;

        if (!MoMileFixedIntegerCheckedAddition(
            &EndAddressValue,
            MO_FALSE,
            BaseAddressValue,
            Length))
        {
            return MO_FALSE;
        }

        if (EndAddress)
        {
            *EndAddress = ((MO_CONSTANT_POINTER)(EndAddressValue));
        }
    }

    return MO_TRUE;
}

MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeContains(
    _Mo_In_Opt_ MO_CONSTANT_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Length,
    _Mo_In_Opt_ MO_CONSTANT_POINTER TargetBaseAddress,
    _Mo_In_ MO_UINTN TargetLength)
{
    MO_CONSTANT_POINTER EndAddress = nullptr;
    MO_CONSTANT_POINTER TargetEndAddress = nullptr;

    if (!MoMileMemoryRangeValidate(
        &EndAddress,
        BaseAddress,
        Length))
    {
        return MO_FALSE;
    }

    if (!MoMileMemoryRangeValidate(
        &TargetEndAddress,
        TargetBaseAddress,
        TargetLength))
    {
        return MO_FALSE;
    }

    {
        MO_UINTN BaseAddressValue = ((MO_UINTN)(BaseAddress));
        MO_UINTN EndAddressValue = ((MO_UINTN)(EndAddress));
        MO_UINTN TargetBaseAddressValue = ((MO_UINTN)(TargetBaseAddress));
        MO_UINTN TargetEndAddressValue = ((MO_UINTN)(TargetEndAddress));

        if (BaseAddressValue <= TargetBaseAddressValue &&
            TargetEndAddressValue <= EndAddressValue)
        {
            return MO_TRUE;
        }
    }

    return MO_FALSE;
}

MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeOverlaps(
    _Mo_In_Opt_ MO_CONSTANT_POINTER LeftBaseAddress,
    _Mo_In_ MO_UINTN LeftLength,
    _Mo_In_Opt_ MO_CONSTANT_POINTER RightBaseAddress,
    _Mo_In_ MO_UINTN RightLength)
{
    MO_CONSTANT_POINTER LeftEndAddress = nullptr;
    MO_CONSTANT_POINTER RightEndAddress = nullptr;

    if (!MoMileMemoryRangeValidate(
        &LeftEndAddress,
        LeftBaseAddress,
        LeftLength))
    {
        return MO_FALSE;
    }

    if (!MoMileMemoryRangeValidate(
        &RightEndAddress,
        RightBaseAddress,
        RightLength))
    {
        return MO_FALSE;
    }

    if (0 == LeftLength || 0 == RightLength)
    {
        return MO_FALSE;
    }

    {
        MO_UINTN LeftBaseAddressValue = ((MO_UINTN)(LeftBaseAddress));
        MO_UINTN LeftEndAddressValue = ((MO_UINTN)(LeftEndAddress));
        MO_UINTN RightBaseAddressValue = ((MO_UINTN)(RightBaseAddress));
        MO_UINTN RightEndAddressValue = ((MO_UINTN)(RightEndAddress));

        if (LeftBaseAddressValue < RightEndAddressValue &&
            RightBaseAddressValue < LeftEndAddressValue)
        {
            return MO_TRUE;
        }
    }

    return MO_FALSE;
}

MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryBlockValidate(
    _Mo_Out_Opt_ PMO_POINTER StartAddress,
    _Mo_In_ PMO_MEMORY_BLOCK MemoryBlock,
    _Mo_In_ MO_UINTN Index,
    _Mo_In_ MO_UINTN Length)
{
    if (!MemoryBlock)
    {
        return MO_FALSE;
    }

    {
        MO_POINTER BaseAddress = MemoryBlock->BaseAddress;
        MO_UINTN Size = MemoryBlock->Size;
        MO_UINTN EndIndex = 0;

        if (!MoMileMemoryRangeValidate(
            nullptr,
            BaseAddress,
            Size))
        {
            return MO_FALSE;
        }

        if (!MoMileFixedIntegerCheckedAddition(
            &EndIndex,
            MO_FALSE,
            Index,
            Length))
        {
            return MO_FALSE;
        }

        if (EndIndex > Size)
        {
            return MO_FALSE;
        }

        if (StartAddress)
        {
            *StartAddress = ((MO_POINTER)(((MO_UINTN)(BaseAddress)) + Index));
        }
    }

    return MO_TRUE;
}
