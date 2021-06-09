[bits 64]

; interrupt handler for irq's and exceptions.
; gcc cannot be used because it fucks with the handler addresses 
; (-mgeneral-regs-only uses 16bit registers => 16bit addresses instead of 64)
; so, this is an assembly stub that calls the appropriate interrupt handler.


%macro push_gpregs 0
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11

    mov rdi, cr0
    push rdi
    mov rdi, cr2
    push rdi
    mov rdi, cr3
    push rdi
    mov rdi, cr4
    push rdi
    mov rdi, cr8
    push rdi
%endmacro
%macro pop_gpregs 0
    pop rax ; doesn't matter which register I use, =
    pop rax ; as long as I retrieve it to its original state.
    pop rax ; Popping the CRn register that I pushed previously. 
    pop rax ; ^^^^
    pop rax ; ^^^^
    
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rax
%endmacro




extern interrupt_handler


%macro interrupt_handler_with_ecode_gate 1
global ie_handler%1
ie_handler%1:
    push qword %1
    push_gpregs

    mov rdi, rsp 
    call interrupt_handler
    
    pop_gpregs
    add rsp, 16
    iretq
%endmacro


%macro interrupt_handler_no_ecode_gate 1
global inoe_handler%1
inoe_handler%1:
    push qword 0xffff
    push qword %1
    push_gpregs

    mov rdi, rsp 
    call interrupt_handler
    
    pop_gpregs
    add rsp, 16
    iretq
%endmacro


; %macro push_simdregs 0
;     sub rsp 512
;     fxsave [rsp]
; %endmacro

; %macro pop_simdregs 0
;     fxrstor [rsp]
;     add rsp 512
; %endmacro

; global test_handler
; test_handler:
;     push qword 0xffff
;     push qword 0x69
;     push_gpregs
;     mov rdi, rsp 
;     call interrupt_handler 
;     pop_gpregs
;     add rsp, 16  
;     iretq


; table is in https://wiki.osdev.org/Exceptions
interrupt_handler_no_ecode_gate   0
interrupt_handler_no_ecode_gate   1
interrupt_handler_no_ecode_gate   2
interrupt_handler_no_ecode_gate   3
interrupt_handler_no_ecode_gate   4
interrupt_handler_no_ecode_gate   5
interrupt_handler_no_ecode_gate   6
interrupt_handler_no_ecode_gate   7
interrupt_handler_with_ecode_gate 8
interrupt_handler_with_ecode_gate 10
interrupt_handler_with_ecode_gate 11
interrupt_handler_with_ecode_gate 12
interrupt_handler_with_ecode_gate 13
interrupt_handler_with_ecode_gate 14
interrupt_handler_no_ecode_gate   16
interrupt_handler_with_ecode_gate 17
interrupt_handler_no_ecode_gate   18
interrupt_handler_no_ecode_gate   19
interrupt_handler_no_ecode_gate   20
interrupt_handler_with_ecode_gate 30


