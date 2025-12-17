/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Memory.SmallHeap.h
 * PURPOSE:    Definition for Mobility Memory Small Heap (v1)
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_MEMORY_SMALLHEAP
#define MOBILITY_MEMORY_SMALLHEAP

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

/*
 * Small Heap (v1) Signature: { 'S', 'H', 'v', '1' } or '1vHS' or 0x31764853
 */
#define MO_MEMORY_SMALL_HEAP_SIGNATURE 0x31764853

/*
 * Small Heap (v1) Physical Size: Fixed 64 KiB
 */
#define MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE (64 * 1024)
/*
 * Small Heap (v1) Physical Units: Fixed 8192 Units (8 Bytes Each)
 * Because start address of the user area is aligned with 8 bytes.
 */
#define MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS \
    (MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE >> 3)

/*
 * Small Heap (v1) Header: 8 Bytes (1 Unit)
 * - Signature: 4 Bytes
 * - Allocated Units: 2 Bytes
 * - Hint Unit: 2 Bytes
 */
#define MO_MEMORY_SMALL_HEAP_HEADER_SIZE 8
/*
 * Small Heap (v1) Bitmap: 8192 Units / 8 = 1024 Bytes
 * Each bit represents each unit allocation state.
 */
#define MO_MEMORY_SMALL_HEAP_BITMAP_SIZE \
    (MO_MEMORY_SMALL_HEAP_PHYSICAL_UNITS >> 3)

#define MO_MEMORY_SMALL_HEAP_SERVICE_AREA_SIZE ( \
    MO_MEMORY_SMALL_HEAP_HEADER_SIZE + \
    MO_MEMORY_SMALL_HEAP_BITMAP_SIZE)
#define MO_MEMORY_SMALL_HEAP_SERVICE_AREA_UNITS ( \
    MO_MEMORY_SMALL_HEAP_SERVICE_AREA_SIZE >> 3)

#define MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE ( \
    MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE - \
    MO_MEMORY_SMALL_HEAP_SERVICE_AREA_SIZE)
#define MO_MEMORY_SMALL_HEAP_USER_AREA_UNITS ( \
    MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE >> 3)

/**
 * @brief The header structure for Small Heap (v1).
 */
typedef struct _MO_MEMORY_SMALL_HEAP_HEADER
{
    /**
     * @brief The signature for Small Heap (v1).
     */
    MO_UINT32 Signature;
    /**
     * @brief The number of allocated units in Small Heap (v1).
     */
    MO_UINT16 AllocatedUnits;
    /**
     * @brief The hint unit for next allocation in Small Heap (v1).
     */
    MO_UINT16 HintUnit;
} MO_MEMORY_SMALL_HEAP_HEADER, *PMO_MEMORY_SMALL_HEAP_HEADER;
MO_C_STATIC_ASSERT(sizeof(MO_MEMORY_SMALL_HEAP_HEADER) \
    == MO_MEMORY_SMALL_HEAP_HEADER_SIZE);

/**
 * @brief The structure for Small Heap (v1).
 */
typedef struct _MO_MEMORY_SMALL_HEAP
{
    /* Service Area */

    /**
     * @brief The header for Small Heap (v1).
     */
    MO_MEMORY_SMALL_HEAP_HEADER Header;
    /**
     * @brief The bitmap for Small Heap (v1).
     */
    MO_UINT8 BitMap[MO_MEMORY_SMALL_HEAP_BITMAP_SIZE];

    /* User Area */

    /**
     * @brief The user area for Small Heap (v1).
     */
    MO_UINT8 UserArea[MO_MEMORY_SMALL_HEAP_USER_AREA_SIZE];
} MO_MEMORY_SMALL_HEAP, *PMO_MEMORY_SMALL_HEAP;
MO_C_STATIC_ASSERT(sizeof(MO_MEMORY_SMALL_HEAP) \
    == MO_MEMORY_SMALL_HEAP_PHYSICAL_SIZE);

#define MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE 8

/**
 * @brief The item header structure for each allocation in Small Heap (v1).
 */
typedef struct _MO_MEMORY_SMALL_HEAP_ITEM_HEADER
{
    /**
     * @brief The offset units from the start of the heap header to this item
     *        header.
     */
    MO_UINT16 HeapHeaderOffsetUnits;
    /**
     * @brief The number of allocated units for this allocation.
     */
    MO_UINT16 AllocatedUnits;
    /**
     * @brief The requested size in bytes for this allocation.
     */
    MO_UINT16 RequestedSize;
    /**
     * @brief The checksum for this item header.
     */
    MO_UINT16 Checksum;
} MO_MEMORY_SMALL_HEAP_ITEM_HEADER, *PMO_MEMORY_SMALL_HEAP_ITEM_HEADER;
MO_C_STATIC_ASSERT(sizeof(MO_MEMORY_SMALL_HEAP_ITEM_HEADER) \
    == MO_MEMORY_SMALL_HEAP_ITEM_HEADER_SIZE);

#endif // !MOBILITY_MEMORY_SMALLHEAP
