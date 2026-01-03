/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Runtime.x64.h
 * PURPOSE:    Definition for Mobility Runtime x64 Specific Parts
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#ifndef MOBILITY_RUNTIME_X64
#define MOBILITY_RUNTIME_X64

#define MILE_MOBILITY_ENABLE_MINIMUM_SAL
#include <Mile.Mobility.Portable.Types.h>

#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#pragma warning(push)
#endif
#pragma warning(disable:4201) // nameless struct/union
#endif

/*
 * @remark Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *         (December 2023)
 *           Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *             3.4.5.1 Code- and Data-Segment Descriptor Types
 *               Table 3-1. Code- and Data-Segment Types
 */
#ifndef MO_RUNTIME_X64_SEGMENT_TYPES_DEFINED
#define MO_RUNTIME_X64_SEGMENT_TYPES_DEFINED

#define MO_RUNTIME_X64_SEGMENT_TYPE_DATA_ACCESSED 0x1
#define MO_RUNTIME_X64_SEGMENT_TYPE_DATA_WRITE_ENABLE 0x2
#define MO_RUNTIME_X64_SEGMENT_TYPE_DATA_EXPANSION_DIRECTION 0x4

#define MO_RUNTIME_X64_SEGMENT_TYPE_CODE_ACCESSED 0x1
#define MO_RUNTIME_X64_SEGMENT_TYPE_CODE_READ_ENABLE 0x2
#define MO_RUNTIME_X64_SEGMENT_TYPE_CODE_CONFORMING 0x4

#define MO_RUNTIME_X64_SEGMENT_TYPE_CODE 0x8

#endif // !MO_RUNTIME_X64_SEGMENT_TYPES_DEFINED

/**
 * @brief The definition of 64-Bit Segment Descriptor.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               3.4.5 Segment Descriptors
 */
typedef union _MO_RUNTIME_X64_SEGMENT_DESCRIPTOR
{
    struct
    {
        /**
         * @brief Segment limit field (bits 0-15)
         */
        MO_UINT32 LimitLow : 16;
        /**
         * @brief Base address field (bits 0-15)
         */
        MO_UINT32 BaseLow : 16;
        /**
         * @brief Base address field (bits 16-23)
         */
        MO_UINT32 BaseMiddle : 8;
        /**
         * @brief Type field (MO_RUNTIME_X64_SEGMENT_TYPE_*)
         */
        MO_UINT32 Type : 4;
        /**
         * @brief S (descriptor type) flag (0 = system; 1 = code or data)
         */
        MO_UINT32 S : 1;
        /**
         * @brief DPL (descriptor privilege level) field
         */
        MO_UINT32 DPL : 2;
        /**
         * @brief P (segment-present) flag
         */
        MO_UINT32 P : 1;
        /**
         * @brief Segment limit field (bits 16-19)
         */
        MO_UINT32 LimitHigh : 4;
        /**
         * @brief Available for use by system software
         */
        MO_UINT32 AVL : 1;
        /**
         * @brief L (64-bit code segment) flag (IA-32e mode only)
         */
        MO_UINT32 L : 1;
        /**
         * @brief D/B (default operation size/default stack pointer size and/or
         *        upper bound) flag (0 = 16-bit segment; 1 = 32-bit segment)
         */
        MO_UINT32 DB : 1;
        /**
         * @brief G (granularity) flag
         */
        MO_UINT32 G : 1;
        /**
         * @brief Base address field (bits 24-31)
         */
        MO_UINT32 BaseHigh : 8;
    };
    MO_UINT64 RawData;
} MO_RUNTIME_X64_SEGMENT_DESCRIPTOR, *PMO_RUNTIME_X64_SEGMENT_DESCRIPTOR;

/*
 * @remark Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *         (December 2023)
 *           Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *             Section 3.5 SYSTEM DESCRIPTOR TYPES
 *               Table 3-2. System-Segment and Gate-Descriptor Types
 */
#ifndef MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPES_DEFINED
#define MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPES_DEFINED

#define MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPE_LDT 0x2
#define MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPE_TSS_AVAILABLE 0x9
#define MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPE_TSS_BUSY 0xB

#endif // !MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPES_DEFINED

/**
 * @brief The definition of 64-Bit System Segment Descriptor.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               8.2.3 TSS Descriptor in 64-bit mode
 */
