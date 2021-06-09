[bits 64]


; expects pointer to 1st choice in rdi
; expects pointer to 2nd choice in rsi
; expects pointer to bitmask    in rdx (1/0 for each uint32_t in __m128)
global store_u32_epi8mask
store_u32_epi8mask:
    sub      rsp  , 16
    movdqu   [rsp], xmm0  ; push xmm0 to stack
    sub      rsp  , 16
    movdqu   [rsp], xmm8  ; push xmm8 to stack
    sub      rsp  , 16
    movdqu   [rsp], xmm9  ; push xmm9 to stack

    movdqa   xmm8 , [rdi] ; 1st choice
    movdqa   xmm9 , [rsi] ; 2nd choice
    movdqa   xmm0 , [rdx] ; mask
    blendvps xmm9 , xmm8  ; perform conditional
    movdqu   [rdi], xmm9  ; put back in memory location

    movdqu   xmm9 , [rsp] ; pop xmm9
    add      rsp  , 16
    movdqu   xmm8 , [rsp] ; pop xmm8
    add      rsp  , 16
    movdqu   xmm0 , [rsp] ; pop xmm0
    add      rsp  , 16
    ret