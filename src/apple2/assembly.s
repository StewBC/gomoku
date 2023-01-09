;---------------------------------------------------------------------------
; Gomoku By Stefam Wessels, 2023
; This is free and unencumbered software released into the public domain.
;---------------------------------------------------------------------------

.export _asm_DetectMachine, _splash_screen, _asm_ClearHiRes, _asm_ShowTile
.export _asm_ShowFont, _asm_InvertSquare, _asm_DecompSplash

.include "zeropage.inc"
.import popa, popax
.import DECOMPRESS_LZSA2, LZSA_DST, LZSA_SRC

PAGE1MEM = $2000
TILE_HEIGHT = 11
BOARD_HEIGHT = 15

;-----------------------------------------------------------------------------
.segment "HGR"
; Blank segment - space must just be reserved and warning is off with this

;-----------------------------------------------------------------------------
.segment "RODATA"

_splash_screen:
.incbin "Gomoku.lzh"

; Tables to look up every pixel row start address
rowL:
    .repeat $C0, Row
        .byte   Row & $08 << 4 | Row & $C0 >> 1 | Row & $C0 >> 3
    .endrep

rowH:
    .repeat $C0, Row
        .byte   >PAGE1MEM | Row & $07 << 2 | Row & $30 >> 4
    .endrep

times_tile_height:
    .repeat BOARD_HEIGHT+2, Row
        .byte Row * TILE_HEIGHT
    .endrep

;-----------------------------------------------------------------------------
; font masks
masksLeft:
.byte %01111111    ; MSB-BITS-11 White
.byte %00000000    ; 0-BITS-00 Black
.byte %11111111    ; 1-BITS-11 White
.byte %10000000    ; 1-BITS-00 Black

masksRight:
.byte %01111111    ; MSB-BITS-11 White
.byte %00000000    ; 0-BITS-00 Black
.byte %11111111    ; 1-BITS-11 White
.byte %10000000    ; 1-BITS-00 Black

;-----------------------------------------------------------------------------
.segment "CODE"
_asm_DetectMachine:
    lda	$c081      ; Read from rom
    lda	$c081
    sec
    jsr $fe1f      ; Detect the IIgs
    bcc iigs
    lda $fbb3      ; Look for a IIc+
    cmp #6         ; Possibly a IIc+ (or many other IIs)
    bne a2
    lda $fbbf      ; Specific to the IIc+
    cmp #5         ; when there's a 5 here
    bne a2
iicplus:
    lda #1         ; return 1 for a IIc+
    bne :+
iigs:
    lda #3         ; return 3 for a IIgs
    bne :+
a2:
    lda #0         ; return 0 for all other IIs
:
    ldx #0         ; cc65 returns are int's so clear hi
    rts 

;-----------------------------------------------------------------------------
_asm_ClearHiRes:
    ldy #0
    sty ptr1
    lda #$20
    sta ptr1+1
:
    lda #$2a
    sta (ptr1),y
    iny
    lda #$55
    sta (ptr1),y
    iny
    bne :-
    inc ptr1+1
    lda ptr1+1
    cmp #$40
    bne :-
    rts

;-----------------------------------------------------------------------------
_asm_ShowTile:
    sta src2+1     ; odd tile lo/hi comes in ax
    stx src2+2

    jsr popax
    sta src1+1     ; even tile lo/hi from stack
    stx src1+2

    jsr popa       ; col from stack
    asl            ; 2 bytes / col
    sta tmp2

    jsr popa       ; row from stack
    tay
    lda times_tile_height,y
    sta tmp1

    lda #TILE_HEIGHT
    sta tmp3       ; tile height
    ldx #0

loop:
    ldy tmp1       ; row
    lda rowL,y     ; row start address lo
    adc tmp2       ; add cols
    sta ptr1
    lda rowH,y     ; row hi
    sta ptr1+1     ; ptr1 now points at row/col to write

    ldy #0
src1:
    lda $ffff,x    ; load tile left
    sta (ptr1),y   ; write to screen
    iny            ; go to right
src2:
    lda $ffff,x    ; load tile right
    sta (ptr1),y   ; write to screen
    inx            ; next bytes in tile

    dec tmp3       ; one less row to do in tile
    beq done       ; until all rows done

    inc tmp1       ; next row
    bne loop

done:
    rts

;-----------------------------------------------------------------------------
_asm_ShowFont:
    sta charSrc1+1 ; odd half lo
    sta charSrc2+1 ; even half lo
    stx charSrc1+2 ; odd half hi
    stx charSrc2+2 ; even half hi

    jsr popa       ; color
    tay
    lda masksLeft,y
    sta ptr2
    lda masksRight,y
    sta ptr2+1

    jsr popa       ; col
    asl 
    sta tmp2

    jsr popa       ; row
    sta tmp1

    lda #8
    sta tmp3       ; font height
    ldx #0

font_loop:
    ldy tmp1
    lda rowL,y
    adc tmp2
    sta ptr1
    lda rowH,y
    sta ptr1+1

    ldy #0
charSrc1:
    lda $ffff,x
    pha
    and ptr2
    sta tmp4
    pla
    eor #$7f
    and #$2a
    ora tmp4
    sta (ptr1),y
    inx
    iny
charSrc2:
    lda $ffff,x
    pha
    and ptr2+1
    sta tmp4
    pla
    eor #$7f
    and #$55
    ora tmp4
    sta (ptr1),y

    dec tmp3
    bmi font_done

    inx
    inc tmp1
    bne font_loop

font_done:
    rts

;-----------------------------------------------------------------------------
_asm_InvertSquare:
    asl
    sta tmp2
    jsr popa
    tay 
    lda times_tile_height,y
    sta tmp1

    ldx #8         ; font height

invert_loop:
    ldy tmp1
    lda rowL,y
    adc tmp2
    sta ptr1
    lda rowH,y
    sta ptr1+1

    ldy #0
    lda (ptr1),y
    eor #$80
    sta (ptr1),y

    iny
    lda (ptr1),y
    eor #$80
    sta (ptr1),y

    inc tmp1       ; next row
    dex            ; one less line in character
    bpl invert_loop
    rts

;-----------------------------------------------------------------------------
_asm_DecompSplash:
    lda	#<_splash_screen
    sta	LZSA_SRC+1
    lda	#>_splash_screen
    sta	LZSA_SRC+2
    lda	#<PAGE1MEM
    sta	LZSA_DST+1
    lda	#>PAGE1MEM
    sta	LZSA_DST+2
    jmp	DECOMPRESS_LZSA2