typedef union _MO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR
{
    struct
    {
        /**
         * @brief Segment limit field (bits 0-15)
         */
        MO_UINT32 LimitLow : 16;
        /**
         * @brief Base address field (bits 0-15)
         */
        MO_UINT32 BaseLow : 16;
        /**
         * @brief Base address field (bits 16-23)
         */
        MO_UINT32 BaseMiddleLow : 8;
        /**
         * @brief Type field (MO_RUNTIME_X64_SYSTEM_SEGMENT_TYPE_*)
         */
        MO_UINT32 Type : 4;
        /**
         * @brief Must be zero
         */
        MO_UINT32 Reserved0 : 1;
        /**
         * @brief DPL (descriptor privilege level) field
         */
        MO_UINT32 DPL : 2;
        /**
         * @brief P (segment-present) flag
         */
        MO_UINT32 P : 1;
        /**
         * @brief Segment limit field (bits 16-19)
         */
        MO_UINT32 LimitHigh : 4;
        /**
         * @brief Available for use by system software
         */
        MO_UINT32 AVL : 1;
        /**
         * @brief Must be zero
         */
        MO_UINT32 Reserved1 : 2;
        /**
         * @brief G (granularity) flag
         */
        MO_UINT32 G : 1;
        /**
         * @brief Base address field (bits 24-31)
         */
        MO_UINT32 BaseMiddleHigh : 8;
        /**
         * @brief Base address field (bits 32-63)
         */
        MO_UINT32 BaseHigh : 32;
        /**
         * @brief Must be zero
         */
        MO_UINT32 Reserved2 : 32;
    };
    MO_UINT64 RawData[2];
} MO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR, *PMO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR;

#pragma pack(push, MO_RUNTIME_X64_TASK_STATE_SEGMENT_PRAGMA_PACK, 4)
/**
 * @brief The definition of 64-Bit TSS Descriptor.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               8.7 TASK MANAGEMENT IN 64-BIT MODE
 */
typedef struct _MO_RUNTIME_X64_TASK_STATE_SEGMENT
{
    /**
     * @brief Reserved bits. Set to 0.
     */
    MO_UINT32 Reserved0;
    /**
     * @brief The full 64-bit canonical forms of the stack pointers (RSP) for
     *        privilege levels 0-2.
     */
    MO_UINT64 RSP[3];
    /**
     * @brief Reserved bits. Set to 0.
     */
    MO_UINT64 Reserved1;
    /**
     * @brief The full 64-bit canonical forms of the interrupt stack table (IST)
     *        pointers.
     */
    MO_UINT64 IST[7];
    /**
     * @brief Reserved bits. Set to 0.
     */
    MO_UINT64 Reserved2;
    /**
     * @brief Reserved bits. Set to 0.
     */
    MO_UINT16 Reserved3;
    /**
     * @brief The 16-bit offset to the I/O permission bit map from the 64-bit
     *        TSS base.
     */
    MO_UINT16 IoMapBaseAddress;
} MO_RUNTIME_X64_TASK_STATE_SEGMENT, *PMO_RUNTIME_X64_TASK_STATE_SEGMENT;
#pragma pack(pop, MO_RUNTIME_X64_TASK_STATE_SEGMENT_PRAGMA_PACK)

/*
 * @remark Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *         (December 2023)
 *           Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *             Section 3.5 SYSTEM DESCRIPTOR TYPES
 *               Table 3-2. System-Segment and Gate-Descriptor Types
 */
#ifndef MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPES_DEFINED
#define MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPES_DEFINED

#define MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPE_CALL 0xC
#define MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPE_INTERRUPT 0xE
#define MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPE_TRAP 0xF

#endif // !MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPES_DEFINED

/**
 * @brief The definition of 64-Bit IDT Gate Descriptor.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               Section 6.14.1 64-Bit Mode IDT
 */
typedef union _MO_RUNTIME_X64_IDT_GATE_DESCRIPTOR
{
    struct
    {
        /**
         * @brief Offset to procedure entry point field (bits 0-15)
         */
        MO_UINT16 OffsetLow;
        /**
         * @brief Segment Selector for destination code segment field
         */
        MO_UINT16 Selector;
        /**
         * @brief IST (interrupt stack table) field
         */
        MO_UINT8 IST : 3;
        /**
         * @brief Must be zero
         */
        MO_UINT8 Reserved0 : 5;
        /**
        * @brief Type field (MO_RUNTIME_X64_GATE_DESCRIPTOR_TYPE_*)
        */
        MO_UINT8 Type : 4;
        /**
         * @brief Must be zero
         */
        MO_UINT8 Reserved1 : 1;
        /**
         * @brief DPL (descriptor privilege level) field
         */
        MO_UINT8 DPL : 2;
        /**
         * @brief P (segment-present) flag
         */
        MO_UINT8 P : 1;
        /**
         * @brief Offset to procedure entry point field (bits 16-31)
         */
        MO_UINT16 OffsetMiddle;
        /**
         * @brief Offset to procedure entry point field (bits 32-63)
         */
        MO_UINT32 OffsetHigh;
        /**
         * @brief Must be zero
         */
        MO_UINT32 Reserved2;
    };
    MO_UINT64 RawData[2];
} MO_RUNTIME_X64_IDT_GATE_DESCRIPTOR, *PMO_RUNTIME_X64_IDT_GATE_DESCRIPTOR;

