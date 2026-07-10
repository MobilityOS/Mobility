/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.Memory.Unstaged.h
 * PURPOSE:    Definition for Mobility Unstaged Memory Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_MOBILITY_UTILITIES_MEMORY_UNSTAGED
#define MILE_MOBILITY_UTILITIES_MEMORY_UNSTAGED

#include <Mile.Mobility.Utilities.FixedInteger.h>

/*
 * - All functions in this header file are C functions.
 * - All functions are prefixed with "MoMileMemory".
 * - Only depends on Mile.Mobility.Utilities.FixedInteger.h.
 */

/**
 * @brief The structure for describing a contiguous memory block.
 */
typedef struct _MO_MEMORY_BLOCK
{
    /**
     * @brief The base address of the memory block.
     */
    MO_POINTER BaseAddress;
    /**
     * @brief The size, in bytes, of the memory block.
     */
    MO_UINTN Size;
} MO_MEMORY_BLOCK, *PMO_MEMORY_BLOCK;

#ifndef MO_MEMORY_BLOCK_INITIALIZER
/**
 * @brief Expands to an initializer for a memory block structure.
 * @param BaseAddress The base address of the memory block.
 * @param Size The size, in bytes, of the memory block.
 */
#define MO_MEMORY_BLOCK_INITIALIZER(BaseAddress, Size) \
    { (BaseAddress), (Size) }
#endif // !MO_MEMORY_BLOCK_INITIALIZER

#ifndef MO_MEMORY_BLOCK_EXPAND_ARGUMENTS
/**
 * @brief Expands a memory block structure into its base address and size
 *        arguments.
 * @param MemoryBlock The pointer to the memory block structure.
 */
#define MO_MEMORY_BLOCK_EXPAND_ARGUMENTS(MemoryBlock) \
    (MemoryBlock)->BaseAddress, \
    (MemoryBlock)->Size
#endif // !MO_MEMORY_BLOCK_EXPAND_ARGUMENTS

/**
  * @brief Determines whether a memory range representation is valid.
  * @param EndAddress The optional pointer to the variable that receives the end
  *                   boundary address of the memory range. EndAddress may be
  *                   null. If the return value is MO_FALSE, the value pointed
  *                   to by EndAddress is not modified. The received address is
  *                   the end boundary of the half-open range and is not
  *                   guaranteed to be readable or writable.
  * @param BaseAddress The base address of the memory range. If Length is zero,
  *                    BaseAddress may be null. If Length is nonzero,
  *                    BaseAddress must not be null.
  * @param Length The length, in bytes, of the memory range.
  * @return MO_TRUE if the memory range is a valid half-open range
  *         [BaseAddress, the computed end boundary address), and the address
  *         computation does not wrap around. MO_FALSE otherwise. The boundary
  *         computation is performed by using native-width unsigned address
  *         representations. This function only validates the range
  *         representation; it does not prove that the memory range is actually
  *         readable or writable.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeValidate(
    _Mo_Out_Opt_ PMO_CONSTANT_POINTER EndAddress,
    _Mo_In_Opt_ MO_CONSTANT_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Length);

/**
  * @brief Determines whether a memory range fully contains another memory
  *        range.
  * @param BaseAddress The base address of the memory range. If Length is zero,
  *                    BaseAddress may be null. If Length is nonzero,
  *                    BaseAddress must not be null.
  * @param Length The length, in bytes, of the memory range.
  * @param TargetBaseAddress The base address of the target memory range. If
  *                          TargetLength is zero, TargetBaseAddress may be
  *                          null. If TargetLength is nonzero,
  *                          TargetBaseAddress must not be null.
  * @param TargetLength The length, in bytes, of the target memory range.
  * @return MO_TRUE if both memory ranges are valid and the target memory range
  *         is fully contained in the memory range. MO_FALSE otherwise. The
  *         containment test is based on the native-width unsigned address
  *         representations of the boundary addresses. If TargetLength is zero,
  *         the target memory range is empty and is considered contained if and
  *         only if its boundary address lies within the boundary addresses of
  *         the containing memory range.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeContains(
    _Mo_In_Opt_ MO_CONSTANT_POINTER BaseAddress,
    _Mo_In_ MO_UINTN Length,
    _Mo_In_Opt_ MO_CONSTANT_POINTER TargetBaseAddress,
    _Mo_In_ MO_UINTN TargetLength);

/**
  * @brief Determines whether two memory ranges have a non-empty overlap.
  * @param LeftBaseAddress The base address of the left memory range. If
  *                        LeftLength is zero, LeftBaseAddress may be null. If
  *                        LeftLength is nonzero, LeftBaseAddress must not be
  *                        null.
  * @param LeftLength The length, in bytes, of the left memory range.
  * @param RightBaseAddress The base address of the right memory range. If
  *                         RightLength is zero, RightBaseAddress may be null.
  *                         If RightLength is nonzero, RightBaseAddress must not
  *                         be null.
  * @param RightLength The length, in bytes, of the right memory range.
  * @return MO_TRUE if both memory ranges are valid and have a non-empty
  *         overlap. MO_FALSE otherwise. Empty memory ranges do not overlap any
  *         memory range. The overlap test is based on the native-width unsigned
  *         address representations of the boundary addresses.
  */
MO_EXTERN_C MO_BOOL MOAPI MoMileMemoryRangeOverlaps(
    _Mo_In_Opt_ MO_CONSTANT_POINTER LeftBaseAddress,
    _Mo_In_ MO_UINTN LeftLength,
    _Mo_In_Opt_ MO_CONSTANT_POINTER RightBaseAddress,
    _Mo_In_ MO_UINTN RightLength);

#endif // !MILE_MOBILITY_UTILITIES_MEMORY_UNSTAGED
