#define _CRT_SECURE_NO_WARNINGS

#pragma warning(disable:4018)
#pragma warning(disable:4146)
#pragma warning(disable:4244)
#pragma warning(disable:4389)

// 8086tiny: a tiny, highly functional, highly portable PC emulator/VM
// Copyright 2013-14, Adrian Cable (adrian.cable@gmail.com)
// - http://www.megalith.co.uk/8086tiny
//
// Revision 1.25
//
// This work is licensed under the MIT License. See included LICENSE.TXT.

#include <Mile.Mobility.Portable.Types.h>
#include <Mobility.Display.Core.h>

MO_EXTERN_C MO_VOID MOAPI MoRetroVPcSpeakerSetTone(
    _Mo_In_ MO_BOOL Enabled,
    _Mo_In_ MO_UINT16 PitDivisor);

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayCreate(
    _Mo_In_ MO_UINT32 HorizontalResolution,
    _Mo_In_ MO_UINT32 VerticalResolution,
    _Mo_Out_ PMO_DISPLAY_BGRA32_FRAMEBUFFER FrameBuffer);

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayPresent(MO_VOID);

MO_EXTERN_C MO_BOOL MOAPI MoRetroVDisplayReadKey(
    _Mo_Out_ PMO_UINT16 KeyEvent);

MO_EXTERN_C MO_VOID MOAPI MoRetroVDisplayDestroy(MO_VOID);

#include <stdint.h>
#include <time.h>
#include <sys/timeb.h>
#include <string.h>

#include <conio.h>
#include <fcntl.h>
#include <io.h>

// Emulator system constants
#define IO_PORT_COUNT 0x10000
#define RAM_SIZE 0x10FFF0
#define REGS_BASE 0xF0000
#define VIDEO_RAM_SIZE 0x10000

// Graphics/timer/keyboard update delays (explained later)
#ifndef GRAPHICS_UPDATE_DELAY
#define GRAPHICS_UPDATE_DELAY 360000
#endif
#define KEYBOARD_TIMER_UPDATE_DELAY 20000

// 16-bit register decodes
#define REG_AX 0
#define REG_CX 1
#define REG_DX 2
#define REG_BX 3
#define REG_SP 4
#define REG_BP 5
#define REG_SI 6
#define REG_DI 7

#define REG_ES 8
#define REG_CS 9
#define REG_SS 10
#define REG_DS 11

#define REG_ZERO 12
#define REG_SCRATCH 13

// 8-bit register decodes
#define REG_AL 0
#define REG_AH 1
#define REG_CL 2
#define REG_CH 3
#define REG_DL 4
#define REG_DH 5
#define REG_BL 6
#define REG_BH 7

// FLAGS register decodes
#define FLAG_CF 40
#define FLAG_PF 41
#define FLAG_AF 42
#define FLAG_ZF 43
#define FLAG_SF 44
#define FLAG_TF 45
#define FLAG_IF 46
#define FLAG_DF 47
#define FLAG_OF 48

// Lookup tables in the BIOS binary
#define TABLE_XLAT_OPCODE 8
#define TABLE_XLAT_SUBFUNCTION 9
#define TABLE_STD_FLAGS 10
#define TABLE_PARITY_FLAG 11
#define TABLE_BASE_INST_SIZE 12
#define TABLE_I_W_SIZE 13
#define TABLE_I_MOD_SIZE 14
#define TABLE_COND_JUMP_DECODE_A 15
#define TABLE_COND_JUMP_DECODE_B 16
#define TABLE_COND_JUMP_DECODE_C 17
#define TABLE_COND_JUMP_DECODE_D 18
#define TABLE_FLAGS_BITFIELDS 19

// Bitfields for TABLE_STD_FLAGS values
#define FLAGS_UPDATE_SZP 1
#define FLAGS_UPDATE_AO_ARITH 2
#define FLAGS_UPDATE_OC_LOGIC 4

// Helper macros

// Decode mod, r_m and reg fields in instruction
#define DECODE_RM_REG \
    scratch2_uint = 4 * !i_mod, \
    op_to_addr = rm_addr = i_mod < 3 \
        ? SEGREG( \
            seg_override_en \
                ? seg_override \
                : bios_table_lookup[scratch2_uint + 3][i_rm], \
            bios_table_lookup[scratch2_uint][i_rm], \
            regs16[bios_table_lookup[scratch2_uint + 1][i_rm]] + \
                bios_table_lookup[scratch2_uint + 2][i_rm] * i_data1 +) \
        : GET_REG_ADDR(i_rm), \
    op_from_addr = GET_REG_ADDR(i_reg), \
    i_d && ( \
        scratch_uint = op_from_addr, \
        op_from_addr = rm_addr, \
        op_to_addr = scratch_uint)

// Return memory-mapped register location (offset into mem array) for register
// #reg_id
#define GET_REG_ADDR(reg_id) \
    (REGS_BASE + (i_w \
        ? 2 * reg_id \
        : 2 * reg_id + reg_id / 4 & 7))

// Returns number of top bit in operand (i.e. 8 for 8-bit operands, 16 for
// 16-bit operands)
#define TOP_BIT 8*(i_w + 1)

// Opcode execution unit helpers
#define OPCODE ;break; case
#define OPCODE_CHAIN ; case

// [I]MUL/[I]DIV/DAA/DAS/ADC/SBB helpers
#define MUL_MACRO(op_data_type, out_regs) \
    (set_opcode(0x10), \
        out_regs[i_w + 1] = ( \
            op_result = CAST(op_data_type)mem[rm_addr] * \
                (op_data_type)*out_regs) >> 16, \
        regs16[REG_AX] = op_result, \
        set_OF(set_CF(op_result - (op_data_type)op_result)))