interrupt_handler_no_ecode_gate 32
interrupt_handler_no_ecode_gate 33
interrupt_handler_no_ecode_gate 34
interrupt_handler_no_ecode_gate 35
interrupt_handler_no_ecode_gate 36
interrupt_handler_no_ecode_gate 37
interrupt_handler_no_ecode_gate 38
interrupt_handler_no_ecode_gate 39
interrupt_handler_no_ecode_gate 40
interrupt_handler_no_ecode_gate 41
interrupt_handler_no_ecode_gate 42
interrupt_handler_no_ecode_gate 43
interrupt_handler_no_ecode_gate 44
interrupt_handler_no_ecode_gate 45
interrupt_handler_no_ecode_gate 46
interrupt_handler_no_ecode_gate 47
interrupt_handler_no_ecode_gate 48
interrupt_handler_no_ecode_gate 49
interrupt_handler_no_ecode_gate 50
interrupt_handler_no_ecode_gate 51
interrupt_handler_no_ecode_gate 52
interrupt_handler_no_ecode_gate 53
interrupt_handler_no_ecode_gate 54
interrupt_handler_no_ecode_gate 55
interrupt_handler_no_ecode_gate 56
interrupt_handler_no_ecode_gate 57
interrupt_handler_no_ecode_gate 58
interrupt_handler_no_ecode_gate 59
interrupt_handler_no_ecode_gate 60
interrupt_handler_no_ecode_gate 61
interrupt_handler_no_ecode_gate 62
interrupt_handler_no_ecode_gate 63
interrupt_handler_no_ecode_gate 64
interrupt_handler_no_ecode_gate 65
interrupt_handler_no_ecode_gate 66
interrupt_handler_no_ecode_gate 67
interrupt_handler_no_ecode_gate 68
interrupt_handler_no_ecode_gate 69
interrupt_handler_no_ecode_gate 70
interrupt_handler_no_ecode_gate 71
interrupt_handler_no_ecode_gate 72
interrupt_handler_no_ecode_gate 73
interrupt_handler_no_ecode_gate 74
interrupt_handler_no_ecode_gate 75
interrupt_handler_no_ecode_gate 76
interrupt_handler_no_ecode_gate 77
interrupt_handler_no_ecode_gate 78
interrupt_handler_no_ecode_gate 79
interrupt_handler_no_ecode_gate 80
interrupt_handler_no_ecode_gate 81
interrupt_handler_no_ecode_gate 82
interrupt_handler_no_ecode_gate 83
interrupt_handler_no_ecode_gate 84
interrupt_handler_no_ecode_gate 85
interrupt_handler_no_ecode_gate 86
interrupt_handler_no_ecode_gate 87
interrupt_handler_no_ecode_gate 88
interrupt_handler_no_ecode_gate 89
interrupt_handler_no_ecode_gate 90
interrupt_handler_no_ecode_gate 91
interrupt_handler_no_ecode_gate 92
interrupt_handler_no_ecode_gate 93
interrupt_handler_no_ecode_gate 94
interrupt_handler_no_ecode_gate 95
interrupt_handler_no_ecode_gate 96
interrupt_handler_no_ecode_gate 97
interrupt_handler_no_ecode_gate 98
interrupt_handler_no_ecode_gate 99
interrupt_handler_no_ecode_gate 100
interrupt_handler_no_ecode_gate 101
interrupt_handler_no_ecode_gate 102
interrupt_handler_no_ecode_gate 103
interrupt_handler_no_ecode_gate 104
interrupt_handler_no_ecode_gate 105
interrupt_handler_no_ecode_gate 106
interrupt_handler_no_ecode_gate 107
interrupt_handler_no_ecode_gate 108
interrupt_handler_no_ecode_gate 109
interrupt_handler_no_ecode_gate 110
interrupt_handler_no_ecode_gate 111
interrupt_handler_no_ecode_gate 112
interrupt_handler_no_ecode_gate 113
interrupt_handler_no_ecode_gate 114
interrupt_handler_no_ecode_gate 115
interrupt_handler_no_ecode_gate 116
interrupt_handler_no_ecode_gate 117
interrupt_handler_no_ecode_gate 118
interrupt_handler_no_ecode_gate 119
interrupt_handler_no_ecode_gate 120
interrupt_handler_no_ecode_gate 121
interrupt_handler_no_ecode_gate 122
interrupt_handler_no_ecode_gate 123
interrupt_handler_no_ecode_gate 124
interrupt_handler_no_ecode_gate 125
interrupt_handler_no_ecode_gate 126
interrupt_handler_no_ecode_gate 127
interrupt_handler_no_ecode_gate 128
interrupt_handler_no_ecode_gate 129
interrupt_handler_no_ecode_gate 130
interrupt_handler_no_ecode_gate 131
interrupt_handler_no_ecode_gate 132
interrupt_handler_no_ecode_gate 133
interrupt_handler_no_ecode_gate 134
interrupt_handler_no_ecode_gate 135
interrupt_handler_no_ecode_gate 136
interrupt_handler_no_ecode_gate 137
interrupt_handler_no_ecode_gate 138
interrupt_handler_no_ecode_gate 139
interrupt_handler_no_ecode_gate 140
interrupt_handler_no_ecode_gate 141
interrupt_handler_no_ecode_gate 142
interrupt_handler_no_ecode_gate 143
interrupt_handler_no_ecode_gate 144
interrupt_handler_no_ecode_gate 145
interrupt_handler_no_ecode_gate 146
interrupt_handler_no_ecode_gate 147
interrupt_handler_no_ecode_gate 148
interrupt_handler_no_ecode_gate 149
interrupt_handler_no_ecode_gate 150
interrupt_handler_no_ecode_gate 151
interrupt_handler_no_ecode_gate 152
interrupt_handler_no_ecode_gate 153
interrupt_handler_no_ecode_gate 154
interrupt_handler_no_ecode_gate 155
interrupt_handler_no_ecode_gate 156
interrupt_handler_no_ecode_gate 157
interrupt_handler_no_ecode_gate 158
interrupt_handler_no_ecode_gate 159
interrupt_handler_no_ecode_gate 160
interrupt_handler_no_ecode_gate 161
interrupt_handler_no_ecode_gate 162
interrupt_handler_no_ecode_gate 163
interrupt_handler_no_ecode_gate 164
interrupt_handler_no_ecode_gate 165
interrupt_handler_no_ecode_gate 166
interrupt_handler_no_ecode_gate 167
interrupt_handler_no_ecode_gate 168
interrupt_handler_no_ecode_gate 169
interrupt_handler_no_ecode_gate 170
interrupt_handler_no_ecode_gate 171
interrupt_handler_no_ecode_gate 172
interrupt_handler_no_ecode_gate 173
interrupt_handler_no_ecode_gate 174
interrupt_handler_no_ecode_gate 175
interrupt_handler_no_ecode_gate 176
interrupt_handler_no_ecode_gate 177
interrupt_handler_no_ecode_gate 178
interrupt_handler_no_ecode_gate 179
interrupt_handler_no_ecode_gate 180
interrupt_handler_no_ecode_gate 181
interrupt_handler_no_ecode_gate 182
interrupt_handler_no_ecode_gate 183
interrupt_handler_no_ecode_gate 184
interrupt_handler_no_ecode_gate 185
interrupt_handler_no_ecode_gate 186
interrupt_handler_no_ecode_gate 187
interrupt_handler_no_ecode_gate 188
interrupt_handler_no_ecode_gate 189
interrupt_handler_no_ecode_gate 190
interrupt_handler_no_ecode_gate 191
interrupt_handler_no_ecode_gate 192
interrupt_handler_no_ecode_gate 193
interrupt_handler_no_ecode_gate 194
interrupt_handler_no_ecode_gate 195
interrupt_handler_no_ecode_gate 196
interrupt_handler_no_ecode_gate 197
interrupt_handler_no_ecode_gate 198
interrupt_handler_no_ecode_gate 199
interrupt_handler_no_ecode_gate 200
interrupt_handler_no_ecode_gate 201
interrupt_handler_no_ecode_gate 202
interrupt_handler_no_ecode_gate 203
interrupt_handler_no_ecode_gate 204
interrupt_handler_no_ecode_gate 205
interrupt_handler_no_ecode_gate 206
interrupt_handler_no_ecode_gate 207
interrupt_handler_no_ecode_gate 208
interrupt_handler_no_ecode_gate 209
interrupt_handler_no_ecode_gate 210
interrupt_handler_no_ecode_gate 211
interrupt_handler_no_ecode_gate 212
interrupt_handler_no_ecode_gate 213
interrupt_handler_no_ecode_gate 214
interrupt_handler_no_ecode_gate 215
interrupt_handler_no_ecode_gate 216
interrupt_handler_no_ecode_gate 217
interrupt_handler_no_ecode_gate 218
interrupt_handler_no_ecode_gate 219
interrupt_handler_no_ecode_gate 220
interrupt_handler_no_ecode_gate 221
interrupt_handler_no_ecode_gate 222
interrupt_handler_no_ecode_gate 223
interrupt_handler_no_ecode_gate 224
interrupt_handler_no_ecode_gate 225
interrupt_handler_no_ecode_gate 226
interrupt_handler_no_ecode_gate 227
interrupt_handler_no_ecode_gate 228
interrupt_handler_no_ecode_gate 229
interrupt_handler_no_ecode_gate 230
interrupt_handler_no_ecode_gate 231
interrupt_handler_no_ecode_gate 232
interrupt_handler_no_ecode_gate 233
interrupt_handler_no_ecode_gate 234
interrupt_handler_no_ecode_gate 235
interrupt_handler_no_ecode_gate 236
interrupt_handler_no_ecode_gate 237
interrupt_handler_no_ecode_gate 238
interrupt_handler_no_ecode_gate 239
interrupt_handler_no_ecode_gate 240
interrupt_handler_no_ecode_gate 241
interrupt_handler_no_ecode_gate 242
interrupt_handler_no_ecode_gate 243
interrupt_handler_no_ecode_gate 244
interrupt_handler_no_ecode_gate 245
interrupt_handler_no_ecode_gate 246
interrupt_handler_no_ecode_gate 247
interrupt_handler_no_ecode_gate 248
interrupt_handler_no_ecode_gate 249
interrupt_handler_no_ecode_gate 250
interrupt_handler_no_ecode_gate 251
interrupt_handler_no_ecode_gate 252
interrupt_handler_no_ecode_gate 253
interrupt_handler_no_ecode_gate 254
interrupt_handler_no_ecode_gate 255