; 64-bit SEH header for NASM

; References

; https://msdn.microsoft.com/en-us/library/1eyas8tf.aspx
; https://msdn.microsoft.com/en-us/library/cc704588.aspx
; https://msdn.microsoft.com/en-us/library/aa363082.aspx
; https://www.reactos.org/wiki/Techwiki:SEH64

; This header is placed in the pulbic domain.

; UNWIND_CODE

; List of unwind operation codes

UWOP_PUSH_NONVOL equ 0 ; Push a register value on the stack
UWOP_ALLOC_LARGE equ 1 ; Allocate a large buffer on the stack
UWOP_ALLOC_SMALL equ 2 ; Allocate a small buffer (with a size of 8 to 128 bytes) on the stack
UWOP_SET_FPREG equ 3 ; Set up a stack frame
UWOP_SAVE_NONVOL equ 4 ; Save a register value on the stack by 'mov'
UWOP_SAVE_NONVOL_FAR equ 5 ; Save a register value on the stack with a long offset by 'mov'
UWOP_SAVE_XMM128 equ 8 ; Save a 128-bit XMM register value on the stack by "mov"
UWOP_SAVE_XMM128_FAR equ 9 ; Save a 128-bit XMM register value on the stack with a long offset by 'mov'
UWOP_PUSH_MACHFRAME equ 10

; Operation info: Register constants (unofficial)

OPINFO_RCX equ 1
OPINFO_RDX equ 2
OPINFO_RBX equ 3
OPINFO_RSP equ 4
OPINFO_RBP equ 5
OPINFO_RSI equ 6
OPINFO_RDI equ 7
OPINFO_R8 equ 8
OPINFO_R9 equ 9
OPINFO_R10 equ 10
OPINFO_R11 equ 11
OPINFO_R12 equ 12
OPINFO_R13 equ 13
OPINFO_R14 equ 14
OPINFO_R15 equ 15

; UNWIND_INFO

; Version number of unwind data (unofficial)

UNWIND_VERSION equ 1

; Flag constants

UNW_FLAG_NHANDLER equ 0
UNW_FLAG_EHANDLER equ 1
UNW_FLAG_UHANDLER equ 2
UNW_FLAG_CHAININFO equ 4

; EXCEPTION_RECORD

; Return values
ExceptionContinueExecution equ 0
ExceptionContinueSearch equ 1
ExceptionNestedException equ 2
ExceptionCollidedUnwind equ 3

EXCEPTION_MAXIMUM_PARAMETERS equ 15

; Exception codes

EXCEPTION_ACCESS_VIOLATION equ 0xC0000005
EXCEPTION_ARRAY_BOUNDS_EXCEEDED equ 0xC000008C
EXCEPTION_BREAKPOINT equ 0x80000003
EXCEPTION_DATATYPE_MISALIGNMENT equ 0x80000002
EXCEPTION_FLT_DENORMAL_OPERAND equ 0xC000008D
EXCEPTION_FLT_DIVIDE_BY_ZERO equ 0xC000008E
EXCEPTION_FLT_INEXACT_RESULT equ 0xC000008F
EXCEPTION_FLT_INVALID_OPERATION equ 0xC0000090
EXCEPTION_FLT_OVERFLOW equ 0xC0000091
EXCEPTION_FLT_STACK_CHECK equ 0xC0000092
EXCEPTION_FLT_UNDERFLOW equ 0xC0000093
EXCEPTION_ILLEGAL_INSTRUCTION equ 0xC000001D
EXCEPTION_IN_PAGE_ERROR equ 0xC0000006
EXCEPTION_INT_DIVIDE_BY_ZERO equ 0xC0000094
EXCEPTION_INT_OVERFLOW equ 0xC0000095
EXCEPTION_INVALID_DISPOSITION equ 0xC0000026
EXCEPTION_NONCONTINUABLE_EXCEPTION equ 0xC0000025
EXCEPTION_PRIV_INSTRUCTION equ 0xC0000096
EXCEPTION_SINGLE_STEP equ 0x80000004
EXCEPTION_STACK_OVERFLOW equ 0xC00000FD

; Exception flags
; used as the value of 'x' in the expression "__except(x)"