#define DIV_MACRO(out_data_type, in_data_type, out_regs) \
    (scratch_int = CAST(out_data_type)mem[rm_addr]) && \
        !( \
            scratch2_uint = (in_data_type)(scratch_uint = \
                (out_regs[i_w + 1] << 16) + regs16[REG_AX]) / scratch_int, \
            scratch2_uint - (out_data_type)scratch2_uint) \
        ? out_regs[i_w + 1] = \
            scratch_uint - scratch_int * (*out_regs = scratch2_uint) \
        : pc_interrupt(0)

#define DAA_DAS(op1, op2, mask, min) \
    set_AF( \
        (((scratch2_uint = regs8[REG_AL]) & 0x0F) > 9) || \
        regs8[FLAG_AF]) && \
        (op_result = regs8[REG_AL] op1 6, \
            set_CF( \
                regs8[FLAG_CF] || \
                (regs8[REG_AL] op2 scratch2_uint))), \
    set_CF( \
        (((mask & 1 ? scratch2_uint : regs8[REG_AL]) & mask) > min) || \
        regs8[FLAG_CF]) && \
        (op_result = regs8[REG_AL] op1 0x60)

#define ADC_SBB_MACRO(a) \
    OP(a##= regs8[FLAG_CF] +), \
    set_CF( \
        regs8[FLAG_CF] && (op_result == op_dest) || \
        (a op_result < a(int32_t)op_dest)), \
    set_AF_OF_arith()

// Execute arithmetic/logic operations in emulator memory/registers
#define R_M_OP(dest, op, src) \
    (i_w \
        ? op_dest = CAST(uint16_t)dest, \
            op_result = CAST(uint16_t)dest op \
                (op_source = CAST(uint16_t)src) \
        : (op_dest = dest, \
            op_result = dest op (op_source = CAST(uint8_t)src)))

#define MEM_OP(dest, op, src) R_M_OP(mem[dest], op, mem[src])
#define OP(op) MEM_OP(op_to_addr, op, op_from_addr)

// Increment or decrement a register #reg_id (usually SI or DI), depending on
// direction flag and operand size (given by i_w)
#define INDEX_INC(reg_id) \
    (regs16[reg_id] -= (2 * regs8[FLAG_DF] - 1) * (i_w + 1))

// Helpers for stack operations
#define R_M_PUSH(a) \
    (i_w = 1, \
        R_M_OP(mem[SEGREG(REG_SS, REG_SP, --)], =, a))

#define R_M_POP(a) \
    (i_w = 1, \
        regs16[REG_SP] += 2, \
        R_M_OP(a, =, mem[SEGREG(REG_SS, REG_SP, -2 +)]))

// Convert segment:offset to linear address in emulator memory space
#define SEGREG(reg_seg, reg_ofs, op) \
    16 * regs16[reg_seg] + (uint16_t)(op regs16[reg_ofs])

// Returns sign bit of an 8-bit or 16-bit operand
#define SIGN_OF(a) (1 & (i_w ? CAST(int16_t)a : a) >> (TOP_BIT - 1))

// Reinterpretation cast
#define CAST(a) *(a*)&

// Global variable definitions
uint8_t mem[RAM_SIZE];
uint8_t io_ports[IO_PORT_COUNT];
uint8_t* opcode_stream;
uint8_t* regs8;

uint8_t i_rm;
uint8_t i_w;
uint8_t i_reg;
uint8_t i_mod;
uint8_t i_mod_size;
uint8_t i_d;
uint8_t i_reg4bit;
uint8_t raw_opcode_id;
uint8_t xlat_opcode_id;
uint8_t extra;
uint8_t rep_mode;
uint8_t seg_override_en;
uint8_t rep_override_en;
uint8_t trap_flag;
uint8_t int8_asap;
uint8_t scratch_uchar;
uint8_t io_hi_lo;
uint8_t pc_speaker_mode_enabled;

uint8_t* vid_mem_base;
uint8_t bios_table_lookup[20][256];

uint16_t* regs16;
uint16_t reg_ip;
uint16_t seg_override;
uint16_t file_index;

uint32_t op_source;
uint32_t op_dest;
uint32_t rm_addr;
uint32_t op_to_addr;
uint32_t op_from_addr;
uint32_t i_data0;
uint32_t i_data1;
uint32_t i_data2;
uint32_t scratch_uint;
uint32_t scratch2_uint;
uint32_t inst_counter;
uint32_t set_flags_type;
uint32_t GRAPHICS_X;
uint32_t GRAPHICS_Y;
uint32_t pixel_colors[16];
uint32_t rgb332_colors[256];
uint32_t vmem_ctr;

int32_t op_result;
int disk[3];
int32_t scratch_int;

MO_DISPLAY_BGRA32_FRAMEBUFFER display_frame_buffer;

time_t clock_buf;
struct timeb ms_clock;

uint16_t vid_addr_lookup[VIDEO_RAM_SIZE];

uint16_t cga_colors[4] =
{
    0, /* Black */
    0x1F1F, /* Cyan */
    0xE3E3, /* Magenta */
    0xFFFF /* White */
};

// Helper functions

// Set carry flag
uint8_t set_CF(int32_t new_CF)
{
    return regs8[FLAG_CF] = !!new_CF;
}

// Set auxiliary flag
uint8_t set_AF(int32_t new_AF)
{
    return regs8[FLAG_AF] = !!new_AF;
}

// Set overflow flag
uint8_t set_OF(int32_t new_OF)
{
    return regs8[FLAG_OF] = !!new_OF;
}

// Set auxiliary and overflow flag after arithmetic operations
uint8_t set_AF_OF_arith(void)
{
    set_AF((op_source ^= op_dest ^ op_result) & 0x10);
    if (op_result == op_dest)
        return set_OF(0);
    else
        return set_OF(1 & (regs8[FLAG_CF] ^ op_source >> (TOP_BIT - 1)));
}

// Assemble and return emulated CPU FLAGS register in scratch_uint
void make_flags(void)
{
    scratch_uint = 0xF002; // 8086 has reserved and unused flags set to 1
    for (int i = 9; i--;)
        scratch_uint +=
            regs8[FLAG_CF + i] << bios_table_lookup[TABLE_FLAGS_BITFIELDS][i];
}

// Set emulated CPU FLAGS register from regs8[FLAG_xx] values
void set_flags(int32_t new_flags)
{
    for (int i = 9; i--;)
        regs8[FLAG_CF + i] =
            !!(1 << bios_table_lookup[TABLE_FLAGS_BITFIELDS][i] & new_flags);
}

// Convert raw opcode to translated opcode index. This condenses a large number
// of different encodings of similar instructions into a much smaller number of
// distinct functions, which we then execute
void set_opcode(uint8_t opcode)
{
    xlat_opcode_id =
        bios_table_lookup[TABLE_XLAT_OPCODE][raw_opcode_id = opcode];
    extra = bios_table_lookup[TABLE_XLAT_SUBFUNCTION][opcode];
    i_mod_size = bios_table_lookup[TABLE_I_MOD_SIZE][opcode];
    set_flags_type = bios_table_lookup[TABLE_STD_FLAGS][opcode];
}

// Execute INT #interrupt_num on the emulated machine
uint8_t pc_interrupt(uint8_t interrupt_num)
{
    set_opcode(0xCD); // Decode like INT

    make_flags();
    R_M_PUSH(scratch_uint);
    R_M_PUSH(regs16[REG_CS]);
    R_M_PUSH(reg_ip);
    MEM_OP(REGS_BASE + 2 * REG_CS, =, 4 * interrupt_num + 2);
    R_M_OP(reg_ip, =, mem[4 * interrupt_num]);

    return regs8[FLAG_TF] = regs8[FLAG_IF] = 0;
}

// AAA and AAS instructions - which_operation is +1 for AAA, and -1 for AAS
int32_t AAA_AAS(int8_t which_operation)
{
    return (
        regs16[REG_AX] += 262 * which_operation *
            set_AF(set_CF(((regs8[REG_AL] & 0x0F) > 9) || regs8[FLAG_AF])),
        regs8[REG_AL] &= 0x0F);
}

static void update_pc_speaker(void)
{
    MO_BOOL enabled = pc_speaker_mode_enabled && ((io_ports[0x61] & 3) == 3)
        ? MO_TRUE
        : MO_FALSE;

    MoRetroVPcSpeakerSetTone(enabled, CAST(uint16_t)mem[0x4AA]);
}

// Emulator entry point
int main_8086tiny(int argc, char **argv)
{
    // regs16 and reg8 point to F000:0, the start of memory-mapped registers.
    // CS is initialised to F000
    regs16 = (uint16_t*)(regs8 = mem + REGS_BASE);
    regs16[REG_CS] = 0xF000;

    // Trap flag off
    regs8[FLAG_TF] = 0;

    // Set DL equal to the boot device: 0 for the FD, or 0x80 for the HD.
    // Normally, boot from the FD.
    // But, if the HD image file is prefixed with @, then boot from the HD
    regs8[REG_DL] = ((argc > 3) && (*argv[3] == '@'))
        ? argv[3]++, 0x80
        : 0;

    // Open BIOS (file id disk[2]), floppy disk image (disk[1]), and hard disk
    // image (disk[0]) if specified
    for (file_index = 3; file_index;)
        disk[--file_index] = *++argv ? _open(*argv, 32898) : 0;

    // Set CX:AX equal to the hard disk image size, if present
    CAST(uint32_t)regs16[REG_AX] = *disk ? _lseek(*disk, 0, 2) >> 9 : 0;

    // Load BIOS image into F000:0100, and set IP to 0100
    _read(disk[2], regs8 + (reg_ip = 0x100), 0xFF00);

    // Load instruction decoding helper table
    for (int i = 0; i < 20; i++)
        for (int j = 0; j < 256; j++)
            bios_table_lookup[i][j] = regs8[regs16[0x81 + i] + j];

    // Instruction execution loop. Terminates if CS:IP = 0:0
    for (;
        opcode_stream = mem + 16 * regs16[REG_CS] + reg_ip,
        opcode_stream != mem;)
    {
        // Set up variables to prepare for decoding an opcode
        set_opcode(*opcode_stream);

        // Extract i_w and i_d fields from instruction
        i_w = (i_reg4bit = raw_opcode_id & 7) & 1;
        i_d = i_reg4bit / 2 & 1;

        // Extract instruction data fields
        i_data0 = CAST(int16_t)opcode_stream[1];
        i_data1 = CAST(int16_t)opcode_stream[2];
        i_data2 = CAST(int16_t)opcode_stream[3];

        // seg_override_en and rep_override_en contain number of instructions to
        // hold segment override and REP prefix respectively
        if (seg_override_en)
            seg_override_en--;
        if (rep_override_en)
            rep_override_en--;

        // i_mod_size > 0 indicates that opcode uses i_mod/i_rm/i_reg, so decode
        // them
        if (i_mod_size)
        {
            i_mod = (i_data0 & 0xFF) >> 6;
            i_rm = i_data0 & 7;
            i_reg = i_data0 / 8 & 7;

            if ((!i_mod && i_rm == 6) || (i_mod == 2))
                i_data2 = CAST(int16_t)opcode_stream[4];
            else if (i_mod != 1)
                i_data2 = i_data1;
            else
                // If i_mod is 1, operand is (usually) 8 bits rather than 16
                // bits
                i_data1 = (int8_t)i_data1;

            DECODE_RM_REG;
        }

        // Instruction execution unit
        switch (xlat_opcode_id)
        {
            OPCODE_CHAIN 0: // Conditional jump (JAE, JNAE, etc.)
                // i_w is the invert flag, e.g. i_w == 1 means JNAE, whereas
                // i_w == 0 means JAE
                scratch_uchar = raw_opcode_id / 2 & 7;
                reg_ip += (int8_t)i_data0 * (
                    i_w ^ (
                        regs8[bios_table_lookup[
                            TABLE_COND_JUMP_DECODE_A][scratch_uchar]] ||
                        regs8[bios_table_lookup[
                            TABLE_COND_JUMP_DECODE_B][scratch_uchar]] ||
                        regs8[bios_table_lookup[
                            TABLE_COND_JUMP_DECODE_C][scratch_uchar]] ^
                        regs8[bios_table_lookup[
                            TABLE_COND_JUMP_DECODE_D][scratch_uchar]]))
            OPCODE 1: // MOV reg, imm
                i_w = !!(raw_opcode_id & 8);
                R_M_OP(mem[GET_REG_ADDR(i_reg4bit)], =, i_data0)
            OPCODE 3: // PUSH regs16
                R_M_PUSH(regs16[i_reg4bit])
            OPCODE 4: // POP regs16
                R_M_POP(regs16[i_reg4bit])
            OPCODE 2: // INC|DEC regs16
                i_w = 1;
                i_d = 0;
                i_reg = i_reg4bit;
                DECODE_RM_REG;
                i_reg = extra
            OPCODE_CHAIN 5: // INC|DEC|JMP|CALL|PUSH
                if (i_reg < 2) // INC|DEC
                    MEM_OP(
                        op_from_addr,
                        += 1 - 2 * i_reg +,
                        REGS_BASE + 2 * REG_ZERO),
                    op_source = 1,
                    set_AF_OF_arith(),
                    set_OF(op_dest + 1 - i_reg == 1 << (TOP_BIT - 1)),
                    (xlat_opcode_id == 5) &&
                        (set_opcode(0x10), 0); // Decode like ADC
                else if (i_reg != 6) // JMP|CALL
                    i_reg - 3 || R_M_PUSH(regs16[REG_CS]), // CALL (far)
                    i_reg & 2 && R_M_PUSH(
                        reg_ip + 2 +
                        i_mod * (i_mod != 3) +
                        2 * (!i_mod && i_rm == 6)), // CALL (near or far)
                    i_reg & 1 && (
                        regs16[REG_CS] = CAST(int16_t)mem[
                            op_from_addr + 2]), // JMP|CALL (far)
                    R_M_OP(reg_ip, =, mem[op_from_addr]),
                    set_opcode(0x9A); // Decode like CALL
                else // PUSH
                    R_M_PUSH(mem[rm_addr])
            OPCODE 6: // TEST r/m, imm16 / NOT|NEG|MUL|IMUL|DIV|IDIV reg
                op_to_addr = op_from_addr;

                switch (i_reg)
                {
                    OPCODE_CHAIN 0: // TEST
                        set_opcode(0x20); // Decode like AND
                        reg_ip += i_w + 1;
                        R_M_OP(mem[op_to_addr], &, i_data2)
                    OPCODE 2: // NOT
                        OP(=~)
                    OPCODE 3: // NEG
                        OP(=-);
                        op_dest = 0;
                        set_opcode(0x28); // Decode like SUB
                        set_CF(op_result > op_dest)
                    OPCODE 4: // MUL
                        i_w
                            ? MUL_MACRO(uint16_t, regs16)
                            : MUL_MACRO(uint8_t, regs8)
                    OPCODE 5: // IMUL
                        i_w
                            ? MUL_MACRO(int16_t, regs16)
                            : MUL_MACRO(int8_t, regs8)
                    OPCODE 6: // DIV
                        i_w
                            ? DIV_MACRO(uint16_t, uint32_t, regs16)
                            : DIV_MACRO(uint8_t, uint16_t, regs8)
                    OPCODE 7: // IDIV
                        i_w
                            ? DIV_MACRO(int16_t, int32_t, regs16)
                            : DIV_MACRO(int8_t, int16_t, regs8);
                }
            OPCODE 7: // ADD|OR|ADC|SBB|AND|SUB|XOR|CMP AL/AX, immed
                rm_addr = REGS_BASE;
                i_data2 = i_data0;
                i_mod = 3;
                i_reg = extra;
                reg_ip--;
            OPCODE_CHAIN 8: // ADD|OR|ADC|SBB|AND|SUB|XOR|CMP reg, immed
                op_to_addr = rm_addr;
                regs16[REG_SCRATCH] = (i_d |= !i_w) ? (int8_t)i_data2 : i_data2;
                op_from_addr = REGS_BASE + 2 * REG_SCRATCH;
                reg_ip += !i_d + 1;
                set_opcode(0x08 * (extra = i_reg));
            OPCODE_CHAIN 9: // ADD|OR|ADC|SBB|AND|SUB|XOR|CMP|MOV reg, r/m
                switch (extra)
                {
                    OPCODE_CHAIN 0: // ADD
                        OP(+=),
                        set_CF(op_result < op_dest)
                    OPCODE 1: // OR
                        OP(|=)
                    OPCODE 2: // ADC
                        ADC_SBB_MACRO(+)
                    OPCODE 3: // SBB
                        ADC_SBB_MACRO(-)
                    OPCODE 4: // AND
                        OP(&=)
                    OPCODE 5: // SUB
                        OP(-=),
                        set_CF(op_result > op_dest)
                    OPCODE 6: // XOR
                        OP(^=)
                    OPCODE 7: // CMP
                        OP(-),
                        set_CF(op_result > op_dest)
                    OPCODE 8: // MOV
                        OP(=);
                }
            OPCODE 10: // MOV sreg, r/m | POP r/m | LEA reg, r/m
                if (!i_w) // MOV
                    i_w = 1,
                    i_reg += 8,
                    DECODE_RM_REG,
                    OP(=);
                else if (!i_d) // LEA
                    seg_override_en = 1,
                    seg_override = REG_ZERO,
                    DECODE_RM_REG,
                    R_M_OP(mem[op_from_addr], =, rm_addr);
                else // POP
                    R_M_POP(mem[rm_addr])
            OPCODE 11: // MOV AL/AX, [loc]
                i_mod = i_reg = 0;
                i_rm = 6;
                i_data1 = i_data0;
                DECODE_RM_REG;
                MEM_OP(op_from_addr, =, op_to_addr)
            OPCODE 12:
                // ROL|ROR|RCL|RCR|SHL|SHR|???|SAR reg/mem, 1/CL/imm (80186)
                scratch2_uint = SIGN_OF(mem[rm_addr]),
                scratch_uint = extra ? // xxx reg/mem, imm
                    ++reg_ip,
                    (int8_t)i_data1
                : // xxx reg/mem, CL
                    i_d
                        ? 31 & regs8[REG_CL]
                : // xxx reg/mem, 1
                    1;
                if (scratch_uint)
                {
                    if (i_reg < 4) // Rotate operations
                        scratch_uint %= i_reg / 2 + TOP_BIT,
                        R_M_OP(scratch2_uint, =, mem[rm_addr]);
                    if (i_reg & 1) // Rotate/shift right operations
                        R_M_OP(mem[rm_addr], >>=, scratch_uint);
                    else // Rotate/shift left operations
                        R_M_OP(mem[rm_addr], <<=, scratch_uint);
                    if (i_reg > 3) // Shift operations
                        set_opcode(0x10); // Decode like ADC
                    if (i_reg > 4) // SHR or SAR
                        set_CF(op_dest >> (scratch_uint - 1) & 1);
                }

                switch (i_reg)
                {
                    OPCODE_CHAIN 0: // ROL
                        R_M_OP(
                            mem[rm_addr],
                            += ,
                            scratch2_uint >> (TOP_BIT - scratch_uint));
                        set_OF(SIGN_OF(op_result) ^ set_CF(op_result & 1))
                    OPCODE 1: // ROR
                        scratch2_uint &= (1 << scratch_uint) - 1,
                        R_M_OP(
                            mem[rm_addr],
                            += ,
                            scratch2_uint << (TOP_BIT - scratch_uint));
                        set_OF(
                            SIGN_OF(op_result * 2) ^ set_CF(SIGN_OF(op_result)))
                    OPCODE 2: // RCL
                        R_M_OP(
                            mem[rm_addr],
                            += (regs8[FLAG_CF] << (scratch_uint - 1)) + ,
                            scratch2_uint >> (1 + TOP_BIT - scratch_uint));
                        set_OF(
                            SIGN_OF(op_result) ^ set_CF(
                                scratch2_uint & 1 << (TOP_BIT - scratch_uint)))
                    OPCODE 3: // RCR
                        R_M_OP(
                            mem[rm_addr],
                            += (regs8[FLAG_CF] << (TOP_BIT - scratch_uint)) + ,
                            scratch2_uint << (1 + TOP_BIT - scratch_uint));
                        set_CF(scratch2_uint & 1 << (scratch_uint - 1));
                        set_OF(SIGN_OF(op_result) ^ SIGN_OF(op_result * 2))
                    OPCODE 4: // SHL
                        set_OF(
                            SIGN_OF(op_result) ^ set_CF(
                                SIGN_OF(op_dest << (scratch_uint - 1))))
                    OPCODE 5: // SHR
                        set_OF(SIGN_OF(op_dest))
                    OPCODE 7: // SAR
                        scratch_uint < TOP_BIT || set_CF(scratch2_uint);
                        set_OF(0);
                        R_M_OP(
                            mem[rm_addr],
                            +=,
                            scratch2_uint *=
                                ~(((1 << TOP_BIT) - 1) >> scratch_uint));
                }
            OPCODE 13: // LOOPxx|JCZX
                scratch_uint = !!--regs16[REG_CX];

                switch(i_reg4bit)
                {
                    OPCODE_CHAIN 0: // LOOPNZ
                        scratch_uint &= !regs8[FLAG_ZF]
                    OPCODE 1: // LOOPZ
                        scratch_uint &= regs8[FLAG_ZF]
                    OPCODE 3: // JCXXZ
                        scratch_uint = !++regs16[REG_CX];
                }
                reg_ip += scratch_uint*(int8_t)i_data0
            OPCODE 14: // JMP | CALL short/near
                reg_ip += 3 - i_d;
                if (!i_w)
                {
                    if (i_d) // JMP far
                        reg_ip = 0,
                        regs16[REG_CS] = i_data2;
                    else // CALL
                        R_M_PUSH(reg_ip);
                }
                reg_ip += i_d && i_w ? (int8_t)i_data0 : i_data0
            OPCODE 15: // TEST reg, r/m
                MEM_OP(op_from_addr, &, op_to_addr)
            OPCODE 16: // XCHG AX, regs16
                i_w = 1;
                op_to_addr = REGS_BASE;
                op_from_addr = GET_REG_ADDR(i_reg4bit);
            OPCODE_CHAIN 24: // NOP|XCHG reg, r/m
                if (op_to_addr != op_from_addr)
                    OP(^=),
                    MEM_OP(op_from_addr, ^=, op_to_addr),
                    OP(^=)
            OPCODE 17: // MOVSx (extra=0)|STOSx (extra=1)|LODSx (extra=2)
                scratch2_uint = seg_override_en ? seg_override : REG_DS;

                for (scratch_uint = rep_override_en ? regs16[REG_CX] : 1;
                    scratch_uint;
                    scratch_uint--)
                {
                    MEM_OP(
                        extra < 2 ? SEGREG(REG_ES, REG_DI,) : REGS_BASE,
                        =,
                        extra & 1 ? REGS_BASE : SEGREG(scratch2_uint, REG_SI,)),
                    extra & 1 || INDEX_INC(REG_SI),
                    extra & 2 || INDEX_INC(REG_DI);
                }

                if (rep_override_en)
                    regs16[REG_CX] = 0
            OPCODE 18: // CMPSx (extra=0)|SCASx (extra=1)
                scratch2_uint = seg_override_en ? seg_override : REG_DS;

                if ((scratch_uint = rep_override_en ? regs16[REG_CX] : 1))
                {
                    for (; scratch_uint; rep_override_en || scratch_uint--)
                    {
                        MEM_OP(
                            extra ? REGS_BASE : SEGREG(scratch2_uint, REG_SI,),
                            -,
                            SEGREG(REG_ES, REG_DI,)),
                        extra || INDEX_INC(REG_SI),
                        INDEX_INC(REG_DI),
                        rep_override_en &&
                            !(--regs16[REG_CX] &&
                                (!op_result == rep_mode)) && (scratch_uint = 0);
                    }

                    // Funge to set SZP/AO flags
                    set_flags_type = FLAGS_UPDATE_SZP | FLAGS_UPDATE_AO_ARITH;
                    set_CF(op_result > op_dest);
                }
            OPCODE 19: // RET|RETF|IRET
                i_d = i_w;
                R_M_POP(reg_ip);
                if (extra) // IRET|RETF|RETF imm16
                    R_M_POP(regs16[REG_CS]);
                if (extra & 2) // IRET
                    set_flags(R_M_POP(scratch_uint));
                else if (!i_d) // RET|RETF imm16
                    regs16[REG_SP] += i_data0
            OPCODE 20: // MOV r/m, immed
                R_M_OP(mem[op_from_addr], =, i_data2)
            OPCODE 21: // IN AL/AX, DX/imm8
                io_ports[0x20] = 0; // PIC EOI
                // PIT channel 0/2 read placeholder
                io_ports[0x42] = --io_ports[0x40];
                io_ports[0x3DA] ^= 9; // CGA refresh
                scratch_uint = extra ? regs16[REG_DX] : (uint8_t)i_data0;
                // Scancode read flag
                scratch_uint == 0x60 && (io_ports[0x64] = 0);
                // CRT cursor position
                scratch_uint == 0x3D5
                    && (io_ports[0x3D4] >> 1 == 7)
                    && (io_ports[0x3D5] = (
                        (mem[0x49E] * 80 + mem[0x49D] + CAST(int16_t)mem[0x4AD])
                        & (io_ports[0x3D4] & 1 ? 0xFF : 0xFF00))
                        >> (io_ports[0x3D4] & 1 ? 0 : 8));
                R_M_OP(regs8[REG_AL], =, io_ports[scratch_uint]);
            OPCODE 22: // OUT DX/imm8, AL/AX
                scratch_uint = extra ? regs16[REG_DX] : (uint8_t)i_data0;
                R_M_OP(io_ports[scratch_uint], =, regs8[REG_AL]);

                if (scratch_uint == 0x61)
                {
                    io_hi_lo = 0;
                    update_pc_speaker();
                }

                if ((scratch_uint == 0x40 || scratch_uint == 0x42) &&
                    (io_ports[0x43] & 6))
                {
                    io_hi_lo ^= 1;
                    mem[0x469 + scratch_uint - io_hi_lo] = regs8[REG_AL];

                    if (scratch_uint == 0x42 && !io_hi_lo)
                        update_pc_speaker();
                }

                if (scratch_uint == 0x43)
                {
                    io_hi_lo = 0;

                    if (regs8[REG_AL] >> 6 == 2)
                    {
                        pc_speaker_mode_enabled =
                            ((regs8[REG_AL] & 0xF7) == 0xB6);

                        update_pc_speaker();
                    }
                }

                // CRT video RAM start offset
                scratch_uint == 0x3D5
                    && (io_ports[0x3D4] >> 1 == 6)
                    && (mem[0x4AD + !(io_ports[0x3D4] & 1)] = regs8[REG_AL]);
                // CRT cursor position
                scratch_uint == 0x3D5
                    && (io_ports[0x3D4] >> 1 == 7)
                    && (scratch2_uint = (
                        (mem[0x49E] * 80 + mem[0x49D] + CAST(int16_t)mem[0x4AD])
                        & (io_ports[0x3D4] & 1 ? 0xFF00 : 0xFF))
                        + (regs8[REG_AL] << (io_ports[0x3D4] & 1 ? 0 : 8))
                        - CAST(int16_t)mem[0x4AD],
                        mem[0x49D] = scratch2_uint % 80,
                        mem[0x49E] = scratch2_uint / 80);
                // Hercules resolution reprogramming. Defaults are set in the BIOS
                scratch_uint == 0x3B5
                    && io_ports[0x3B4] == 1
                    && (GRAPHICS_X = regs8[REG_AL] * 16);
                scratch_uint == 0x3B5
                    && io_ports[0x3B4] == 6
                    && (GRAPHICS_Y = regs8[REG_AL] * 4);
            OPCODE 23: // REPxx
                rep_override_en = 2;
                rep_mode = i_w;
                seg_override_en && seg_override_en++
            OPCODE 25: // PUSH reg
                R_M_PUSH(regs16[extra])
            OPCODE 26: // POP reg
                R_M_POP(regs16[extra])
            OPCODE 27: // xS: segment overrides
                seg_override_en = 2;
                seg_override = extra;
                rep_override_en && rep_override_en++
            OPCODE 28: // DAA/DAS
                i_w = 0;
                // extra = 0 for DAA, 1 for DAS
                extra ? DAA_DAS(-=, >=, 0xFF, 0x99) : DAA_DAS(+=, <, 0xF0, 0x90)
            OPCODE 29: // AAA/AAS
                op_result = AAA_AAS(extra - 1)
            OPCODE 30: // CBW
                regs8[REG_AH] = -SIGN_OF(regs8[REG_AL])
            OPCODE 31: // CWD
                regs16[REG_DX] = -SIGN_OF(regs16[REG_AX])
            OPCODE 32: // CALL FAR imm16:imm16
                R_M_PUSH(regs16[REG_CS]);
                R_M_PUSH(reg_ip + 5);
                regs16[REG_CS] = i_data2;
                reg_ip = i_data0
            OPCODE 33: // PUSHF
                make_flags();
                R_M_PUSH(scratch_uint)
            OPCODE 34: // POPF
                set_flags(R_M_POP(scratch_uint))
            OPCODE 35: // SAHF
                make_flags();
                set_flags((scratch_uint & 0xFF00) + regs8[REG_AH])
            OPCODE 36: // LAHF
                make_flags(),
                regs8[REG_AH] = scratch_uint
            OPCODE 37: // LES|LDS reg, r/m
                i_w = i_d = 1;
                DECODE_RM_REG;
                OP(=);
                MEM_OP(REGS_BASE + extra, =, rm_addr + 2)
            OPCODE 38: // INT 3
                ++reg_ip;
                pc_interrupt(3)
            OPCODE 39: // INT imm8
                reg_ip += 2;
                pc_interrupt(i_data0)
            OPCODE 40: // INTO
                ++reg_ip;
                regs8[FLAG_OF] && pc_interrupt(4)
            OPCODE 41: // AAM
                if (i_data0 &= 0xFF)
                    regs8[REG_AH] = regs8[REG_AL] / i_data0,
                    op_result = regs8[REG_AL] %= i_data0;
                else // Divide by zero
                    pc_interrupt(0)
            OPCODE 42: // AAD
                i_w = 0;
                regs16[REG_AX] =
                    op_result = 0xFF & regs8[REG_AL] + i_data0 * regs8[REG_AH]
            OPCODE 43: // SALC
                regs8[REG_AL] = -regs8[FLAG_CF]
            OPCODE 44: // XLAT
                regs8[REG_AL] = mem[SEGREG(
                    seg_override_en ? seg_override : REG_DS,
                    REG_BX,
                    regs8[REG_AL] +)]
            OPCODE 45: // CMC
                regs8[FLAG_CF] ^= 1
            OPCODE 46: // CLC|STC|CLI|STI|CLD|STD
                regs8[extra / 2] = extra & 1
            OPCODE 47: // TEST AL/AX, immed
                R_M_OP(regs8[REG_AL], &, i_data0)
            OPCODE 48: // Emulator-specific 0F xx opcodes
                switch ((int8_t)i_data0)
                {
                    OPCODE_CHAIN 0: // PUTCHAR_AL
                        _write(1, regs8, 1)
                    OPCODE 1: // GET_RTC
                        time(&clock_buf);
                        ftime(&ms_clock);
                        memcpy(
                            mem + SEGREG(REG_ES, REG_BX,),
                            localtime(&clock_buf),
                            sizeof(struct tm));
                        CAST(int16_t)mem[SEGREG(REG_ES, REG_BX, 36 +)] =
                            ms_clock.millitm;
                    OPCODE 2: // DISK_READ
                    OPCODE_CHAIN 3: // DISK_WRITE
                        regs8[REG_AL] = ~_lseek(
                            disk[regs8[REG_DL]],
                            CAST(uint32_t)regs16[REG_BP] << 9,
                            0)
                            ? ((int8_t)i_data0 == 3
                                ? (int(*)())_write
                                : (int(*)())_read)(
                                    disk[regs8[REG_DL]],
                                    mem + SEGREG(REG_ES, REG_BX,),
                                    regs16[REG_AX])
                            : 0;
                }
        }

        // Increment instruction pointer by computed instruction length. Tables
        // in the BIOS binary help us here.
        reg_ip += (i_mod * (i_mod != 3) + 2 * (!i_mod && i_rm == 6))
            * i_mod_size
            + bios_table_lookup[TABLE_BASE_INST_SIZE][raw_opcode_id]
            + bios_table_lookup[TABLE_I_W_SIZE][raw_opcode_id] * (i_w + 1);

        // If instruction needs to update SF, ZF and PF, set them as appropriate
        if (set_flags_type & FLAGS_UPDATE_SZP)
        {
            regs8[FLAG_SF] = SIGN_OF(op_result);
            regs8[FLAG_ZF] = !op_result;
            regs8[FLAG_PF] = bios_table_lookup[
                TABLE_PARITY_FLAG][(uint8_t)op_result];

            // If instruction is an arithmetic or logic operation, also set
            // AF/OF/CF as appropriate.
            if (set_flags_type & FLAGS_UPDATE_AO_ARITH)
                set_AF_OF_arith();
            if (set_flags_type & FLAGS_UPDATE_OC_LOGIC)
                set_CF(0), set_OF(0);
        }

        // Poll timer/keyboard every KEYBOARD_TIMER_UPDATE_DELAY instructions
        if (!(++inst_counter % KEYBOARD_TIMER_UPDATE_DELAY))
            int8_asap = 1;

        // Update the graphics display every GRAPHICS_UPDATE_DELAY instructions
        if (!(inst_counter % GRAPHICS_UPDATE_DELAY))
        {
            // Video card in graphics mode?
            if (io_ports[0x3B8] & 2)
            {
                // Create the display and compute color and video memory
                // translation tables when entering graphics mode.
                if (!display_frame_buffer.FrameBufferBase)
                {
                    for (uint32_t i = 0; i < 256; ++i)
                    {
                        uint8_t red = (uint8_t)((i >> 5) & 7);
                        uint8_t green = (uint8_t)((i >> 2) & 7);
                        uint8_t blue = (uint8_t)(i & 3);

                        red = (uint8_t)(
                            (red << 5) |
                            (red << 2) |
                            (red >> 1));

                        green = (uint8_t)(
                            (green << 5) |
                            (green << 2) |
                            (green >> 1));

                        blue = (uint8_t)(blue * 85);

                        rgb332_colors[i] = MO_DISPLAY_BGRA32_MAKE(
                            red,
                            green,
                            blue,
                            MO_UINT8_MAX);
                    }

                    for (uint32_t i = 0; i < 16; ++i)
                    {
                        pixel_colors[i] = mem[0x4AC]
                            ? (uint32_t)cga_colors[(i & 12) >> 2] +
                            ((uint32_t)cga_colors[i & 3] << 16)
                            : UINT32_C(0xFF) * (
                                ((uint32_t)(i & 1) << 24) +
                                ((uint32_t)(i & 2) << 15) +
                                ((uint32_t)(i & 4) << 6) +
                                ((uint32_t)(i & 8) >> 3));
                    }

                    for (uint32_t i = 0;
                        i < GRAPHICS_X * GRAPHICS_Y / 4;
                        ++i)
                    {
                        vid_addr_lookup[i] =
                            i / GRAPHICS_X * (GRAPHICS_X / 8) +
                            (i / 2) % (GRAPHICS_X / 8) +
                            0x2000 * (
                                mem[0x4AC]
                                ? (2 * i / GRAPHICS_X) % 2
                                : (4 * i / GRAPHICS_X) % 4);
                    }

                    MoRetroVDisplayCreate(
                        GRAPHICS_X,
                        GRAPHICS_Y,
                        &display_frame_buffer);
                }

                // Refresh the BGRA32 frame buffer from emulated video RAM.
                vid_mem_base =
                    mem +
                    0xB0000 +
                    0x8000 * (
                        mem[0x4AC]
                        ? 1
                        : io_ports[0x3B8] >> 7);

                uint32_t pixel_group_count = GRAPHICS_X * GRAPHICS_Y / 4;

                uint32_t pixel_groups_per_line = GRAPHICS_X / 4;

                for (uint32_t i = 0;
                    i < pixel_group_count;
                    ++i)
                {
                    uint32_t packed_colors =
                        pixel_colors[
                            15 & (
                                vid_mem_base[vid_addr_lookup[i]] >>
                                (4 * !(i & 1)))];

                    PMO_UINT32 destination =
                        display_frame_buffer.FrameBufferBase +
                        (i / pixel_groups_per_line) *
                        display_frame_buffer.PixelsPerScanLine +
                        (i % pixel_groups_per_line) * 4;

                    destination[0] =
                        rgb332_colors[(uint8_t)(packed_colors >> 0)];

                    destination[1] =
                        rgb332_colors[(uint8_t)(packed_colors >> 8)];

                    destination[2] =
                        rgb332_colors[(uint8_t)(packed_colors >> 16)];

                    destination[3] =
                        rgb332_colors[(uint8_t)(packed_colors >> 24)];
                }

                MoRetroVDisplayPresent();
            }
            else if (display_frame_buffer.FrameBufferBase)
            {
                // The application has returned to text mode.
                MoRetroVDisplayDestroy();
                display_frame_buffer.FrameBufferBase = nullptr;
            }
        }

        // Application has set trap flag, so fire INT 1
        if (trap_flag)
            pc_interrupt(1);

        trap_flag = regs8[FLAG_TF];

        // If a timer tick is pending, interrupts are enabled, and no
        // overrides/REP are active, then process the tick and check for new
        // keystrokes
        if (int8_asap &&
            !seg_override_en &&
            !rep_override_en &&
            regs8[FLAG_IF] &&
            !regs8[FLAG_TF])
        {
            pc_interrupt(0xA);
            int8_asap = 0;

            if (display_frame_buffer.FrameBufferBase)
            {
                MO_UINT16 key_event = 0;

                if (MoRetroVDisplayReadKey(&key_event))
                {
                    CAST(uint16_t)mem[0x4A6] = key_event;

                    pc_interrupt(7);
                }
            }
            else if (_kbhit())
            {
                // Keyboard driver for console.
                mem[0x4A6] = _getch();
                pc_interrupt(7);
            }
        }
    }

    if (display_frame_buffer.FrameBufferBase)
    {
        MoRetroVDisplayDestroy();
        display_frame_buffer.FrameBufferBase = nullptr;
    }

    return 0;
}
