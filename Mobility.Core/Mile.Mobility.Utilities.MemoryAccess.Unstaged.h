/*
 * PROJECT:    Mouri Internal Library Essentials
 * FILE:       Mile.Mobility.Utilities.MemoryAccess.Unstaged.h
 * PURPOSE:    Definition for Mobility Unstaged Memory Access Utilities
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED
#define MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED

#include "Mile.Mobility.Portable.Types.h"

/*
 * - All functions in this header file are C functions.
 * - All functions are prefixed with "MoMile".
 * - Only depends on Mile.Mobility.Portable.Types.h.
 */

/**
  * @brief Atomically compares and conditionally exchanges an 8-bit value.
  * @param Destination The pointer to the 8-bit variable to compare and
  *                    conditionally replace. Destination must not be null and
  *                    must be properly aligned for 8-bit interlocked access.
  * @param Exchange The value to store into Destination if the current value is
  *                 equal to Comparand.
  * @param Comparand The value to compare with the current value pointed to by
  *                  Destination.
  * @return The initial value pointed to by Destination. If the return value is
  *         equal to Comparand, the exchange was performed. Otherwise,
  *         Destination was not modified by this operation. The comparison and
  *         exchange are performed atomically with interlocked memory ordering
  *         semantics.
  */
MO_EXTERN_C MO_UINT8 MOAPI MoMileInterlockedCompareExchange8(
    _Mo_InOut_ MO_UINT8 volatile* Destination,
    _Mo_In_ MO_UINT8 Exchange,
    _Mo_In_ MO_UINT8 Comparand);

/**
  * @brief Atomically compares and conditionally exchanges a 16-bit value.
  * @param Destination The pointer to the 16-bit variable to compare and
  *                    conditionally replace. Destination must not be null and
  *                    must be properly aligned for 16-bit interlocked access.
  * @param Exchange The value to store into Destination if the current value is
  *                 equal to Comparand.
  * @param Comparand The value to compare with the current value pointed to by
  *                  Destination.
  * @return The initial value pointed to by Destination. If the return value is
  *         equal to Comparand, the exchange was performed. Otherwise,
  *         Destination was not modified by this operation. The comparison and
  *         exchange are performed atomically with interlocked memory ordering
  *         semantics.
  */
MO_EXTERN_C MO_UINT16 MOAPI MoMileInterlockedCompareExchange16(
    _Mo_InOut_ MO_UINT16 volatile* Destination,
    _Mo_In_ MO_UINT16 Exchange,
    _Mo_In_ MO_UINT16 Comparand);

/**
  * @brief Atomically compares and conditionally exchanges a 32-bit value.
  * @param Destination The pointer to the 32-bit variable to compare and
  *                    conditionally replace. Destination must not be null and
  *                    must be properly aligned for 32-bit interlocked access.
  * @param Exchange The value to store into Destination if the current value is
  *                 equal to Comparand.
  * @param Comparand The value to compare with the current value pointed to by
  *                  Destination.
  * @return The initial value pointed to by Destination. If the return value is
  *         equal to Comparand, the exchange was performed. Otherwise,
  *         Destination was not modified by this operation. The comparison and
  *         exchange are performed atomically with interlocked memory ordering
  *         semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedCompareExchange32(
    _Mo_InOut_ MO_UINT32 volatile* Destination,
    _Mo_In_ MO_UINT32 Exchange,
    _Mo_In_ MO_UINT32 Comparand);

/**
  * @brief Atomically compares and conditionally exchanges a 64-bit value.
  * @param Destination The pointer to the 64-bit variable to compare and
  *                    conditionally replace. Destination must not be null and
  *                    must be properly aligned for 64-bit interlocked access.
  * @param Exchange The value to store into Destination if the current value is
  *                 equal to Comparand.
  * @param Comparand The value to compare with the current value pointed to by
  *                  Destination.
  * @return The initial value pointed to by Destination. If the return value is
  *         equal to Comparand, the exchange was performed. Otherwise,
  *         Destination was not modified by this operation. The comparison and
  *         exchange are performed atomically with interlocked memory ordering
  *         semantics.
  */
MO_EXTERN_C MO_UINT64 MOAPI MoMileInterlockedCompareExchange64(
    _Mo_InOut_ MO_UINT64 volatile* Destination,
    _Mo_In_ MO_UINT64 Exchange,
    _Mo_In_ MO_UINT64 Comparand);

/**
  * @brief Atomically compares and conditionally exchanges a pointer value.
  * @param Destination The pointer to the pointer variable to compare and
  *                    conditionally replace. Destination must not be null and
  *                    must be properly aligned for pointer-sized interlocked
  *                    access.
  * @param Exchange The pointer value to store into Destination if the current
  *                 pointer value is equal to Comparand. Exchange may be null.
  * @param Comparand The pointer value to compare with the current pointer value
  *                  pointed to by Destination. Comparand may be null.
  * @return The initial pointer value pointed to by Destination. If the return
  *         value is equal to Comparand, the exchange was performed. Otherwise,
  *         Destination was not modified by this operation. The comparison and
  *         exchange are performed atomically with interlocked memory ordering
  *         semantics.
  */
