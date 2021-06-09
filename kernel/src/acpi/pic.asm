

global pic_disable

pic_disable:
    push ax
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
    pop ax
    ret