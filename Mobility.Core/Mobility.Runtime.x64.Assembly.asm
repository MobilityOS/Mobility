;
; PROJECT:    Mobility
; FILE:       Mobility.Runtime.x64.Assembly.asm
; PURPOSE:    Implementation for Mobility Runtime x64 Specific Assembly Parts
;
; LICENSE:    The MIT License
;
; MAINTAINER: MouriNaruto (Kenji.Mouri@outlook.com)
;

.CODE

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformPause();
;------------------------------------------------------------------------------
MoPlatformPause PROC
    pause
    ret
MoPlatformPause ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformHalt();
;------------------------------------------------------------------------------
MoPlatformHalt PROC
    hlt
    ret
MoPlatformHalt ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformDebugBreak();
;------------------------------------------------------------------------------
MoPlatformDebugBreak PROC
    int 3
    ret
MoPlatformDebugBreak ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformDisableInterrupts();
;------------------------------------------------------------------------------
MoPlatformDisableInterrupts PROC
    cli
    ret
MoPlatformDisableInterrupts ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformEnableInterrupts();
;------------------------------------------------------------------------------
MoPlatformEnableInterrupts PROC
    sti
    ret
MoPlatformEnableInterrupts ENDP

;------------------------------------------------------------------------------
; EXTERN_C MO_UINT64 MOAPI MoPlatformReadMsr(
;     _In_ MO_UINT32 Index);
;------------------------------------------------------------------------------
MoPlatformReadMsr PROC
    ; RAX = Value
    ; RCX = Index

    rdmsr
    shl rdx, 32
    or rax, rdx
    ret
MoPlatformReadMsr ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformWriteMsr(
;     _In_ MO_UINT32 Index,
;     _In_ MO_UINT64 Value);
;------------------------------------------------------------------------------
MoPlatformWriteMsr PROC
    ; RCX = Index
    ; RDX = Value

    mov rax, rdx
    shr rdx, 32
    wrmsr
    ret
MoPlatformWriteMsr ENDP

;------------------------------------------------------------------------------
; EXTERN_C MO_UINT64 MOAPI MoPlatformReadCr3();
;------------------------------------------------------------------------------
MoPlatformReadCr3 PROC
    mov rax, cr3
    ret
MoPlatformReadCr3 ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformWriteCr3(
;     _In_ MO_UINT64 Value);
;------------------------------------------------------------------------------
MoPlatformWriteCr3 PROC
    ; RCX = Value

    mov cr3, rcx
    ret
MoPlatformWriteCr3 ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadGlobalDescriptorTable(
;     _In_ PMO_RUNTIME_X64_PSEUDO_DESCRIPTOR Descriptor);
;------------------------------------------------------------------------------
MoPlatformLoadGlobalDescriptorTable PROC
    ; RCX = Descriptor

    lgdt FWORD PTR [rcx]
    ret
MoPlatformLoadGlobalDescriptorTable ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadInterruptDescriptorTable(
;     _In_ PMO_RUNTIME_X64_PSEUDO_DESCRIPTOR Descriptor);
;------------------------------------------------------------------------------
MoPlatformLoadInterruptDescriptorTable PROC
    ; RCX = Descriptor

    lidt FWORD PTR [rcx]
    ret
MoPlatformLoadInterruptDescriptorTable ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformReloadSegmentSelectors(
;     MO_UINT16 DataSelector,
;     MO_UINT16 CodeSelector);
;------------------------------------------------------------------------------
MoPlatformReloadSegmentSelectors PROC
    ; CX = DataSelector
    ; DX = CodeSelector

    ; Set the data segment selector.
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    mov ss, cx

    lea rcx, NewReturnAddress

    ; Set the code segment selector.
    movzx rdx, dx
	push rdx

    ; Set the new return address.
	push rcx

    ; Perform a far return for switch to the new code segment.
	retfq
NewReturnAddress:
	ret
MoPlatformReloadSegmentSelectors ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformLoadTaskRegister(
;     MO_UINT16 TssSelector);
;------------------------------------------------------------------------------
MoPlatformLoadTaskRegister PROC
    ; RCX = TssSelector

    ltr cx
    ret
MoPlatformLoadTaskRegister ENDP

;------------------------------------------------------------------------------
; EXTERN_C VOID MOAPI MoPlatformSwitchToNewStack(
;     MO_POINTER StackAddress,
;     MO_POINTER FunctionAddress);
;------------------------------------------------------------------------------
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

END
