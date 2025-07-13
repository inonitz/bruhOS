align 0x1000
section .text
[bits 16]


global ApStart16
ApStart16:
    cli 


    ; Get the Trampoline Address
    mov ecx, cs
    shl ecx, 4


    ; Setup Temporary Stack
    mov esp, ecx
    add esp, SMP_STACK + 0x400
    mov ebp, esp


    ; 32 bit GDT
    lgdt [cs:ApStartGdt] 


    ; Enter Protected Mode (PE Bit in cr0)
    mov eax, cr0
    or  eax, 0x1
    mov cr0, eax
    xor eax, eax


    ; far jump to protected mode, flush 16-bit instructions.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; Because of endianess, the address and segment are reversed. (farPointer32)
    jmp far dword [cs:farPointer32]



section .data
extern ApPrepare64
extern ApStartGdtEntries
global ApStartGdt
global farPointer32


align 4
ApStartGdt:
    .size: dw 23
    .base: dd ApStartGdtEntries

align 4
farPointer32:
    .address: dd ApPrepare64
    .segment: dw 0x08

align 16
SMP_STACK:
    resb 0x400