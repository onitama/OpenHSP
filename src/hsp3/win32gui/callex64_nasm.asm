; 64-bit 'CallFunc'
;
; Kurogoma 08/26/2016
;
; This NASM script is in the pulbic domain.

bits 64

%include "sehhdr64_nasm.h"

global CallFunc64

section .text


; CallFunc64(INT_PTR * arguments, FARPROC externalFunction, int numberOfArguments)
; This function calls a external function.
; arguments:            Argument or arguments to pass to the function of 'externalFunction'
; externalFunction:     Function to call
; numberOfArguments:    Number of the argument or arguments of 'arguments'
CallFunc64:

    push rbp
    .pushRBP:

    mov rbp, rsp
    .setFrame:

    .endPrologue:

    mov r10, rdx

    ; Put the 'arguments' on the stack

    mov eax, r8d
    sal eax, 3
    .@IfLabel00:
    jle .@EndIfLabel00
        xor eax, eax
    .@EndIfLabel00:

    sub rax, 8*4
    .@IfLabel10:
    jge .@IfLabel10_Else

        add rsp, rax
        jmp .@EndIfLabel10

    .@IfLabel10_Else:

        and eax, 8
        sub rsp, rax

    .@EndIfLabel10:

    jmp .@00
    .@DoLabel_00:

       push qword [rcx + r8*8]

       .@00:
       dec r8d
       jge .@DoLabel_00

    .@UntilLabel_00:

    ; Put the first 4 arguments into registers

    pop rcx
    pop rdx
    pop r8
    pop r9

    sub rsp, 8*4

    movq xmm0, rcx
    movq xmm1, rdx
    movq xmm2, r8
    movq xmm3, r9

    call r10

    leave
    ret
    .endFunc:


section .pdata

    ; Funciton table for exception handling
    TableOfSEHRuntimeFunctionStructs:

        .CallFunc64:
            dd CallFunc64
            dd CallFunc64.endFunc
            dd TableOfSEHUnwindInfoStructs.CallFunc64


section .xdata

    ; Unwind data information
    TableOfSEHUnwindInfoStructs:

        .CallFunc64:

            db UNWIND_VERSION | (UNW_FLAG_NHANDLER << 3)
            db CallFunc64.endPrologue - CallFunc64  ; Size of the Prologue
            db (TableOfSEHUnwindInfoStructs.CallFunc64_unwindCodesTo - TableOfSEHUnwindInfoStructs.CallFunc64_unwindCodesFrom)/2  ; Length of the unwind code array
            db OPINFO_RBP | (0 << 4)   ; Info of the stack frame (lea rbp, [rsp + 0*16])

            ; Unwind code arrray

            .CallFunc64_unwindCodesFrom:

            db CallFunc64.setFrame - CallFunc64
            db UWOP_SET_FPREG

            db CallFunc64.pushRBP - CallFunc64
            db UWOP_PUSH_NONVOL + (OPINFO_RBP << 4)

            .CallFunc64_unwindCodesTo: