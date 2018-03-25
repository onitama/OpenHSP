; 64-bit 'CallFunc'
;
; Kurogoma 08/26/2016
;
; This MASM script is in the pulbic domain.

OPTION CASEMAP: NONE

INT_PTR     TYPEDEF PTR
FARPROC     TYPEDEF PTR

PUBLIC CallFunc64

.CODE

; CallFunc64(INT_PTR * arguments , FARPROC externalFunction, int numbrofArguments)
; This function calls a external function.
; argumtnts:            Argument or arguments to pass to the funciton of 'externalFunction'
; externalFunciton:     Function to call
; numberOgArguments:    Number of the argument or arguments of 'arguments'
CallFunc64 PROC FRAME   arguments: PTR INT_PTR, externalFunction: FARPROC, numberOfArguments:SDWORD

    push rbp
    .PUSHREG rbp
    mov rbp, rsp
    .SETFRAME rbp, 0

    .ENDPROLOG

    mov r10, rdx

    ; Put the 'arguments' on the stack

    mov eax, r8d
    sal eax, 3
    @IfLabel00:
    jge @EndIfLabel00
        xor eax, eax
    @EndIfLabel00:

    sub rax, 8*4
    @IfLabel10:
    jge @IfLabel10_Else

        add rsp, rax
        jmp @EndIfLabel10

    @IfLabel10_Else:

        and eax, 8
        sub rsp, rax

    @EndIfLabel10:

    jmp @F
    @DoLabel_00:

        push QWORD PTR [rcx + r8*8]

        @@:
        dec r8d
        jge @DoLabel_00

    @UntilLabel_00:

    ; Put the first 4 arguments into registers

;    mov rax, rsp

    pop rcx
    pop rdx
    pop r8
    pop r9

    sub rsp, 8*4

    ; Note: Why can't MASM assenble these codes?
    db 66h, 48h, 0fh, 6eh, 0c1h ; movq xmm0, rcx
    db 66h, 48h, 0fh, 6eh, 0cah ; movq xmm1, rdx
    db 66h, 48h, 0fh, 6eh, 0d0h ; movq xmm2, r8
    db 66h, 48h, 0fh, 6eh, 0d9h ; movq xmm3, r9

;    mov rsp, rax

    call r10

    leave
    ret 0

CallFunc64 ENDP


END