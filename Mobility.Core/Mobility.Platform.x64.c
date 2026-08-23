/*
 * PROJECT:    Mobility
 * FILE:       Mobility.Platform.x64.c
 * PURPOSE:    Implementation for Mobility Runtime x64 Specific Parts
 *
 * LICENSE:    The MIT License
 *
 * MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
 */

#include "Mobility.Platform.x64.h"

#include <Mile.Mobility.Utilities.MemoryAccess.h>

#ifdef _MSC_VER

#ifndef MOBILITY_PLATFORM_X64_PRIVATE
#define MOBILITY_PLATFORM_X64_PRIVATE

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void _mm_pause();

void __halt();

void __cdecl __debugbreak();

void __cpuid(int[4], int);

unsigned __int64 __readmsr(unsigned long);
void __writemsr(unsigned long, unsigned __int64);

unsigned __int64 __readcr3();
void __writecr3(unsigned __int64);

void __lidt(void*);

unsigned char __inbyte(unsigned short);
unsigned short __inword(unsigned short);
unsigned long __indword(unsigned short);

void __outbyte(unsigned short, unsigned char);
void __outword(unsigned short, unsigned short);
void __outdword(unsigned short, unsigned long);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // !MOBILITY_PLATFORM_X64_PRIVATE

MO_EXTERN_C MO_VOID MOAPI MoPlatformPause()
{
    _mm_pause();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformHalt()
{
    __halt();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformDebugBreak()
{
    __debugbreak();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformReadCpuid(
    _Mo_Out_ PMO_PLATFORM_X64_CPUID_RESULT Result,
    _Mo_In_ MO_UINT32 Index)
{
    __cpuid((int*)Result, (int)Index);
}

MO_EXTERN_C MO_UINT64 MOAPI MoPlatformReadMsr(
    _Mo_In_ MO_UINT32 Index)
{
    return __readmsr(Index);
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformWriteMsr(
    _Mo_In_ MO_UINT32 Index,
    _Mo_In_ MO_UINT64 Value)
{
    __writemsr(Index, Value);
}

MO_EXTERN_C MO_UINT64 MOAPI MoPlatformReadCr3()
{
    return __readcr3();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformWriteCr3(
    _Mo_In_ MO_UINT64 Value)
{
    __writecr3(Value);
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformLoadInterruptDescriptorTable(
    _Mo_In_ PMO_PLATFORM_X64_PSEUDO_DESCRIPTOR Descriptor)
{
    __lidt(Descriptor);
}

MO_EXTERN_C MO_UINT8 MOAPI MoPlatformReadIoPort8(
    _Mo_In_ MO_UINT16 Port)
{
    MoMileCompilerBarrier();
    MO_UINT8 Result = __inbyte(Port);
    MoMileCompilerBarrier();
    return Result;
}

MO_EXTERN_C MO_UINT16 MOAPI MoPlatformReadIoPort16(
    _Mo_In_ MO_UINT16 Port)
{
    MoMileCompilerBarrier();
    MO_UINT16 Result = __inword(Port);
    MoMileCompilerBarrier();
    return Result;
}

MO_EXTERN_C MO_UINT32 MOAPI MoPlatformReadIoPort32(
    _Mo_In_ MO_UINT16 Port)
{
    MoMileCompilerBarrier();
    MO_UINT32 Result = __indword(Port);
    MoMileCompilerBarrier();
    return Result;
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformWriteIoPort8(
    _Mo_In_ MO_UINT16 Port,
    _Mo_In_ MO_UINT8 Value)
{
    MoMileCompilerBarrier();
    __outbyte(Port, Value);
    MoMileCompilerBarrier();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformWriteIoPort16(
    _Mo_In_ MO_UINT16 Port,
    _Mo_In_ MO_UINT16 Value)
{
    MoMileCompilerBarrier();
    __outword(Port, Value);
    MoMileCompilerBarrier();
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformWriteIoPort32(
    _Mo_In_ MO_UINT16 Port,
    _Mo_In_ MO_UINT32 Value)
{
    MoMileCompilerBarrier();
    __outdword(Port, Value);
    MoMileCompilerBarrier();
}

#endif // _MSC_VER

MO_EXTERN_C MO_VOID MOAPI MoPlatformSetSegmentDescriptorBase(
    _Mo_Out_ PMO_PLATFORM_X64_SEGMENT_DESCRIPTOR Descriptor,
    _Mo_In_ MO_UINT32 Base)
{
    if (Descriptor)
    {
        Descriptor->BaseLow = ((MO_UINT16)(Base));
        Descriptor->BaseMiddle = ((MO_UINT8)(Base >> 16)) & 0xFF;
        Descriptor->BaseHigh = ((MO_UINT8)(Base >> 24)) & 0xFF;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformSetSegmentDescriptorLimit(
    _Mo_Out_ PMO_PLATFORM_X64_SEGMENT_DESCRIPTOR Descriptor,
    _Mo_In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformSetSystemSegmentDescriptorBase(
    _Mo_Out_ PMO_PLATFORM_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _Mo_In_ MO_UINT64 Base)
{
    if (Descriptor)
    {
        Descriptor->BaseLow = ((MO_UINT16)(Base));
        Descriptor->BaseMiddleLow = ((MO_UINT8)(Base >> 16)) & 0xFF;
        Descriptor->BaseMiddleHigh = ((MO_UINT8)(Base >> 24)) & 0xFF;
        Descriptor->BaseHigh = ((MO_UINT32)(Base >> 32)) & 0xFFFFFFFF;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformSetSystemSegmentDescriptorLimit(
    _Mo_Out_ PMO_PLATFORM_X64_SYSTEM_SEGMENT_DESCRIPTOR Descriptor,
    _Mo_In_ MO_UINT32 Limit)
{
    if (Descriptor)
    {
        Descriptor->LimitLow = ((MO_UINT16)(Limit));
        Descriptor->LimitHigh = ((MO_UINT8)(Limit >> 16)) & 0xF;
    }
}

MO_EXTERN_C MO_VOID MOAPI MoPlatformSetIdtGateDescriptorOffset(
    _Mo_Out_ PMO_PLATFORM_X64_IDT_GATE_DESCRIPTOR Descriptor,
    _Mo_In_ MO_UINT64 Offset)
{
    if (Descriptor)
    {
        Descriptor->OffsetLow = ((MO_UINT16)(Offset));
        Descriptor->OffsetMiddle = ((MO_UINT16)(Offset >> 16)) & 0xFFFF;
        Descriptor->OffsetHigh = ((MO_UINT32)(Offset >> 32)) & 0xFFFFFFFF;
    }
}

MO_EXTERN_C MO_RESULT MOAPI MoPlatformSerialPortInitialize(
    _Mo_In_ MO_UINT16 Base,
    _Mo_In_ MO_UINT32 Baud)
{
    if (MO_PLATFORM_X64_SERIAL_PORT_COM1 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM2 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM3 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM4 != Base)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!Baud || Baud > MO_PLATFORM_X64_SERIAL_PORT_BAUD_BASE)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    MO_UINT16 Divisor = 0;
    {
        MO_UINT32 Remaining = MO_PLATFORM_X64_SERIAL_PORT_BAUD_BASE;
        while (Remaining >= Baud)
        {
            if (MO_UINT16_MAX == Divisor)
            {
                // The divisor value is out of bounds.
                return MO_RESULT_ERROR_OUT_OF_BOUNDS;
            }

            Remaining -= Baud;
            ++Divisor;
        }
        if (Remaining >= Baud - Remaining)
        {
            if (MO_UINT16_MAX == Divisor)
            {
                // The divisor value is out of bounds.
                return MO_RESULT_ERROR_OUT_OF_BOUNDS;
            }
            ++Divisor;
        }
    }

    MO_PLATFORM_X64_SERIAL_PORT_REGISTER LineControlRegister;

    // Initialize the content of the Line Control Register (LCR).
    {
        // Clear the register.
        LineControlRegister.RawData = 0;
        // Set data bits to 8.
        LineControlRegister.LCR.WLS = 3;
        // Set stop bits to 1.
        LineControlRegister.LCR.STB = 0;
        // Disable parity.
        LineControlRegister.LCR.PEN = 0;
        // Clear Divisor Latch Access Bit (DLAB).
        LineControlRegister.LCR.DLAB = 0;
    }

    // Set the Line Control Register (LCR) to access the Interrupt Enable
    // Register (IER).
    {
        // Set the Line Control Register (LCR).
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_LCR_OFFSET,
            LineControlRegister.RawData);
    }

    // Disable all interrupts by clearing the Interrupt Enable Register (IER).
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Clear the register.
        Register.RawData = 0;
        // Set the Interrupt Enable Register (IER).
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_IER_OFFSET,
            Register.RawData);
    }

    // Set the Line Control Register (LCR) to access the Divisor Latch (Least
    // Significant Byte (LS)) (DLL) register and Divisor Latch (Most Significant
    // Byte (MS)) (DLM) register.
    {
        // Set Divisor Latch Access Bit (DLAB).
        LineControlRegister.LCR.DLAB = 1;
        // Set the Line Control Register (LCR).
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_LCR_OFFSET,
            LineControlRegister.RawData);
    }

    // Set the Divisor Latch (Least Significant Byte (LS)) (DLL) Register.
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Set the least significant byte of the divisor value for the desired
        // baud.
        Register.RawData = (MO_UINT8)Divisor;
        // Set the Divisor Latch (Least Significant Byte (LS)) (DLL) Register.
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_DLL_OFFSET,
            Register.RawData);
    }

    // Set the Divisor Latch (Most Significant Byte (MS)) (DLM) Register.
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Set the most significant byte of the divisor value for the desired
        // baud.
        Register.RawData = (MO_UINT8)(Divisor >> 8);
        // Set the Divisor Latch (Most Significant Byte (MS)) (DLM) Register.
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_DLM_OFFSET,
            Register.RawData);
    }

    // Set the Line Control Register (LCR) to access the Receive Buffer Register
    // (RBR) (Read Only), the Transmit Holding Register (THR) (Write Only), and
    // the Interrupt Enable Register (IER).
    {
        // Clear Divisor Latch Access Bit (DLAB).
        LineControlRegister.LCR.DLAB = 0;
        // Set the Line Control Register (LCR).
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_LCR_OFFSET,
            LineControlRegister.RawData);
    }

    // Set the FIFO Control Register (FCR) to enable and clear the FIFO.
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Clear the register.
        Register.RawData = 0;
        // Enable the FIFO.
        Register.FCR.Enable = 1;
        // Set the Receiver FIFO Reset.
        Register.FCR.ReceiverReset = 1;
        // Set the Transmit FIFO Reset.
        Register.FCR.TransmitReset = 1;
        // Clear the DMA Mode Select.
        Register.FCR.DmaModeSelect = 0;
        // Set the Receiver Trigger Level to 1 byte.
        Register.FCR.ReceiveTrigger = 0;
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_FCR_OFFSET,
            Register.RawData);
    }

    // Set the MODEM Control Register (MCR) to enable Data Terminal Ready (DTR)
    // and Request To Send (RTS).
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Clear the register.
        Register.RawData = 0;
        // Enable Data Terminal Ready (DTR).
        Register.MCR.DTR = 1;
        // Enable Request To Send (RTS).
        Register.MCR.RTS = 1;
        // Clear Output 1 (OUT1).
        Register.MCR.OUT1 = 0;
        // Clear Output 2 (OUT2).
        Register.MCR.OUT2 = 0;
        // Disable Loopback Mode.
        Register.MCR.Loop = 0;
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_MCR_OFFSET,
            Register.RawData);
    }

    return MO_RESULT_SUCCESS_OK;
}