#pragma pack(push, MO_RUNTIME_X64_PSEUDO_DESCRIPTOR_PRAGMA_PACK, 1)
/**
 * @brief The definition of 64-Bit Pseudo Descriptor.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               3.5.1 Segment Descriptor Tables
 *                 Figure 3-11. Pseudo-Descriptor Formats
 */
typedef struct _MO_RUNTIME_X64_PSEUDO_DESCRIPTOR
{
    /**
     * @brief The size of the table in bytes subtracted by 1. This subtraction
     *        occurs because the maximum value of this field is 65535, while the
     *        GDT and IDT an be up to 65536 bytes in length. Further, no GDT or
     *        IDT can have a size of 0 bytes.
     */
    MO_UINT16 Limit;
    /**
     * @brief The 64-bit linear base address of the GDT or IDT (not the physical
     *        address, paging applies).
     */
    MO_UINT64 Base;
} MO_RUNTIME_X64_PSEUDO_DESCRIPTOR, *PMO_RUNTIME_X64_PSEUDO_DESCRIPTOR;
#pragma pack(pop, MO_RUNTIME_X64_PSEUDO_DESCRIPTOR_PRAGMA_PACK)

/**
 * @brief The definition of Page-Directory Entries that maps a page.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               4.5.4 Linear-Address Translation with 4-Level Paging and
 *               5-Level Paging
 */
typedef union _MO_RUNTIME_X64_PAGE_TABLE_ENTRY
{
    struct
    {
        /**
         * @brief Present; must be 1 to map a page
         */
        MO_UINT64 P : 1;
        /**
         * @brief Read/write; if 0, writes may not be allowed to the page
         *        referenced by this entry
         */
        MO_UINT64 RW : 1;
        /**
         * @brief User/supervisor; if 0, user-mode accesses are not allowed to
         *        the page referenced by this entry
         */
        MO_UINT64 US : 1;
        /**
         * @brief Page-level write-through; indirectly determines the memory
         *        type used to access the page referenced by this entry 
         */
        MO_UINT64 PWT : 1;
        /**
         * @brief Page-level cache disable; indirectly determines the memory
         *        type used to access the page referenced by this entry
         */
        MO_UINT64 PCD : 1;
        /**
         * @brief Accessed; indicates whether software has accessed the page
         *        referenced by this entry
         */
        MO_UINT64 A : 1;
        /**
         * @brief Dirty; indicates whether software has written to the page
         *        referenced by this entry
         */
        MO_UINT64 D : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved0 : 1;
        /**
         * @brief Global; if CR4.PGE= 1, determines whether the translation is
         *        global; ignored otherwise
         */
        MO_UINT64 G : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved1 : 2;
        /**
         * @brief For ordinary paging, ignored; for HLAT paging, restart (if 1,
         *        linear-address translation is restarted with ordinary paging)
         */
        MO_UINT64 R : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved2 : 40;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved3 : 7;
        /**
         * @brief Protection key; if CR4.PKE= 1 or CR4.PKS= 1, this may control
         *        the page's access rights (see Section 4.6.2); otherwise, it is
         *        ignored and not used to control access rights.
         */
        MO_UINT64 ProtectionKey : 4;
        /**
         * @brief If IA32_EFER.NXE= 1, execute-disable (if 1, instruction
         *        fetches are not allowed from the page controlled by this
         *        entry); otherwise, reserved (must be 0)
         */
        MO_UINT64 XD : 1;
    };
    struct
    {
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_4K_0 : 7;
        /**
         * @brief Indirectly determines the memory type used to access the
         *        4-KByte page referenced by this entry
         */
        MO_UINT64 PAT_4K : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_4K_1 : 4;
        /**
         * @brief Physical address of the 4-KByte page referenced by this entry
         */
        MO_UINT64 PageFrameNumber_4K : 40;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_4K_2 : 12;
    };
    struct
    {
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_2M_0 : 7;
        /**
         * @brief Page size; must be 1 (otherwise, this entry references a page
         *        table)
         */
        MO_UINT64 PS_2M : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_2M_1 : 4;
        /**
         * @brief Indirectly determines the memory type used to access the
         *        2-MByte page referenced by this entry
         */
        MO_UINT64 PAT_2M : 1;
        /**
         * @brief Reserved (must be 0)
         */
        MO_UINT64 Reserved_2M_2 : 8;
        /**
         * @brief Physical address of the 2-MByte page referenced by this entry
         */
        MO_UINT64 PageFrameNumber_2M : 31;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_2M_3 : 12;
    };
    struct
    {
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_1G_0 : 7;
        /**
         * @brief Page size; must be 1 (otherwise, this entry references a page
         *        directory)
         */
        MO_UINT64 PS_1G : 1;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_1G_1 : 4;
        /**
         * @brief Indirectly determines the memory type used to access the
         *        1-GByte page referenced by this entry
         */
        MO_UINT64 PAT_1G : 1;
        /**
         * @brief Reserved (must be 0)
         */
        MO_UINT64 Reserved_1G_2 : 17;
        /**
         * @brief Physical address of the 1-GByte page referenced by this entry
         */
        MO_UINT64 PageFrameNumber_1G : 22;
        /**
         * @brief Should be determined by other structs in this union
         */
        MO_UINT64 Reserved_1G_3 : 12;
    };
    MO_UINT64 RawData;
} MO_RUNTIME_X64_PAGE_TABLE_ENTRY, *PMO_RUNTIME_X64_PAGE_TABLE_ENTRY;

