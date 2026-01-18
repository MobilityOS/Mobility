;
; PROJECT:    Mobility
; FILE:       Mobility.Platform.x64.Assembly.asm
; PURPOSE:    Implementation for Mobility Runtime x64 Specific Assembly Parts
;
; LICENSE:    The MIT License
;
; MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
;

.CODE

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformPause();
; -----------------------------------------------------------------------------
MoPlatformPause PROC
    pause
    ret
MoPlatformPause ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformHalt();
; -----------------------------------------------------------------------------
MoPlatformHalt PROC
    hlt
    ret
MoPlatformHalt ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformDebugBreak();
; -----------------------------------------------------------------------------
MoPlatformDebugBreak PROC
    int 3
    ret
MoPlatformDebugBreak ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformDisableInterrupts();
; -----------------------------------------------------------------------------
MoPlatformDisableInterrupts PROC
    cli
    ret
MoPlatformDisableInterrupts ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformEnableInterrupts();
; -----------------------------------------------------------------------------
MoPlatformEnableInterrupts PROC
    sti
    ret
MoPlatformEnableInterrupts ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformReadCpuid(
;     _Out_ PMO_PLATFORM_X64_CPUID_RESULT Result,
;     _In_ MO_UINT32 Index);
; -----------------------------------------------------------------------------
MoPlatformReadCpuid PROC
    ; RCX = Result
    ; RDX = Index

    push rbx
    mov r8, rcx
    mov eax, edx
    xor ecx, ecx
    cpuid
    mov [r8], eax
    mov [r8 + 4], ebx
    mov [r8 + 8], ecx
    mov [r8 + 12], edx
    pop rbx
    ret
MoPlatformReadCpuid ENDP

; -----------------------------------------------------------------------------
; EXTERN_C MO_UINT64 MOAPI MoPlatformReadMsr(
;     _In_ MO_UINT32 Index);
; -----------------------------------------------------------------------------
MoPlatformReadMsr PROC
    ; RAX = Value
    ; RCX = Index

    rdmsr
    shl rdx, 32
    or rax, rdx
    ret
MoPlatformReadMsr ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformWriteMsr(
;     _In_ MO_UINT32 Index,
;     _In_ MO_UINT64 Value);
; -----------------------------------------------------------------------------
MoPlatformWriteMsr PROC
    ; RCX = Index
    ; RDX = Value

    mov rax, rdx
    shr rdx, 32
    wrmsr
    ret
MoPlatformWriteMsr ENDP

; -----------------------------------------------------------------------------
; EXTERN_C MO_UINT64 MOAPI MoPlatformReadCr3();
; -----------------------------------------------------------------------------
MoPlatformReadCr3 PROC
    mov rax, cr3
    ret
MoPlatformReadCr3 ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformWriteCr3(
;     _In_ MO_UINT64 Value);
; -----------------------------------------------------------------------------
MoPlatformWriteCr3 PROC
    ; RCX = Value

    mov cr3, rcx
    ret
MoPlatformWriteCr3 ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadGlobalDescriptorTable(
;     _In_ PMO_PLATFORM_X64_PSEUDO_DESCRIPTOR Descriptor);
; -----------------------------------------------------------------------------
MoPlatformLoadGlobalDescriptorTable PROC
    ; RCX = Descriptor

    lgdt FWORD PTR [rcx]
    ret
MoPlatformLoadGlobalDescriptorTable ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadInterruptDescriptorTable(
;     _In_ PMO_PLATFORM_X64_PSEUDO_DESCRIPTOR Descriptor);
; -----------------------------------------------------------------------------
MoPlatformLoadInterruptDescriptorTable PROC
    ; RCX = Descriptor

    lidt FWORD PTR [rcx]
    ret
MoPlatformLoadInterruptDescriptorTable ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformSetCodeSegmentSelector(
;     _In_ MO_UINT16 CodeSelector);
; -----------------------------------------------------------------------------
MoPlatformSetCodeSegmentSelector PROC PUBLIC
    ; CX = CodeSelector

    ; Set the new return address.
    lea rcx, NewReturnAddress

    ; Set the code segment selector.
    movzx rdx, cx
	push rdx

    ; Set the new return address.
	push rcx

    ; Perform a far return for switch to the new code segment.
	retfq

NewReturnAddress:

    ; Return to the caller.
	ret
MoPlatformSetCodeSegmentSelector ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformSetDataSegmentSelectors(
;     _In_ MO_UINT16 DataSelector);
; -----------------------------------------------------------------------------
MoPlatformSetDataSegmentSelectors PROC PUBLIC
    ; CX = DataSelector

    mov ss, cx
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    ret
MoPlatformSetDataSegmentSelectors ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadTaskRegister(
;     _In_ MO_UINT16 TssSelector);
; -----------------------------------------------------------------------------
MoPlatformLoadTaskRegister PROC
    ; RCX = TssSelector

    ltr cx
    ret
MoPlatformLoadTaskRegister ENDP