static MO_FORCEINLINE MO_BOOL MoPlatformInternalSerialPortTryRead(
    _Mo_In_ MO_UINT16 Base,
    _Mo_Out_ PMO_UINT8 Data)
{
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Get from the Line Status Register (LSR).
        Register.RawData = MoPlatformReadIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_LSR_OFFSET);
        if (!Register.LSR.DR)
        {
            // If the Data Ready (DR) bit is not set, there is no data available
            // to read.
            return MO_FALSE;
        }
    }

    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Get from the Receive Buffer Register (RBR) (Read Only).
        Register.RawData = MoPlatformReadIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_RBR_OFFSET);
        // Get the data received from the serial port.
        *Data = Register.RawData;
    }

    return MO_TRUE;
}

MO_EXTERN_C MO_RESULT MOAPI MoPlatformSerialPortTryRead(
    _Mo_In_ MO_UINT16 Base,
    _Mo_Out_ PMO_UINT8 Data)
{
    if (MO_PLATFORM_X64_SERIAL_PORT_COM1 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM2 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM3 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM4 != Base)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!Data)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    return MoPlatformInternalSerialPortTryRead(Base, Data)
        ? MO_RESULT_SUCCESS_OK
        : MO_RESULT_ERROR_PENDING;
}

MO_EXTERN_C MO_RESULT MOAPI MoPlatformSerialPortRead(
    _Mo_In_ MO_UINT16 Base,
    _Mo_Out_ PMO_UINT8 Data)
{
    if (MO_PLATFORM_X64_SERIAL_PORT_COM1 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM2 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM3 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM4 != Base)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    if (!Data)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    while (!MoPlatformInternalSerialPortTryRead(Base, Data))
    {
        MoPlatformPause();
    }

    return MO_RESULT_SUCCESS_OK;
}

