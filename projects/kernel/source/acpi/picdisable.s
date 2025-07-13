[bits 64]

global pic_disable_asm

pic_disable_asm:
    push ax
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
    pop ax
    ret