; -----------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformSwitchToNewStack(
;     _In_ MO_POINTER StackAddress,
;     _In_ MO_POINTER FunctionAddress);
; -----------------------------------------------------------------------------
MoPlatformSwitchToNewStack PROC
    ; RCX = StackAddress
    ; RDX = FunctionAddress

    ; Reserve space for register parameters (rcx, rdx, r8 & r9) on the stack,
    ; in case the callee wishes to spill them.
    lea rsp, [rcx - 20h]

    ; Call the function.
    call rdx

    ; If the function returns, we halt because the stack has been switched
    ; and we cannot return to the original caller.
    cli
HaltLoop:
    hlt
    jmp HaltLoop
MoPlatformSwitchToNewStack ENDP

.DATA

ALIGN 8
; -----------------------------------------------------------------------------
; EXTERN_C PMO_PLATFORM_X64_INTERRUPT_HANDLER* MoPlatformInterruptHandlerTable;
; -----------------------------------------------------------------------------
; The length of the pointed interrupt handler table must be 256 entries.
PUBLIC MoPlatformInterruptHandlerTable
MoPlatformInterruptHandlerTable QWORD 0

.CODE

; -----------------------------------------------------------------------------
; MoPlatformInterruptCommonEntry
; -----------------------------------------------------------------------------
; The follow algorithm is used for the common interrupt routine.
MoPlatformInterruptCommonEntry PROC PUBLIC FRAME
    .PUSHFRAME CODE
    .ALLOCSTACK 8
    cli

    ; All interrupt handlers are invoked through interrupt gates, so IF flag
    ; automatically cleared at the entry point

    push rbp
    .PUSHREG rbp
    mov rbp, rsp
    .SETFRAME rbp, 0
    .ENDPROLOG

    ; Stack:
    ; +---------------------+ <-- 16-byte aligned ensured by processor
    ; +    Old SS           +
    ; +---------------------+
    ; +    Old RSP          +
    ; +---------------------+
    ; +    RFlags           +
    ; +---------------------+
    ; +    CS               +
    ; +---------------------+
    ; +    RIP              +
    ; +---------------------+
    ; +    Error Code       +
    ; +---------------------+
    ; +    Vector Number    +
    ; +---------------------+
    ; +    RBP              +
    ; +---------------------+ <-- RBP, 16-byte aligned

    ; Since here the stack pointer is 16-byte aligned, so
    ; MO_PLATFORM_X64_FXSAVE_AREA of PMO_PLATFORM_X64_INTERRUPT_CONTEXT
    ; is 16-byte aligned

    ; MO_UINT64 Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
    ; MO_UINT64 R8, R9, R10, R11, R12, R13, R14, R15;

    push r15
    push r14
    push r13
    push r12
    push r11
    push r10
    push r9
    push r8
    push rax
    push rcx
    push rdx
    push rbx
    push qword ptr [rbp + 48] ; RSP
    push qword ptr [rbp] ; RBP
    push rsi
    push rdi

    ; MO_UINT64 Gs, Fs, Es, Ds, Cs, Ss;
    ;insure high 16 bits of each is zero

    movzx rax, word ptr [rbp + 56]
    push rax ; for ss
    movzx rax, word ptr [rbp + 32]
    push rax ; for cs
    mov rax, ds
    push rax
    mov rax, es
    push rax
    mov rax, fs
    push rax
    mov rax, gs
    push rax

    ; MO_UINT64 Rip;
    push qword ptr [rbp + 24]

    ; MO_UINT64 Gdtr[2], Idtr[2];

    xor rax, rax
    push rax
    push rax
    sidt [rsp]
    xchg rax, [rsp + 2]
    xchg rax, [rsp]
    xchg rax, [rsp + 8]

    xor rax, rax
    push rax
    push rax
    sgdt [rsp]
    xchg rax, [rsp + 2]
    xchg rax, [rsp]
    xchg rax, [rsp + 8]

    ; MO_UINT64 Ldtr, Tr;

    xor rax, rax
    str ax
    push rax
    sldt ax
    push rax

    ; MO_UINT64 RFlags;
    push qword ptr [rbp + 40]

    ; MO_UINT64 Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;

    mov rax, cr8
    push rax
    mov rax, cr4
    or rax, 208h
    mov cr4, rax
    push rax
    mov rax, cr3
    push rax
    mov rax, cr2
    push rax
    xor rax, rax
    push rax
    mov rax, cr0
    push rax

    ; MO_UINT64 Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;

    mov rax, dr7
    push rax
    mov rax, dr6
    push rax
    mov rax, dr3
    push rax
    mov rax, dr2
    push rax
    mov rax, dr1
    push rax
    mov rax, dr0
    push rax

    ; MO_PLATFORM_X64_FXSAVE_AREA FxSaveState;

    sub rsp, 512
    mov rcx, rsp
    fxsave [rcx]

    ; UEFI calling convention for x64 requires that Direction flag in EFLAGs is
    ; clear
    cld

    ; MO_UINT32 ExceptionData;
    push qword ptr [rbp + 16]

    ; Call into exception handler
    movzx rcx, byte ptr [rbp + 8]
    mov rax, qword ptr [MoPlatformInterruptHandlerTable]
    test rax, rax ; NULL?
    jz SkipCallInterruptHandler
    mov rax, [rax + rcx * 8]
    test rax, rax ; NULL?
    jz SkipCallInterruptHandler;

    ; Prepare parameter and call
    mov rdx, rsp

    ; Per X64 calling convention, allocate maximum parameter stack space and
    ; make sure RSP is 16-byte aligned

    sub rsp, 4 * 8 + 8
    call rax
    add rsp, 4 * 8 + 8

