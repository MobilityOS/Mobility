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

#include "Mobility.Runtime.Core.h"
#include "Mobility.Console.Core.h"

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

/**
 * @brief The interrupt types for x64 architecture.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 3 (3A, 3B, 3C, & 3D): System Programming Guide
 *               Section 6.3.1 External Interrupts
 *                 Table 6-1. Protected-Mode Exceptions and Interrupts
 */
#ifndef MO_RUNTIME_X64_INTERRUPT_TYPES_DEFINED

/**
 * @brief Divide Error (#DE)
 */
#define MO_RUNTIME_X64_INTERRUPT_DIVIDE_ERROR 0
/**
 * @brief Debug Exception (#DB)
 */
#define MO_RUNTIME_X64_INTERRUPT_DEBUG_EXCEPTION 1
/**
 * @brief NMI Interrupt
 */
#define MO_RUNTIME_X64_INTERRUPT_NMI_INTERRUPT 2
/**
 * @brief Breakpoint (#BP)
 */
#define MO_RUNTIME_X64_INTERRUPT_BREAKPOINT 3
/**
 * @brief Overflow (#OF)
 */
#define MO_RUNTIME_X64_INTERRUPT_OVERFLOW 4
/**
 * @brief BOUND Range Exceeded (#BR)
 */
#define MO_RUNTIME_X64_INTERRUPT_BOUND_RANGE_EXCEEDED 5
/**
 * @brief Invalid Opcode (Undefined Opcode) (#UD)
 */
#define MO_RUNTIME_X64_INTERRUPT_INVALID_OPCODE 6
/**
 * @brief Device Not Available (No Math Coprocessor) (#NM)
 */
#define MO_RUNTIME_X64_INTERRUPT_DEVICE_NOT_AVAILABLE 7
/**
 * @brief Double Fault (#DF)
 */
#define MO_RUNTIME_X64_INTERRUPT_DOUBLE_FAULT 8
/**
 * @brief Invalid TSS (#TS)
 */
#define MO_RUNTIME_X64_INTERRUPT_INVALID_TSS 10
/**
 * @brief Segment Not Present (#NP)
 */
#define MO_RUNTIME_X64_INTERRUPT_SEGMENT_NOT_PRESENT 11
/**
 * @brief Stack-Segment Fault (#SS)
 */
#define MO_RUNTIME_X64_INTERRUPT_STACK_SEGMENT_FAULT 12
/**
 * @brief General Protection (#GP)
 */
#define MO_RUNTIME_X64_INTERRUPT_GENERAL_PROTECTION 13
/**
 * @brief Page Fault (#PF)
 */
#define MO_RUNTIME_X64_INTERRUPT_PAGE_FAULT 14
/**
 * @brief x87 FPU Floating-Point Error (Math Fault) (#MF)
 */
#define MO_RUNTIME_X64_INTERRUPT_FPU_FLOATING_POINT_ERROR 16
/**
 * @brief Alignment Check (#AC)
 */
#define MO_RUNTIME_X64_INTERRUPT_ALIGNMENT_CHECK 17
/**
 * @brief Machine Check (#MC)
 */
#define MO_RUNTIME_X64_INTERRUPT_MACHINE_CHECK 18
/**
 * @brief SIMD Floating-Point Exception (#XM)
 */
#define MO_RUNTIME_X64_INTERRUPT_SIMD_FLOATING_POINT_EXCEPTION 19
/**
 * @brief Virtualization Exception (#VE)
 */
#define MO_RUNTIME_X64_INTERRUPT_VIRTUALIZATION_EXCEPTION 20
/**
 * @brief Control Protection Exception (#CP)
 */
#define MO_RUNTIME_X64_INTERRUPT_CONTROL_PROTECTION_EXCEPTION 21

#endif // !MO_RUNTIME_X64_INTERRUPT_TYPES_DEFINED

/**
 * @brief The FXSAVE area for x64 architecture.
 * @remark For more information, see these references:
 *         - Intel(R) 64 and IA-32 Architectures Software Developer's Manual
 *           (December 2023)
 *             Volume 1: Basic Architecture
 *               Section 10.5.1 FXSAVE Area
 *                 Table 10-2.  Format of an FXSAVE Area
 */