MO_EXTERN_C MO_RESULT MOAPI MoPlatformSerialPortWrite(
    _Mo_In_ MO_UINT16 Base,
    _Mo_In_ MO_UINT8 Data)
{
    if (MO_PLATFORM_X64_SERIAL_PORT_COM1 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM2 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM3 != Base &&
        MO_PLATFORM_X64_SERIAL_PORT_COM4 != Base)
    {
        return MO_RESULT_ERROR_INVALID_PARAMETER;
    }

    for (;;)
    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Get from the Line Status Register (LSR).
        Register.RawData = MoPlatformReadIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_LSR_OFFSET);
        if (Register.LSR.THRE)
        {
            // If the Transmitter Holding Register Empty (THRE) bit is set, the
            // serial port is ready to transmit data.
            break; 
        }
        MoPlatformPause();
    }

    {
        MO_PLATFORM_X64_SERIAL_PORT_REGISTER Register;
        // Set the data to be transmitted to the serial port.
        Register.RawData = Data;
        // Set the Transmit Holding Register (THR) (Write Only).
        MoPlatformWriteIoPort8(
            Base + MO_PLATFORM_X64_SERIAL_PORT_THR_OFFSET,
            Register.RawData);
    }

    return MO_RESULT_SUCCESS_OK;
}
