align 0x1000
section .text
global ApPrepare64
global ApStart64

global farPointer64
global ApStartGdtEntries
global ApStartGdtCs_48_55


[bits 32]
ApPrepare64:
    mov byte [ecx + ApStartGdtCs_48_55], 0xA0 ; Update GDT to kernel segment (for 64-bit)

    ; Enable PAE
    mov eax, cr4
    or  eax, 0x20
    mov cr4, eax


    ; Load PML4
    mov ebx, [ecx + local_targs + trampoline_data.pml4]
    mov cr3, ebx
    
    mov esi, ecx

    xor ecx, ecx
    xor edx, edx
    xor eax, eax


    ; IA32_EFER MSR: Enable LME, SCE. bit 11 - Long Mode Enable; bit 1 - Syscall Extensions.
    mov ecx, 0xC0000080
    rdmsr
    or  eax, (1 << 8) | (1 << 11)
    wrmsr


    ; Enable - PG, WP. Disable - CD, NW.
    mov ecx, cr0
    or  ecx, 0x80010000 ; bits 31, 16 - Enable Paging (True),  Write Protect     (True).
    and ecx, 0x9fffffff ; bits 30, 29 - Cache Disable (False), Not-write through (False).
    mov cr0, ecx

    jmp far dword [esi + farPointer64]




[bits 64]
ApStart64:
    ; Notify the BSP we are good to go, and that we're part of the cool kids now >:)
    mov byte [esi + local_targs + trampoline_data.ready], 0x21 ; ! symbol
 

    ; Update to 64 bit GDT
    mov rbx, qword [esi + local_targs + trampoline_data.gdt64]
    lgdt [rbx]


    ; Update to 64 bit IDT
    mov rcx, qword [esi + local_targs + trampoline_data.idt64]
    lidt [rcx]


    ; Setup new 64-bit Stack
    mov rbp, qword [esi + local_targs + trampoline_data.rbp64]
    mov rsp, rbp


    ; ; Setup TSS Segment.
    ; mov ax, word [esi + local_targs + trampoline_data.tssOff]
    ; ltr ax


    ; Prepare to jump to smp_init at higher half.
    ; first func argument.
    mov rdi, local_targs
    add edi, esi
    
    ; reset segment registers.
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov rbx, qword [esi + local_targs + trampoline_data.entry]
    jmp qword rbx



struc trampoline_data
    .ready:   resb 1
    .lock:    resb 1
    .tssOff:  resw 1
    .pml4:    resd 1
    .rbp64:   resq 1
    .gdt64:   resq 1
    .idt64:   resq 1
    .entry:   resq 1
endstruc

align 8
ApStartGdtEntries:
    ; NULL - 0x00
    ApStartGdtNull_0_31  dd 0
    ApStartGdtNull_32_63 dd 0

    ; Kernel code segment (CS) - 0x08
    ApStartGdtCs_0_15  dw 0xFFFF
    ApStartGdtCs_16_31 dw 0
    ApStartGdtCs_32_39 db 0
    ApStartGdtCs_40_47 db 0x9A
    ApStartGdtCs_48_55 db 0xCF
    ApStartGdtCs_56_63 db 0

    ; Kernel data segment (DS) - 0x10
    ApStartGdtDs_0_15  dw 0xFFFF
    ApStartGdtDs_16_31 dw 0
    ApStartGdtDs_32_39 db 0
    ApStartGdtDs_40_47 db 0x92
    ApStartGdtDs_48_55 db 0xCF
    ApStartGdtDs_56_63 db 0


align 8
farPointer64:
    .address: dd ApStart64
    .segment: dw 0x08



section .bss
    global local_targs
    local_targs: 
        resb 40