typedef struct _MO_RUNTIME_X64_FXSAVE_AREA
{
    MO_UINT16 Fcw;
    MO_UINT16 Fsw;
    MO_UINT8 Ftw;
    MO_UINT8 Reserved0;
    MO_UINT16 Fop;
    MO_UINT64 FpuIp;
    MO_UINT64 FpuDp;
    MO_UINT32 MxCsr;
    MO_UINT32 MxCsrMask;
    MO_UINT8 St0Mm0[10];
    MO_UINT8 Reserved1[6];
    MO_UINT8 St1Mm1[10];
    MO_UINT8 Reserved2[6];
    MO_UINT8 St2Mm2[10];
    MO_UINT8 Reserved3[6];
    MO_UINT8 St3Mm3[10];
    MO_UINT8 Reserved4[6];
    MO_UINT8 St4Mm4[10];
    MO_UINT8 Reserved5[6];
    MO_UINT8 St5Mm5[10];
    MO_UINT8 Reserved6[6];
    MO_UINT8 St6Mm6[10];
    MO_UINT8 Reserved7[6];
    MO_UINT8 St7Mm7[10];
    MO_UINT8 Reserved8[6];
    MO_UINT8 Xmm0[16];
    MO_UINT8 Xmm1[16];
    MO_UINT8 Xmm2[16];
    MO_UINT8 Xmm3[16];
    MO_UINT8 Xmm4[16];
    MO_UINT8 Xmm5[16];
    MO_UINT8 Xmm6[16];
    MO_UINT8 Xmm7[16];
    MO_UINT8 Xmm8[16];
    MO_UINT8 Xmm9[16];
    MO_UINT8 Xmm10[16];
    MO_UINT8 Xmm11[16];
    MO_UINT8 Xmm12[16];
    MO_UINT8 Xmm13[16];
    MO_UINT8 Xmm14[16];
    MO_UINT8 Xmm15[16];
    MO_UINT8 Reserved9[96];
} MO_RUNTIME_X64_FXSAVE_AREA, *PMO_RUNTIME_X64_FXSAVE_AREA;
MO_C_STATIC_ASSERT(sizeof(MO_RUNTIME_X64_FXSAVE_AREA) == 512);

#ifdef _MSC_VER
#if (_MSC_VER >= 1200)
#pragma warning(pop)
#else
#pragma warning(default:4201) // nameless struct/union
#endif
#endif

/**
 * @brief Inserts a PAUSE instruction to improve the performance of spin-wait
 *        loops.
 */
EXTERN_C VOID MOAPI MoPlatformPause();

/**
 * @brief Halts the current processor until the next external interrupt arrives.
 */
EXTERN_C VOID MOAPI MoPlatformHalt();

/**
 * @brief Triggers a breakpoint exception.
 */
EXTERN_C VOID MOAPI MoPlatformDebugBreak();

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

/**
 * @brief Reads the value of the specified Model-Specific Register (MSR).
 * @param Index The index of the MSR to read.
 * @return The value of the specified MSR.
 */
EXTERN_C MO_UINT64 MOAPI MoPlatformReadMsr(
    _In_ MO_UINT32 Index);

/**
 * @brief Writes a value to the specified Model-Specific Register (MSR).
 * @param Index The index of the MSR to write.
 * @param Value The value to write to the MSR.
 */
EXTERN_C VOID MOAPI MoPlatformWriteMsr(
    _In_ MO_UINT32 Index,
    _In_ MO_UINT64 Value);

/**
 * @brief Reads the value of the CR3 register.
 * @return The value of the CR3 register.
 */
EXTERN_C MO_UINT64 MOAPI MoPlatformReadCr3();

/**
 * @brief Writes a value to the CR3 register.
 * @param Value The value to write to the CR3 register.
 */
EXTERN_C VOID MOAPI MoPlatformWriteCr3(
    _In_ MO_UINT64 Value);

/**
 * @brief Loads the Global Descriptor Table (GDT) with the specified descriptor.
 * @param Descriptor A pointer to the pseudo-descriptor that contains the base
 *                   address and limit of the GDT.
 */
EXTERN_C VOID MOAPI MoPlatformLoadGlobalDescriptorTable(
    _In_ PMO_RUNTIME_X64_PSEUDO_DESCRIPTOR Descriptor);

/**
 * @brief Loads the Interrupt Descriptor Table (IDT) with the specified
 *        descriptor.
 * @param Descriptor A pointer to the pseudo-descriptor that contains the base
 *                   address and limit of the IDT.
 */
EXTERN_C VOID MOAPI MoPlatformLoadInterruptDescriptorTable(
    _In_ PMO_RUNTIME_X64_PSEUDO_DESCRIPTOR Descriptor);

/**
 * @brief Reloads the segment selectors for data and code segments.
 * @param DataSelector The data segment selector.
 * @param CodeSelector The code segment selector.
 */
EXTERN_C VOID MOAPI MoPlatformReloadSegmentSelectors(
    MO_UINT16 DataSelector,
    MO_UINT16 CodeSelector);

/**
 * @brief Registers a new Task State Segment (TSS) for task switching.
 * @param TssSelector The TSS segment selector.
 */
EXTERN_C VOID MOAPI MoPlatformLoadTaskRegister(
    MO_UINT16 TssSelector);

/**
 * @brief Switches the current stack to a new stack and jumps to the specified
 *        address.
 * @param StackAddress The address of the new stack to switch to.
 * @param FunctionAddress The address of the function to jump to after switching
 *                        to the new stack.
 */
EXTERN_C VOID MOAPI MoPlatformSwitchToNewStack(
    MO_POINTER StackAddress,
    MO_POINTER FunctionAddress);

