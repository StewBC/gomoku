;---------------------------------------------------------------------------
; Gomoku By Stefam Wessels, 2023
; This is free and unencumbered software released into the public domain.
;---------------------------------------------------------------------------

.include "zeropage.inc"

;-----------------------------------------------------------------------
.export _asm_ClearScreen, _asm_ShowFont, _asm_ShowTile, _asm_DecompSplash

.import popa, popax
.import DECOMPRESS_LZSA2, LZSA_DST, LZSA_SRC

;-----------------------------------------------------------------------
.define BITMAP_ADDRESS			$8000
.define LUMINANCE_ADDERSS		$0800
.define COLOR_ADDERSS           LUMINANCE_ADDERSS + $0400

;-----------------------------------------------------------------------------
.segment "RODATA"

_splash_screen:
.incbin "Gomoku.lzh"

; Tables to look up every pixel row start address
rowL:
    .repeat $C8, Row
        .byte <(BITMAP_ADDRESS + (Row / 8) * 320 + (Row .mod 8))
    .endrep

rowH:
    .repeat $C8, Row
        .byte >(BITMAP_ADDRESS + (Row / 8) * 320 + (Row .mod 8))
    .endrep

; Tables to look up every col start address
colL:
    .repeat $28, Col
        .byte <(Col*8)
    .endrep

colH:
    .repeat $28, Col
        .byte >(Col*8)
    .endrep

; font masks
fontMasks:
.byte %11111111    ; Black
.byte %01010101    ; White
.byte %10101010    ; Yellow

;-----------------------------------------------------------------------
.code

;-----------------------------------------------------------------------
.proc _asm_ClearScreen

    lda #0
    tay
    sta ptr1
    lda #>BITMAP_ADDRESS
    sta ptr1+1
    ldx #(8192/256)

    tya
:
    sta (ptr1),y
    dey
    bne :-
    inc ptr1+1
    dex
    bne :-

done:
    rts

.endproc

;-----------------------------------------------------------------------
.proc _asm_ShowFont
    sta charSrc1+1 ; odd half lo
    sta charSrc2+1 ; even half lo
    stx charSrc1+2 ; odd half hi
    stx charSrc2+2 ; even half hi

    jsr popa       ; color
    tay
    lda fontMasks,y
    sta ptr2

    jsr popa       ; col
    asl            ; font is 2 cols wide
    tay
    lda colL,y
    sta tmp2
    lda colH,y
    sta tmp3

    jsr popa       ; row
    sta tmp1

    lda #8
    sta tmp4       ; font height
    ldx #0

font_loop:
    ldy tmp1       ; re load ptr1 with
    lda rowL,y     ; row base and col offset
    adc tmp2
    sta ptr1
    lda rowH,y
    adc tmp3
    sta ptr1+1
charSrc1:
    lda $ffff,x    ; left character
    and ptr2       ; mask for color
    ldy #0
    sta (ptr1),y
    inx            ; go to right character
charSrc2:
    lda $ffff,x    ; right character
    and ptr2       ; same mask for color
    ldy #8         ; and put along side
    sta (ptr1),y
    inx            ; to next left
    dec tmp4       ; for the height of the font
    beq done
    inc tmp1       ; next row
    bne font_loop  ; BRA
done:
    rts
.endproc

;-----------------------------------------------------------------------
.proc _asm_ShowTile
    sta charSrc2+1 ; odd half lo
    stx charSrc2+2 ; odd half hi

    jsr popax
    sta charSrc1+1 ; even half lo
    stx charSrc1+2 ; even half hi

    jsr popa       ; col
    asl            ; font is 2 cols wide
    tay
    lda colL,y
    sta tmp2
    lda colH,y
    sta tmp3

    clc
    jsr popa       ; row
    asl
    sta tmp1
    asl 
    asl 
    adc tmp1
    sta tmp1

    lda #10
    sta tmp4       ; tile height
    ldx #0

tile_loop:         ; same as font loop but no masking
    ldy tmp1
    lda rowL,y
    adc tmp2
    sta ptr1
    lda rowH,y
    adc tmp3
    sta ptr1+1
charSrc1:
    lda $ffff,x
    ldy #0
    sta (ptr1),y
charSrc2:
    lda $ffff,x
    ldy #8
    sta (ptr1),y
    inx
    dec tmp4
    beq done
    inc tmp1
    bne tile_loop  ; BRA
done:
    rts
.endproc

;-----------------------------------------------------------------------------
.proc _asm_DecompSplash
    lda	#<_splash_screen
    sta	LZSA_SRC+1
    lda	#>_splash_screen
    sta	LZSA_SRC+2
    lda	#<BITMAP_ADDRESS
    sta	LZSA_DST+1
    lda	#>BITMAP_ADDRESS
    sta	LZSA_DST+2
    jmp	DECOMPRESS_LZSA2
.endproc