SkipCallInterruptHandler:
    ; BUGBUG: This should not be necessary, but it's currently true that
    ; interrupt handlers enable interrupts
    cli
    ; MO_UINT64 ExceptionData;
    add rsp, 8

    ; MO_PLATFORM_X64_FXSAVE_AREA FxSaveState;

    mov rcx, rsp
    fxrstor [rcx]
    add rsp, 512

    ; MO_UINT64 Dr0, Dr1, Dr2, Dr3, Dr6, Dr7;
    ; Skip restoration of DRx registers to support in-circuit emualators or
    ; debuggers set breakpoint in interrupt/exception context
    add rsp, 8 * 6

    ; MO_UINT64 Cr0, Cr1, Cr2, Cr3, Cr4, Cr8;
    pop rax
    mov cr0, rax
    add rsp, 8 ; not for Cr1
    pop rax
    mov cr2, rax
    pop rax
    mov cr3, rax
    pop rax
    mov cr4, rax
    pop rax
    mov cr8, rax

    ; MO_UINT64 RFlags;
    pop qword ptr [rbp + 40]

    ; MO_UINT64 Ldtr, Tr;
    ; MO_UINT64 Gdtr[2], Idtr[2];
    ; Best not let anyone mess with these particular registers...
    add rsp, 48

    ; MO_UINT64 Rip;
    pop qword ptr [rbp + 24]

    ; MO_UINT64 Gs, Fs, Es, Ds, Cs, Ss;
    pop rax
    ; mov gs, rax ; not for gs
    pop rax
    ; mov fs, rax ; not for fs
    ; (X64 will not use fs and gs, so we do not restore it)
    pop rax
    mov es, rax
    pop rax
    mov ds, rax
    pop qword ptr [rbp + 32] ; for cs
    pop qword ptr [rbp + 56] ; for ss

    ; MO_UINT64 Rdi, Rsi, Rbp, Rsp, Rbx, Rdx, Rcx, Rax;
    ; MO_UINT64 R8, R9, R10, R11, R12, R13, R14, R15;

    pop rdi
    pop rsi
    add rsp, 8 ; not for rbp
    pop qword ptr [rbp + 48] ; for rsp
    pop rbx
    pop rdx
    pop rcx
    pop rax
    pop r8
    pop r9
    pop r10
    pop r11
    pop r12
    pop r13
    pop r14
    pop r15

    mov rsp, rbp
    mov rbp, qword ptr[rbp]
    add rsp, 24
    iretq

MoPlatformInterruptCommonEntry ENDP

; -----------------------------------------------------------------------------
; EXTERN_C MO_UINT64 MoPlatformInterruptDescriptorTableHandlers[256];
; -----------------------------------------------------------------------------
; These are the actual interrupt vector entry points. The macros below ensure
; that the vector always pushes an error code and the vector number to the stack
; before calling into MoPlatformInterruptCommonEntry. Some interrupts come
; pre-populated with an error code and do not need an extra one pushed.
; N.B. Each entry must be no more than 8 bytes and must be 8-byte aligned.
; This allows the C code to compute the address of each vector as an offset
; from MoPlatformInterruptDescriptorTableHandlers.
ALIGN 8
PUBLIC MoPlatformInterruptDescriptorTableHandlers
MoPlatformInterruptDescriptorTableHandlers LABEL BYTE
Vector = 0
REPEAT 256
ALIGN 8

    ; The following interrupts have error codes associated with them.
    ; For all other interrupts, a dummy error code is pushed.
    ; This mask covers the following exceptions:
    ;   8 - Double Fault (#DF)
    ;   10 - Invalid TSS (#TS)
    ;   11 - Segment Not Present (#NP)
    ;   12 - Stack-Segment Fault (#SS)
    ;   13 - General Protection Fault (#GP)
    ;   14 - Page Fault (#PF)
    ;   17 - Alignment Check (#AC)
    ;   21 - Control Protection Exception (#CP)
    DummyCodeNeeded = 1
    IF Vector EQ 8
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 10
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 11
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 12
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 13
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 14
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 17
        DummyCodeNeeded = 0
    ELSEIF Vector EQ 21
        DummyCodeNeeded = 0
    ENDIF

    IF DummyCodeNeeded
        ; Push a dummy error code. Use rax to get a 1-byte instruction to fit.
        push rax
    ENDIF

    ; Must use LOW(Vector) to ensure only use 2-byte instruction to push vector
    ; number.
    push LOW(Vector)
    jmp MoPlatformInterruptCommonEntry

Vector = Vector + 1
ENDM

END