/**
 * @brief The interrupt for x64 architecture to be hooked.
 */
typedef MO_INTN MO_RUNTIME_X64_INTERRUPT_TYPE;

/**
 * @brief The interrupt context for x64 architecture.
 */
typedef struct _MO_RUNTIME_X64_INTERRUPT_CONTEXT
{
    MO_UINT64 ExceptionData;
    MO_RUNTIME_X64_FXSAVE_AREA FxSaveState;
    MO_UINT64 Dr0;
    MO_UINT64 Dr1;
    MO_UINT64 Dr2;
    MO_UINT64 Dr3;
    MO_UINT64 Dr6;
    MO_UINT64 Dr7;
    MO_UINT64 Cr0;
    MO_UINT64 Cr1; // Reserved
    MO_UINT64 Cr2;
    MO_UINT64 Cr3;
    MO_UINT64 Cr4;
    MO_UINT64 Cr8;
    MO_UINT64 Rflags;
    MO_UINT64 Ldtr;
    MO_UINT64 Tr;
    MO_UINT64 Gdtr[2];
    MO_UINT64 Idtr[2];
    MO_UINT64 Rip;
    MO_UINT64 Gs;
    MO_UINT64 Fs;
    MO_UINT64 Es;
    MO_UINT64 Ds;
    MO_UINT64 Cs;
    MO_UINT64 Ss;
    MO_UINT64 Rdi;
    MO_UINT64 Rsi;
    MO_UINT64 Rbp;
    MO_UINT64 Rsp;
    MO_UINT64 Rbx;
    MO_UINT64 Rdx;
    MO_UINT64 Rcx;
    MO_UINT64 Rax;
    MO_UINT64 R8;
    MO_UINT64 R9;
    MO_UINT64 R10;
    MO_UINT64 R11;
    MO_UINT64 R12;
    MO_UINT64 R13;
    MO_UINT64 R14;
    MO_UINT64 R15;
} MO_RUNTIME_X64_INTERRUPT_CONTEXT, *PMO_RUNTIME_X64_INTERRUPT_CONTEXT;

/**
 * @brief The prototype for x64 interrupt handler.
 * @param InterruptType The type of the interrupt.
 * @param InterruptContext The context of the interrupt.
 */
typedef VOID(MOAPI* PMO_RUNTIME_X64_INTERRUPT_HANDLER)(
    _In_ MO_RUNTIME_X64_INTERRUPT_TYPE InterruptType,
    _In_ PMO_RUNTIME_X64_INTERRUPT_CONTEXT InterruptContext);

/**
 * @brief The interrupt descriptor table handlers for x64 architecture. Because
 *        each entry must be no more than 8 bytes and must be 8-byte aligned,
 *        which allows the C code to compute the address of each vector as an
 *        array index.
 */
EXTERN_C MO_POINTER MOAPI MoPlatformInterruptDescriptorTableHandler[256];

/**
 * @brief Set the Base value for the specific 64-Bit Segment Descriptor.
 * @param Descriptor The pointer to a 64-Bit Segment Descriptor.
 * @param Base The Base value.
 */
EXTERN_C VOID MOAPI MoPlatformSetSegmentDescriptorBase(
    _Out_ PMO_RUNTIME_X64_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Base);

/**
 * @brief Set the Limit value for the specific 64-Bit Segment Descriptor.
 * @param Descriptor The pointer to a 64-Bit Segment Descriptor.
 * @param Limit The Limit value which should be 20-Bit.
 */
EXTERN_C VOID MOAPI MoPlatformSetSegmentDescriptorLimit(
    _Out_ PMO_RUNTIME_X64_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit);

/**
 * @brief Set the Base value for the specific 64-Bit System Segment Descriptor.
 * @param Descriptor The pointer to a 64-Bit System Segment Descriptor.
 * @param Base The Base value.
 */
EXTERN_C VOID MOAPI MoPlatformSetSystemSegmentDescriptorBase(
    _Out_ PMO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT64 Base);

/**
 * @brief Set the Limit value for the specific 64-Bit System Segment Descriptor.
 * @param Descriptor The pointer to a 64-Bit System Segment Descriptor.
 * @param Limit The Limit value which should be 20-Bit.
 */
EXTERN_C VOID MOAPI MoPlatformSetSystemSegmentDescriptorLimit(
    _Out_ PMO_RUNTIME_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _In_ MO_UINT32 Limit);

/**
 * @brief Set the Offset value for the specific 64-Bit IDT Gate Descriptor.
 * @param Descriptor The pointer to a 64-Bit IDT Gate Descriptor.
 * @param Base The Offset value.
 */
EXTERN_C VOID MOAPI MoPlatformSetIdtGateDescriptorOffset(
    _Out_ PMO_RUNTIME_X64_IDT_GATE_DESCRIPTOR Descriptor,
    _In_ MO_UINT64 Offset);

#endif // !MOBILITY_RUNTIME_X64
