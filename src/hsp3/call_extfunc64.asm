; 64-bit 'CallFunc'
;
; LICENCE: Public domain

OPTION CASEMAP: NONE

PUBLIC CallFunc64

.CODE

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

CallFunc64 PROC FRAME

    mov rax, rdx ; Save the value of 'external_function' to rax

    ; Directly go to the function of 'external_function' if it does not receive any parameter

    mov edx, r8d
    test edx, edx
    jg short @has_args

        jmp rax

    @has_args:

    push rbp
    .PUSHREG rbp

    mov rbp, rsp
    .SETFRAME rbp, 0

    .ENDPROLOG

    sal edx, 3 ; edx = number_of_arguments * sizeof(INT_PTR)

    ; Align the stack to 16 bytes and allocate at least 32 bytes

    sub edx, 8 * 4
    @if00:
    jge short @else00

        neg edx
        jmp short @end_if00

    @else00:

        and edx, 8

    @end_if00:

    sub rsp, rdx

    ; Prepare arguments

    @do00:

       dec r8d
       push qword ptr [rcx + r8 * 8]

       jg @do00

    @until00:

    pop rcx
    pop rdx
    pop r8
    pop r9

    movd xmm0, rcx
    movd xmm1, rdx
    movd xmm2, r8
    movd xmm3, r9

    ; cmp rsp, rbp

    ; Call the function of 'external_function'

    sub rsp, 8 * 4
    call rax

    leave
    ret

CallFunc64 ENDP

END