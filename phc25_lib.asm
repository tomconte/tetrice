SECTION code_compiler

; -----------------------------------------------------------------------------
; Decompress bitmaps for the UI
; -----------------------------------------------------------------------------

PUBLIC _decompress_ui_left

_decompress_ui_left:
        ld hl, ui_left
        ld de, 0xe000
        call dzx0_standard
        ret

PUBLIC _decompress_ui_right

_decompress_ui_right:
        ld hl, ui_right
        ld de, 0xe000
        call dzx0_standard
        ret

PUBLIC _decompress_ui_title

_decompress_ui_title:
        ld hl, ui_title
        ld de, 0xe000
        call dzx0_standard
        ret

PUBLIC _decompress_ui_bottom

_decompress_ui_bottom:
        ld hl, ui_bottom
        ld de, 0xe000
        call dzx0_standard
        ret

PUBLIC _decompress_splash

_decompress_splash:
        ld hl, splash
        ld de, 0xe000
        call dzx0_standard
        ret

PUBLIC _decompress_gameover

_decompress_gameover:
        ld hl, gameover
        ld de, 0xe000
        call dzx0_standard
        ret

; -----------------------------------------------------------------------------
; ZX0 decoder by Einar Saukas & Urusergi
; "Standard" version (68 bytes only)
; -----------------------------------------------------------------------------
; Parameters:
;   HL: source address (compressed data)
;   DE: destination address (decompressing)
; -----------------------------------------------------------------------------

PUBLIC dzx0_standard

dzx0_standard:
        ld      bc, $ffff               ; preserve default offset 1
        push    bc
        inc     bc
        ld      a, $80
dzx0s_literals:
        call    dzx0s_elias             ; obtain length
        ldir                            ; copy literals
        add     a, a                    ; copy from last offset or new offset?
        jr      c, dzx0s_new_offset
        call    dzx0s_elias             ; obtain length
dzx0s_copy:
        ex      (sp), hl                ; preserve source, restore offset
        push    hl                      ; preserve offset
        add     hl, de                  ; calculate destination - offset
        ldir                            ; copy from offset
        pop     hl                      ; restore offset
        ex      (sp), hl                ; preserve offset, restore source
        add     a, a                    ; copy from literals or new offset?
        jr      nc, dzx0s_literals
dzx0s_new_offset:
        pop     bc                      ; discard last offset
        ld      c, $fe                  ; prepare negative offset
        call    dzx0s_elias_loop        ; obtain offset MSB
        inc     c
        ret     z                       ; check end marker
        ld      b, c
        ld      c, (hl)                 ; obtain offset LSB
        inc     hl
        rr      b                       ; last offset bit becomes first length bit
        rr      c
        push    bc                      ; preserve new offset
        ld      bc, 1                   ; obtain length
        call    nc, dzx0s_elias_backtrack
        inc     bc
        jr      dzx0s_copy
dzx0s_elias:
        inc     c                       ; interlaced Elias gamma coding
dzx0s_elias_loop:
        add     a, a
        jr      nz, dzx0s_elias_skip
        ld      a, (hl)                 ; load another group of 8 bits
        inc     hl
        rla
dzx0s_elias_skip:
        ret     c
dzx0s_elias_backtrack:
        add     a, a
        rl      c
        rl      b
        jr      dzx0s_elias_loop

; -----------------------------------------------------------------------------
; Compressed bitmaps
; -----------------------------------------------------------------------------

ui_left:
        incbin "gfx/ui_left.bin.zx0"

ui_right:
        incbin "gfx/ui_right.bin.zx0"

ui_title:
        incbin "gfx/ui_title.bin.zx0"

ui_bottom:
        incbin "gfx/ui_bottom.bin.zx0"

splash:
        incbin "gfx/splash.bin.zx0"

gameover:
        incbin "gfx/gameover.bin.zx0"