MO_EXTERN_C MO_POINTER MOAPI MoMileInterlockedCompareExchangePointer(
    _Mo_InOut_ MO_POINTER volatile* Destination,
    _Mo_In_Opt_ MO_POINTER Exchange,
    _Mo_In_Opt_ MO_POINTER Comparand);

/**
  * @brief Atomically exchanges a 32-bit value.
  * @param Target The pointer to the 32-bit variable to replace. Target must not
  *               be null and must be properly aligned for 32-bit interlocked
  *               access.
  * @param Value The value to store into Target.
  * @return The initial value pointed to by Target. The exchange is performed
  *         atomically with interlocked memory ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedExchange32(
    _Mo_InOut_ MO_UINT32 volatile* Target,
    _Mo_In_ MO_UINT32 Value);

/**
  * @brief Atomically exchanges a pointer value.
  * @param Target The pointer to the pointer variable to replace. Target must
  *               not be null and must be properly aligned for pointer-sized
  *               interlocked access.
  * @param Value The pointer value to store into Target. Value may be null.
  * @return The initial pointer value pointed to by Target. The exchange is
  *         performed atomically with interlocked memory ordering semantics.
  */
MO_EXTERN_C MO_POINTER MOAPI MoMileInterlockedExchangePointer(
    _Mo_InOut_ MO_POINTER volatile* Target,
    _Mo_In_Opt_ MO_POINTER Value);

/**
  * @brief Atomically increments a 32-bit value.
  * @param Addend The pointer to the 32-bit variable to increment. Addend must
  *               not be null and must be properly aligned for 32-bit
  *               interlocked access.
  * @return The resulting value after incrementing Addend by one. The operation
  *         is performed modulo 2 ^ 32 and atomically with interlocked memory
  *         ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedIncrement32(
    _Mo_InOut_ MO_UINT32 volatile* Addend);

/**
  * @brief Atomically decrements a 32-bit value.
  * @param Addend The pointer to the 32-bit variable to decrement. Addend must
  *               not be null and must be properly aligned for 32-bit
  *               interlocked access.
  * @return The resulting value after decrementing Addend by one. The operation
  *         is performed modulo 2 ^ 32 and atomically with interlocked memory
  *         ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedDecrement32(
    _Mo_InOut_ MO_UINT32 volatile* Addend);

/**
  * @brief Atomically performs a bitwise AND operation on a 32-bit value.
  * @param Value The pointer to the 32-bit variable to update. Value must not be
  *              null and must be properly aligned for 32-bit interlocked
  *              access.
  * @param Mask The mask to combine with the current value pointed to by Value.
  * @return The initial value pointed to by Value. The bitwise AND operation is
  *         performed atomically with interlocked memory ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedAnd32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask);

/**
  * @brief Atomically performs a bitwise OR operation on a 32-bit value.
  * @param Value The pointer to the 32-bit variable to update. Value must not be
  *              null and must be properly aligned for 32-bit interlocked
  *              access.
  * @param Mask The mask to combine with the current value pointed to by Value.
  * @return The initial value pointed to by Value. The bitwise OR operation is
  *         performed atomically with interlocked memory ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedOr32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask);

/**
  * @brief Atomically performs a bitwise XOR operation on a 32-bit value.
  * @param Value The pointer to the 32-bit variable to update. Value must not be
  *              null and must be properly aligned for 32-bit interlocked
  *              access.
  * @param Mask The mask to combine with the current value pointed to by Value.
  * @return The initial value pointed to by Value. The bitwise XOR operation is
  *         performed atomically with interlocked memory ordering semantics.
  */
MO_EXTERN_C MO_UINT32 MOAPI MoMileInterlockedXor32(
    _Mo_InOut_ MO_UINT32 volatile* Value,
    _Mo_In_ MO_UINT32 Mask);

#ifdef _MSC_VER
MO_EXTERN_C MO_VOID _ReadWriteBarrier(MO_VOID);
#pragma intrinsic(_ReadWriteBarrier)
#endif /* _MSC_VER */

/**
 * @brief Limits the compiler optimizations that can reorder memory accesses
 *        across the point of the call.
 */
static MO_FORCEINLINE MO_VOID MoMileCompilerBarrier(MO_VOID)
{
#ifdef _MSC_VER /* MSVC */
#ifdef __cplusplus
    ::_ReadWriteBarrier();
#else
    _ReadWriteBarrier();
#endif
#elif defined(__GNUC__) || defined(__clang__) /* GCC and Clang */
    __asm__ __volatile__("" : : : "memory");
#else
#error "[Mile.Mobility.Utilities.MemoryAccess.Unstaged] Unsupported platform."
#endif
}

/**
 * @brief Creates a hardware memory barrier (fence) that prevents the CPU from
 *        re-ordering read and write operations. It may also prevent the
 *        compiler from re-ordering read and write operations.
 */
MO_EXTERN_C MO_VOID MOAPI MoMileMemoryBarrier(MO_VOID);

#endif // !MILE_MOBILITY_UTILITIES_MEMORYACCESS_UNSTAGED
