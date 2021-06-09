section .text

global rmodeStartAp
global rmodeEndAp


GDT:
    .Null:
        dq 0x0000000000000000             ; Null Descriptor - should be present.
    
    .Code:
        dq 0x00209A0000000000             ; 64-bit code descriptor (exec/read).
        dq 0x0000920000000000             ; 64-bit data descriptor (read/write).
    
    ALIGN 4
        dw 0                              ; Padding to make the "address of the GDT" field aligned on a 4-byte boundary

    .Pointer:
        dw $ - GDT - 1                    ; 16-bit Size (Limit) of GDT.
        dq GDT                            ; 32-bit Base Address of GDT. (CPU will zero extend to 64-bit)


bits 16

rmodeStartAp:
    cli
    cld
    mov ax, 0
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov eax, cr4
    or eax, 1 << 5              ; Set PAE bit
    mov cr4, eax

    mov ecx, 0xB000
    mov cr3, ecx

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    or eax, 1 << 11
    wrmsr

    mov eax, cr0
    or eax, 0x80000001
    mov cr0, eax

    lgdt [GDT.Pointer]
    jmp dword 0x08:0xA000

    cli
    hlt
rmodeEndAp:

bits 64

global lmodeStartAp
global lmodeEndAp

lmodeStartAp:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rax, cr0
    and ax, 0xFFFB                  ; Clear coprocessor emulation CR0.EM
    or ax, 0x2                     ; Set coprocessor monitoring  CR0.MP
    mov cr0, rax

    mov rax, cr4
    or ax, 3 << 9                  ; Set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, rax

    mov rsp, qword [0xC000]
    mov rbp, 0
    push 0x08
    push HalApEntry
    mov byte [0xC008], 1
    retfq
lmodeEndAp:

extern HalApEntry