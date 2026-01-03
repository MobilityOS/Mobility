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

END