EXCEPTION_NONCONTINUABLE equ 1
EXCEPTION_CONTINUE_EXECUTION equ (-1)
EXCEPTION_CONTINUE_SEARCH equ 0

struc EXCEPTION_RECORD

    .ExceptionCode: resd 1
    .ExceptionFlags: resd 1
    .ExceptionRecord: resq 1
    .ExceptionAddress: resq 1
    .NumberParameters: resd 1
    resd 1
    .ExceptionInformation resq EXCEPTION_MAXIMUM_PARAMETERS

endstruc

; 'CONTENT' structure

struc CONTEXT

    .P1Home: resq 1
    .P2Home: resq 1
    .P3Home: resq 1
    .P4Home: resq 1
    .P5Home: resq 1
    .P6Home: resq 1
    .ContextFlags: resd 1
    .MxCsr: resd 1
    .SegCs: resw 1
    .SegDs: resw 1
    .SegEs: resw 1
    .SegFs: resw 1
    .SegGs: resw 1
    .SegSs: resw 1
    .EFlags: resd 1
    .Dr0: resq 1
    .Dr1: resq 1
    .Dr2: resq 1
    .Dr3: resq 1
    .Dr6: resq 1
    .Dr7: resq 1
    .Rax: resq 1
    .Rcx: resq 1
    .Rdx: resq 1
    .Rbx: resq 1
    .Rsp: resq 1
    .Rbp: resq 1
    .Rsi: resq 1
    .Rdi: resq 1
    .R8: resq 1
    .R9: resq 1
    .R10: resq 1
    .R11: resq 1
    .R12: resq 1
    .R13: resq 1
    .R14: resq 1
    .R15: resq 1
    .Rip: resq 1
    .FltSave:
    .FloatSave:
    .Header: resq 2 * 2
    .Legacy: resq 2 * 8
    .Xmm0: resq 2
    .Xmm1: resq 2
    .Xmm2: resq 2
    .Xmm3: resq 2
    .Xmm4: resq 2
    .Xmm5: resq 2
    .Xmm6: resq 2
    .Xmm7: resq 2
    .Xmm8: resq 2
    .Xmm9: resq 2
    .Xmm10: resq 2
    .Xmm11: resq 2
    .Xmm12: resq 2
    .Xmm13: resq 2
    .Xmm14: resq 2
    .Xmm15: resq 2
    resd 24
    .VectorRegister: resq 2 * 26
    .VectorControl: resq 1
    .DebugControl: resq 1
    .LastBranchToRip: resq 1
    .LastBranchFromRip: resq 1
    .LastExceptionToRip: resq 1
    .LastExceptionFromRip: resq 1

endstruc

; Structures for exception handling

struc RUNTIME_FUNCTION

    ; A 'RUNTIME_FUNCTION' structure should be aligned to 'DWORD' in memory.

    .BeginAddress: resd 1 ; Relative address of the beginning of the function
    .EndAddress: resd 1 ; Relative address of the end
    .UnwindData: resd 1 ; Relative address of the unwind infomation

endstruc

struc UNWIND_INFO

	; A 'UNWIND_INFO' structure should be aligned to 'DWORD' in memory.

    .Version: ; Version of this unwind data (3-bit)
    .Flags: resb 1 ; Exception handler flags (5-bit)
    .SizeOfProlog: resb 1 ; Size of the Prologue
    .CountOfCodes: resb 1 ; Length of the unwind code array
    .FrameRegister: ; Stack frame register information
    .FrameOffset: resb 1 ; Offset of the frame register scaled by 16 (4-bit)

    .UnwindCode: resw 1 ; Unwind code array (The length is variable.)
    .ExceptionHandler: resd 1 ; (Optional) This should be present when if 'Flags' is set to 'UNW_FLAG_EHANDLER' or 'UNW_FLAG_UHANDLER' or both.
    .ExceptionData resd 1 ; (Optional) The 'DISPATCHER_CONTEXT' has a pointer to this member.

endstruc

struc UNWIND_CODE

    CodeOffset: resb 1 ; Offset in the prologue
    UnWindOp: ; Operation code (4-bit)
    OpInfo: resb 1 ; Operation info (4-bit)

    FrameOffset: resw 2 ; (Optional: Used by some operation codes)

endstruc