/**
 * @brief The definition of Page-Directory Entry that maps a page table.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               4.5.4 Linear-Address Translation with 4-Level Paging and
 *               5-Level Paging
 */
typedef union _MO_RUNTIME_X64_PAGE_DIRECTORY_ENTRY
{
    struct
    {
        /**
         * @brief Present; must be 1 to reference a page table
         */
        MO_UINT64 P : 1;
        /**
         * @brief Read/write; if 0, writes may not be allowed to the region
         *        controlled by this entry
         */
        MO_UINT64 RW : 1;
        /**
         * @brief User/supervisor; if 0, user-mode accesses are not allowed to
         *        the region controlled by this entry
         */
        MO_UINT64 US : 1;
        /**
         * @brief Page-level write-through; indirectly determines the memory
         *        type used to access the page table referenced by this entry
         */
        MO_UINT64 PWT : 1;
        /**
         * @brief Page-level cache disable; indirectly determines the memory
         *        type used to access the page table referenced by this entry
         */
        MO_UINT64 PCD : 1;
        /**
         * @brief Accessed; indicates whether this entry has been used for
         *        linear-address translation
         */
        MO_UINT64 A : 1;
        /**
         * @brief Reserved (must be 0)
         */
        MO_UINT64 Reserved0 : 5;
        /**
         * @brief For ordinary paging, ignored; for HLAT paging, restart (if 1,
         *        linear-address translation is restarted with ordinary paging)
         */
        MO_UINT64 R : 1;
        /**
         * @brief Physical address of 4-KByte aligned page table or directory
         *        referenced by this entry
         */
        MO_UINT64 PageTableAddress : 40;
        /**
         * @brief Reserved (must be 0)
         */
        MO_UINT64 Reserved2 : 11;
        /**
         * @brief If IA32_EFER.NXE= 1, execute-disable (if 1, instruction
         *        fetches are not allowed from the region controlled by this
         *        entry); otherwise, reserved (must be 0)
         */
        MO_UINT64 XD : 1;
    };
    MO_UINT64 RawData;
} MO_RUNTIME_X64_PAGE_DIRECTORY_ENTRY, *PMO_RUNTIME_X64_PAGE_DIRECTORY_ENTRY;

#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#pragma warning(pop)
#else
#pragma warning(default:4201) // nameless struct/union
#endif
#endif

/**
 * @brief Disables interrupts on the current processor.
 * @remarks Implemented in the assembly parts.
 */
EXTERN_C VOID MOAPI MoPlatformDisableInterrupts();

/**
 * @brief Enables interrupts on the current processor.
 * @remarks Implemented in the assembly parts.
 */
EXTERN_C VOID MOAPI MoPlatformEnableInterrupts();

#endif // !MOBILITY_RUNTIME_X64

