; 64-bit 'CallFunc'
;
; LICENCE: Public domain

bits 64

%include "sehhdr64_nasm.h"

global CallFunc64

section .text

; [CallFunc64]
;
; The 'CallFunc64' function calls the sppecified function with the specified array as arguments.
;
; [Syntax]
;
; INT_PTR CallFunc64(INT_PTR const *arguments, FARPROC external_function, int number_Of_arguments);
; INT32 CallFunc64(INT_PTR const *arguments, FARPROC external_function, int number_Of_arguments);
; double CallFunc64(INT_PTR const *arguments, FARPROC external_function, int number_of_arguments);
;
; [Parameters]
;
; INT_PTR const *arguments [in, optional]
;     Points to the array used as arguments to the function of 'externalFunction'. If 'numberOfArguments' is set to a negative value or zero, the function makes no use of the parameter.
;
; FARPROC external_function [in]
;     Points to the function to call with the array of 'arguments' as arguments.
;
; int number_of_arguments [in]
;     Represents the number of elements in the array of 'arguments'.
;
; [Return value]
;
; The return value is directly from the function of 'externalFunction'.
;
; [Remarks]
;
; If 'numberOfParameters' is set to a negative value, the behavior is the same as when the parameter is set to zero; the function of 'externalFunction' is then assumed not to receive any parameter.
; The behavior is undefined if the array of 'arguments' is not proper as arguments for the function of 'externalFunction'.

CallFunc64:

    mov rax, rdx ; Save the value of 'external_function' to rax

    ; Directly go to the function of 'external_function' if it does not receive any parameter

    mov edx, r8d
    test edx, edx
    jg short .@has_args

        jmp rax

    .@has_args:

    push rbp
    .push_rbp:

    mov rbp, rsp
    .set_frame:

    .end_of_prologue:

    sal edx, byte 3 ; edx = number_of_arguments * sizeof(INT_PTR)

    ; Align the stack to 16 bytes and allocate at least 32 bytes

    sub edx, byte 8 * 4
    .@if00:
    jge short .@else00

        neg edx
        jmp short .@end_if00

    .@else00:

        and edx, byte 8

    .@end_if00:

    sub rsp, rdx

    ; Prepare arguments

    .@do00:

       dec r8d
       push qword [rcx + r8 * 8]

       jg .@do00

    .@until00:

    pop rcx
    pop rdx
    pop r8
    pop r9

    movq xmm0, rcx
    movq xmm1, rdx
    movq xmm2, r8
    movq xmm3, r9

    ; cmp rsp, rbp

    ; Call the function of 'external_function'

    sub rsp, byte 8 * 4
    call rax

    leave
    ret
    .endFunc:

section .pdata

    ; Funciton table for exception handling
    TableOfSEHFunctionStructs:

        .CallFunc64:
            dd CallFunc64
            dd CallFunc64.endFunc
            dd TableOfSEHUnwindInfoStructs.CallFunc64

section .xdata

    ; Unwind data information
    TableOfSEHUnwindInfoStructs:

        .CallFunc64:

            db UNWIND_VERSION | (UNW_FLAG_NHANDLER << 3)
            db CallFunc64.end_of_prologue - CallFunc64 ; Size of the Prologue
            db (TableOfSEHUnwindInfoStructs.CallFunc64_unwindCodesTo - TableOfSEHUnwindInfoStructs.CallFunc64_unwindCodesFrom) / 2 ; Length of the unwind code array
            db OPINFO_RBP | (0 << 4) ; Size of the stack frame (lea rbp, [rsp + 0*16])

            ; Unwind code arrray

            .CallFunc64_unwindCodesFrom:

            db CallFunc64.set_frame - CallFunc64
            db UWOP_SET_FPREG

            db CallFunc64.push_rbp - CallFunc64
            db UWOP_PUSH_NONVOL + (OPINFO_RBP << 4)

            .CallFunc64_unwindCodesTo:
